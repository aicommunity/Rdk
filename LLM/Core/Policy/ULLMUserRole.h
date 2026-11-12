#ifndef RDK_ULLM_USER_ROLE_H
#define RDK_ULLM_USER_ROLE_H

namespace RDK::LLM {

enum class LLMUserRole { Guest, Operator, Admin };

inline LLMUserRole resolveUserRole(int user_id)
{
    if(user_id < 0)
        return LLMUserRole::Guest;
    if(user_id >= 1000)
        return LLMUserRole::Admin;
    return LLMUserRole::Operator;
}

inline const char* userRoleName(LLMUserRole role)
{
    switch(role)
    {
    case LLMUserRole::Guest:
        return "guest";
    case LLMUserRole::Operator:
        return "operator";
    case LLMUserRole::Admin:
        return "admin";
    }
    return "unknown";
}

} // namespace RDK::LLM

#endif
