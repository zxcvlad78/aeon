#pragma once

#include <string>
#include <vector>

struct ItemData {
    std::string id;
    std::string name;
    std::string description;
    uint32_t max_stack = 64;
    float weight = 0.1f;
    
    std::vector<std::string> tags; 
};