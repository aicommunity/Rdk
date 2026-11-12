#ifndef RDK_ULLM_TASK_PLAN_PARSING_H
#define RDK_ULLM_TASK_PLAN_PARSING_H

#include <string>
#include <vector>

namespace RDK::LLM {

struct ClassAddSpec {
    std::string class_name;
    int count = 1;
};

/// Parses "add 2 Foo and 3 Bar" / «добавь 2 Foo и 3 Bar» into per-class counts.
/// Returns empty when no digit+class pairs found (caller uses single-class fallback).
std::vector<ClassAddSpec> extractClassAddSpecsFromGoal(const std::string& goal_en);

bool isAddComponentGoal(const std::string& goal_en);

} // namespace RDK::LLM

#endif
