#include "UClassImageRegistry.h"
#include "UProperty.h"
#include "UComponentFactory.h"
#include <cmath>
#include <map>
#include <set>
#include <sstream>
#include <typeinfo>

namespace RDK {
namespace {
struct Node { UEPtr<UContainer> component; std::string path; };

void Append(std::string& text, const std::string& value)
{
 text += std::to_string(value.size()) + ":" + value;
}

void Collect(UEPtr<UContainer> component, const std::string& path,
             std::vector<Node>& nodes, std::set<const UContainer*>& seen)
{
 if(!component || !seen.insert(component.Get()).second)
  throw ClassImageError("invalid_graph", path);
 nodes.push_back({component, path});
 std::vector<NameT> children;
 component->GetComponentsList(children);
 for(const auto& name : children)
  Collect(component->GetComponent(name, true), path + "." + name, nodes, seen);
}

void ValidateFinite(const UIProperty* property, const std::string& path)
{
 if(auto value = dynamic_cast<const UVBaseDataProperty<double>*>(property))
 {
  if(!std::isfinite(value->GetData())) throw ClassImageError("nonfinite_parameter", path);
 }
 if(auto value = dynamic_cast<const UVBaseDataProperty<std::vector<double>>*>(property))
  for(double element : value->GetData())
   if(!std::isfinite(element)) throw ClassImageError("nonfinite_parameter", path);
 if(auto value = dynamic_cast<const UVBaseDataProperty<MDMatrix<double>>*>(property))
 {
  const auto& matrix = value->GetData();
  for(size_t i = 0; i < matrix.GetSize(); ++i)
   if(!std::isfinite(matrix.Data[i])) throw ClassImageError("nonfinite_parameter", path);
 }
}

struct Lease
{
 UEPtr<UStorage> storage;
 UEPtr<UContainer> object;
 ~Lease() { if(object) storage->ReturnObject(object); }
};
}

std::string DescribeClassImage(UEPtr<UContainer> prototype)
{
 if(!prototype) throw ClassImageError("invalid_candidate", "null image");
 std::vector<Node> nodes;
 std::set<const UContainer*> seen;
 Collect(prototype, "$", nodes, seen);
 std::map<const UIProperty*, std::string> propertyPaths;
 for(const auto& node : nodes)
  for(const auto& property : node.component->GetPropertiesList())
   propertyPaths[property.second.Property.Get()] = node.path + ":" + property.first;

 std::string result = "RdkClassImage-v1";
 for(const auto& node : nodes)
 {
  Append(result, "node"); Append(result, node.path);
  Append(result, typeid(*node.component.Get()).name());
  // The root's storage alias is assigned by registration, not by its physics.
  if(node.component != prototype) Append(result, node.component->GetCompClassName());
  for(const auto& name : node.component->GetPropertiesOrder())
  {
   auto property = node.component->FindProperty(name);
   if(!property) throw ClassImageError("invalid_graph", node.path + ":" + name);
   if((property->GetType() & ptParameter) && name != "Name" && name != "Id")
   {
    ValidateFinite(property.Get(), node.path + ":" + name);
    std::string value;
    node.component->GetPropertyValue(name, value);
    Append(result, "parameter"); Append(result, name);
    Append(result, std::to_string(property->GetType())); Append(result, value);
   }
   if(auto input = dynamic_cast<const UIPropertyInput*>(property.Get()))
   {
    const auto sources = input->GetDataSources();
    if((input->IsConnected() || input->UsesExternalDataSource()) &&
       (!input->CanInspectDataSources() || sources.empty()))
     throw ClassImageError("uninspectable_binding", node.path + ":" + name);
    for(size_t i = 0; i < sources.size(); ++i)
    {
     auto source = propertyPaths.find(sources[i]);
     if(source == propertyPaths.end())
      throw ClassImageError("external_or_null_binding", node.path + ":" + name);
     Append(result, "binding"); Append(result, name); Append(result, std::to_string(i));
     Append(result, input->UsesExternalDataSource() ? "external" : "pointer");
     Append(result, source->second);
    }
   }
  }
 }
 return result;
}

ClassImageRegistration RegisterClassImage(UEPtr<UStorage> storage,
 const std::string& name, UEPtr<UContainer> prototype)
{
 if(!storage || !prototype || name.empty() || prototype->GetStorage() != storage ||
    prototype->GetOwner() || !prototype->IsReady() || !storage->CheckClass(prototype->GetClass()))
  throw ClassImageError("invalid_candidate", name);
 const auto expected = DescribeClassImage(prototype);
 if(storage->CheckClass(name))
 {
  auto factory = dynamic_pointer_cast<UVirtualMethodFactory>(storage->GetComponentFactory(name));
  if(!factory) throw ClassImageError("unsupported_existing_factory", name);
  if(DescribeClassImage(factory->GetComponent()) != expected)
   throw ClassImageError("image_name_collision", name);
  return {false, expected};
 }

 // Alloc/Copy are used only for parameters/structure, never as a dynamic checkpoint.
 Lease lease{storage, prototype->Alloc(storage, false)};
 if(!lease.object) throw ClassImageError("native_copy_failed", name);
 lease.object->SetEnvironment(prototype->GetEnvironment());
 lease.object->Init();
 if(!lease.object->IsInit() || !lease.object->IsReady() || DescribeClassImage(lease.object) != expected)
  throw ClassImageError("native_copy_mismatch", name);
 std::vector<Node> sourceNodes, copyNodes;
 std::set<const UContainer*> sourcePointers, copyPointers;
 Collect(prototype, "$", sourceNodes, sourcePointers);
 Collect(lease.object, "$", copyNodes, copyPointers);
 for(const auto* pointer : copyPointers)
  if(sourcePointers.count(pointer)) throw ClassImageError("native_copy_alias", name);

 // The factory owns a separate copy. Returning/reusing the caller's object
 // cannot overwrite the registered image. No existing factory is replaced.
 UEPtr<UComponentAbstractFactory> factory(new UVirtualMethodFactory(lease.object));
 lease.object = 0;
 storage->AddClass(factory, name);
 return {true, expected};
}
}
