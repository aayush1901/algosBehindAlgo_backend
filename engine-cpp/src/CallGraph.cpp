#include "CallGraph.h"
#include <queue>
#include <regex>

TopoResult CallGraph::analyzeCallGraph(const std::string& code) {
    std::unordered_map<std::string, std::vector<std::string>> adjList;
    std::unordered_map<std::string, int> inDegree;
    std::vector<std::string> allNodes;

    // Naya Regex: Yeh dekhta hai ki aage return type (int/void) hai ya nahi.
    // Agar 'void helper(' hai -> toh yeh Definition hai.
    // Agar sirf 'helper(' hai -> toh yeh Call (Edge) hai.
    std::regex funcRegex(R"((void|int|string|auto|const\s+)?\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*\()");
    
    std::sregex_iterator next(code.begin(), code.end(), funcRegex);
    std::sregex_iterator end;

    std::string currentParent = "global";

    while (next != end) {
        std::smatch match = *next;
        std::string returnType = match[1].str(); // 'void ', 'int ', etc.
        std::string funcName = match[2].str();   // 'helper', 'solve', etc.

        // Ignore C++ loops/conditions
        if (funcName == "for" || funcName == "while" || funcName == "if" || funcName == "switch") {
            next++;
            continue;
        }

        // Initialize node in graph
        if (inDegree.find(funcName) == inDegree.end()) {
            inDegree[funcName] = 0;
            allNodes.push_back(funcName);
        }

        // Agar return type hai, matlab yahan naya function DECLARE ho raha hai
        if (returnType.length() > 0) {
            currentParent = funcName; // Ab se jo calls hongi, wo iske andar maani jayengi
        } 
        // Agar return type nahi hai, aur parent global nahi hai, matlab function CALL ho raha hai
        else if (currentParent != "global" && currentParent != funcName) {
            adjList[currentParent].push_back(funcName);
            inDegree[funcName]++;
        }
        
        next++;
    }

    // ==========================================
    // KAHN'S ALGORITHM (TOPOLOGICAL SORT)
    // ==========================================
    std::queue<std::string> q;
    for (const std::string& node : allNodes) {
        if (inDegree[node] == 0) q.push(node);
    }

    std::vector<std::string> topoOrder;
    while (!q.empty()) {
        std::string curr = q.front();
        q.pop();
        topoOrder.push_back(curr);

        for (const std::string& neighbor : adjList[curr]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) q.push(neighbor);
        }
    }

    bool hasCycle = (topoOrder.size() < allNodes.size());
    return {topoOrder, hasCycle, (int)allNodes.size()};
}