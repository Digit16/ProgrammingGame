#pragma once

#include <map>
#include <string>

extern std::map<std::string, std::variant<int, float, bool>> GLOBAL_SCOPE;
extern std::unordered_map<std::string, std::vector<std::shared_ptr<AstNode>>> GLOBAL_FUNCTIONS;