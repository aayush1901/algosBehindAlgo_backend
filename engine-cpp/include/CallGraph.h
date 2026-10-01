#pragma once
#include<string>
#include<vector>
#include<unordered_map>
#include "json.hpp"

struct TopoResult{
   std::vector<std::string>topoOrder;
   bool hasCycle;
   int totalNodes;
};

class CallGraph {
public:
    static TopoResult analyzeCallGraph(const std::string& code);
};