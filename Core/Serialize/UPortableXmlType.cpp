#include "UPortableXmlType.h"
#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace RDK { namespace {
enum class Kind { Scalar, Vector, Allocator, Matrix, MVector, Array, Prefix, Space };
enum class Scalar { Bool, Char, SChar, UChar, Short, UShort, Int, UInt, Long, ULong,
                    LongLong, ULongLong, Float, Double, LongDouble, String };
struct Type;
using T=std::shared_ptr<const Type>;
struct Type {
    Kind kind;Scalar scalar=Scalar::Bool;T element;std::string name;std::size_t count=0;
};
T Node(Kind kind,T element={},std::string name={},std::size_t count=0)
{auto p=std::make_shared<Type>();p->kind=kind;p->element=std::move(element);p->name=std::move(name);p->count=count;return p;}
T Atom(Scalar scalar)
{auto p=std::make_shared<Type>();p->kind=Kind::Scalar;p->scalar=scalar;return p;}
bool Same(T a,T b)
{
    if(!a || !b || a->kind!=b->kind || a->scalar!=b->scalar || a->name!=b->name || a->count!=b->count)return false;
    return !a->element && !b->element || (a->element && b->element && Same(a->element,b->element));
}
T ScalarName(const std::string& name)
{
    static const std::map<std::string,Scalar> names={
        {"b",Scalar::Bool},{"bool",Scalar::Bool},{"c",Scalar::Char},{"char",Scalar::Char},
        {"a",Scalar::SChar},{"signed char",Scalar::SChar},{"h",Scalar::UChar},{"unsigned char",Scalar::UChar},
        {"s",Scalar::Short},{"short",Scalar::Short},{"short int",Scalar::Short},
        {"t",Scalar::UShort},{"unsigned short",Scalar::UShort},{"unsigned short int",Scalar::UShort},
        {"i",Scalar::Int},{"int",Scalar::Int},{"j",Scalar::UInt},{"unsigned int",Scalar::UInt},
        {"l",Scalar::Long},{"long",Scalar::Long},{"long int",Scalar::Long},
        {"m",Scalar::ULong},{"unsigned long",Scalar::ULong},{"unsigned long int",Scalar::ULong},
        {"x",Scalar::LongLong},{"long long",Scalar::LongLong},{"long long int",Scalar::LongLong},{"__int64",Scalar::LongLong},
        {"y",Scalar::ULongLong},{"unsigned long long",Scalar::ULongLong},
        {"unsigned long long int",Scalar::ULongLong},{"unsigned __int64",Scalar::ULongLong},
        {"f",Scalar::Float},{"float",Scalar::Float},{"d",Scalar::Double},{"double",Scalar::Double},
        {"e",Scalar::LongDouble},{"long double",Scalar::LongDouble},{"std::string",Scalar::String}};
    const auto i=names.find(name);return i==names.end()?T{}:Atom(i->second);
}
T Prefix(const std::string& name)
{
    if(name=="std::vector" || name=="std::allocator" || name=="RDK::MDMatrix" || name=="RDK::MVector")
        return Node(Kind::Prefix,{},name);
    return {};
}
T Instantiate(T prefix,const std::vector<T>& args)
{
    if(!prefix || prefix->kind!=Kind::Prefix || args.empty() || !args[0])return {};
    if(prefix->name=="std::vector")
    {
        if(args.size()>2 || (args.size()==2 && (!args[1] || args[1]->kind!=Kind::Allocator || !Same(args[0],args[1]->element))))return {};
        return Node(Kind::Vector,args[0]);
    }
    if(args.size()!=1)return {};
    if(prefix->name=="std::allocator")return Node(Kind::Allocator,args[0]);
    return Node(prefix->name=="RDK::MDMatrix"?Kind::Matrix:Kind::MVector,args[0]);
}
class TextParser {
    const std::string& text;std::size_t at=0;
    void Space(){while(at<text.size() && std::isspace(static_cast<unsigned char>(text[at])))++at;}
    bool Word(const char* word)
    {
        Space();const std::string value(word);
        if(text.compare(at,value.size(),value)!=0 || (at+value.size()<text.size() &&
           !std::isspace(static_cast<unsigned char>(text[at+value.size()]))))return false;
        at+=value.size();Space();return true;
    }
    T Read(unsigned depth)
    {
        if(depth>32)return {};Space();const bool elaborated=Word("class") || Word("struct");
        const auto begin=at;
        while(at<text.size() && text[at]!='<' && text[at]!=',' && text[at]!='>' && text[at]!='[')++at;
        auto name=text.substr(begin,at-begin);while(!name.empty() && std::isspace(static_cast<unsigned char>(name.back())))name.pop_back();
        T result=ScalarName(name);if(result && elaborated)return {};
        if(!result)
        {
            if(name=="MDMatrix" || name=="MVector")name="RDK::"+name;
            auto prefix=Prefix(name);if(!prefix || at==text.size() || text[at]!='<')return {};
            ++at;std::vector<T> args;
            for(;;)
            {
                auto arg=Read(depth+1);if(!arg)return {};args.push_back(arg);Space();
                if(at==text.size())return {};
                if(text[at]=='>'){++at;break;}
                if(text[at++]!=',' || args.size()>2)return {};
            }
            result=Instantiate(prefix,args);
        }
        Space();std::vector<std::size_t> bounds;
        while(at<text.size() && text[at]=='[')
        {
            ++at;Space();std::size_t n=0;const auto start=at;
            while(at<text.size() && std::isdigit(static_cast<unsigned char>(text[at])))
            {const auto digit=unsigned(text[at++]-'0');if(n>(std::numeric_limits<std::size_t>::max()-digit)/10)return {};n=n*10+digit;}
            Space();if(start==at || !n || at==text.size() || text[at++]!=']' || bounds.size()>=32)return {};bounds.push_back(n);Space();
        }
        for(auto i=bounds.rbegin();i!=bounds.rend();++i)result=Node(Kind::Array,result,{},*i);
        return result;
    }
public:
    explicit TextParser(const std::string& value):text(value){}
    T Parse(){auto result=Read(0);Space();return at==text.size()?result:T{};}
};
class ItaniumParser {
    const std::string& text;std::size_t at=0;std::vector<T> substitutions;
    bool Add(T value){if(!value || substitutions.size()>=256)return false;substitutions.push_back(value);return true;}
    bool Take(char value){if(at<text.size() && text[at]==value){++at;return true;}return false;}
    std::string Identifier()
    {
        std::size_t size=0;const auto begin=at;
        while(at<text.size() && std::isdigit(static_cast<unsigned char>(text[at])))
        {const auto d=unsigned(text[at++]-'0');if(size>(text.size()-std::min<std::size_t>(d,text.size()))/10)return {};size=size*10+d;}
        if(at==begin || !size || size>text.size()-at)return {};const auto value=text.substr(at,size);at+=size;return value;
    }
    T Substitute()
    {
        if(!Take('S'))return {};
        if(Take('_'))return substitutions.empty()?T{}:substitutions[0];
        std::size_t n=0;const auto begin=at;
        while(at<text.size() && text[at]!='_')
        {
            const auto c=text[at++];const int d=c>='0' && c<='9'?c-'0':c>='A' && c<='Z'?c-'A'+10:-1;
            if(d<0 || n>256)return {};n=n*36+unsigned(d);
        }
        if(at==begin || !Take('_') || n+1>=substitutions.size())return {};return substitutions[n+1];
    }
    T Apply(T prefix,unsigned depth)
    {
        if(!prefix || !Take('I'))return {};std::vector<T> args;
        while(at<text.size() && text[at]!='E')
        {auto value=Read(depth+1);if(!value || args.size()==2)return {};args.push_back(value);}
        if(!Take('E'))return {};auto result=Instantiate(prefix,args);return Add(result)?result:T{};
    }
    T Read(unsigned depth)
    {
        if(depth>32 || at==text.size())return {};
        if(text.compare(at,2,"St")==0)
        {
            at+=2;auto prefix=Prefix("std::"+Identifier());if(!Add(prefix))return {};return Apply(prefix,depth);
        }
        if(text.compare(at,2,"Sa")==0){at+=2;return Apply(Prefix("std::allocator"),depth);}
        if(text[at]=='S')
        {auto value=Substitute();return value && value->kind==Kind::Prefix?Apply(value,depth):value;}
        if(Take('N'))
        {
            T prefix;
            if(at<text.size() && text[at]=='S')prefix=Substitute();
            else if(Identifier()=="RDK"){prefix=Node(Kind::Space,{},"RDK");if(!Add(prefix))return {};}
            if(!prefix)return {};
            if(prefix->kind==Kind::Space)
            {prefix=Prefix(prefix->name+"::"+Identifier());if(!Add(prefix))return {};}
            auto result=Apply(prefix,depth);return Take('E')?result:T{};
        }
        if(Take('A'))
        {
            const auto begin=at;std::size_t count=0;
            while(at<text.size() && std::isdigit(static_cast<unsigned char>(text[at])))
            {const auto d=unsigned(text[at++]-'0');if(count>(std::numeric_limits<std::size_t>::max()-d)/10)return {};count=count*10+d;}
            if(at==begin || !count || !Take('_'))return {};auto element=Read(depth+1);auto result=element?Node(Kind::Array,element,{},count):T{};return Add(result)?result:T{};
        }
        const auto scalar=ScalarName(text.substr(at++,1));return scalar;
    }
public:
    explicit ItaniumParser(const std::string& value):text(value){}
    T Parse(){auto result=Read(0);return at==text.size()?result:T{};}
};
T Decode(const std::string& text)
{
    if(text.empty() || text.size()>65536)return {};
    auto result=TextParser(text).Parse();return result?result:ItaniumParser(text).Parse();
}
int SignedBits(T type)
{
    if(!type || type->kind!=Kind::Scalar)return 0;
    switch(type->scalar){case Scalar::SChar:return 8;case Scalar::Short:return 16;
        case Scalar::Int:case Scalar::Long:return 32;case Scalar::LongLong:return 64;default:return 0;}
}
int UnsignedBits(T type)
{
    if(!type || type->kind!=Kind::Scalar)return 0;
    switch(type->scalar){case Scalar::UChar:return 8;case Scalar::UShort:return 16;
        case Scalar::UInt:case Scalar::ULong:return 32;case Scalar::ULongLong:return 64;default:return 0;}
}
bool IntegralValue(T left,T right,UXmlIntegralDomain domain,const std::string& value)
{
    // A legacy long tag does not specify LP64/LLP64. For a cross-nominal value
    // comparison use the common guaranteed range, never guess a larger ABI.
    const auto bits=std::min(domain==UXmlIntegralDomain::SignedValue?SignedBits(left):UnsignedBits(left),
                             domain==UXmlIntegralDomain::SignedValue?SignedBits(right):UnsignedBits(right));
    if(!bits || value.empty())return false;
    std::size_t begin=value[0]=='-' && domain==UXmlIntegralDomain::SignedValue?1:0;
    if(begin==value.size() || !std::all_of(value.begin()+begin,value.end(),[](unsigned char c){return std::isdigit(c)!=0;}))return false;
    errno=0;char* end=nullptr;
    if(domain==UXmlIntegralDomain::SignedValue)
    {
        const auto n=std::strtoll(value.c_str(),&end,10);if(errno==ERANGE || end!=value.c_str()+value.size())return false;
        if(bits==64)return true;const auto bound=1LL<<(bits-1);return n>=-bound && n<bound;
    }
    const auto n=std::strtoull(value.c_str(),&end,10);return errno!=ERANGE && end==value.c_str()+value.size() &&
        (bits==64 || n<(1ULL<<bits));
}
} // namespace

bool SamePortableXmlType(const std::string& first,const std::string& second,
    UXmlIntegralDomain domain,const std::string& value)
{
    const auto a=Decode(first),b=Decode(second);
    if(domain!=UXmlIntegralDomain::Nominal)
    {
        if(!a || !b)return false;
        // Same nominal long may contain a valid native64 value; its owner
        // still validates typed readback. The conservative intersection is
        // needed only when comparing different nominal integer types.
        if(Same(a,b) && ((domain==UXmlIntegralDomain::SignedValue && SignedBits(a)) ||
                        (domain==UXmlIntegralDomain::UnsignedValue && UnsignedBits(a))))
        {
            auto wide=Atom(domain==UXmlIntegralDomain::SignedValue?Scalar::LongLong:Scalar::ULongLong);
            return IntegralValue(wide,wide,domain,value);
        }
        return IntegralValue(a,b,domain,value);
    }
    if(first==second)return true;
    return Same(a,b);
}
} // namespace RDK
