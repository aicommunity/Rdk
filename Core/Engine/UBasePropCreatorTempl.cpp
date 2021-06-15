#include "UBasePropCreatorTempl.h"
#include "UComponent.h"


namespace RDK {


UBasePropCreatorTempl::UBasePropCreatorTempl()
{

}

const std::vector<std::string>& UBasePropCreatorTempl::GetForbiddenInputs()
{
    static std::vector<std::string> ForbiddenInputs;
    if(ForbiddenInputs.empty())
    {
     ForbiddenInputs.push_back("DataInput0");
     ForbiddenInputs.push_back("DataInput1");
     ForbiddenInputs.push_back("DataInput2");
     ForbiddenInputs.push_back("DataInput3");
     ForbiddenInputs.push_back("DataInput4");
     ForbiddenInputs.push_back("DataInput5");
    }

    return ForbiddenInputs;
}

const std::vector<std::string>& UBasePropCreatorTempl::GetForbiddenOutputs()
{
    static std::vector<std::string> ForbiddenOutputs;
    if(ForbiddenOutputs.empty())
    {
     ForbiddenOutputs.push_back("DataOutput0");
     ForbiddenOutputs.push_back("DataOutput1");
     ForbiddenOutputs.push_back("DataOutput2");
     ForbiddenOutputs.push_back("DataOutput3");
     ForbiddenOutputs.push_back("DataOutput4");
     ForbiddenOutputs.push_back("DataOutput5");
    }

    return ForbiddenOutputs;
}


}
