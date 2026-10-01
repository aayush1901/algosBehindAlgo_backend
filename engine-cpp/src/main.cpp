#include<iostream>
#include<fstream>
#include<cstdlib>
#include "httplib.h"
#include "json.hpp"
#include "Tokenizer.h"
#include "CallGraph.h"

using json = nlohmann::json;
using namespace std;

// ==========================================
// PHASE 0: Compiler Syntax Checker
// ==========================================

bool checkCompilation(string &code){
   ofstream out("temp_user_code.cpp");
   out << code;
   out.close();

   out.close();

   string command = "g++ -fsyntax-only temp_user_code.cpp 2> /dev/null";

   int result = system(command.c_str());
    remove("temp_user_code.cpp");

    return (result == 0);
}

int main(){
   httplib::Server svr;

   // Route: POST /internal/analyze-dsa

   svr.Post("/internal/analyze-dsa", [](const httplib::Request& req, httplib::Response& res) {
        try {
            
            auto body = json::parse(req.body);
            string sourceCode = body["sourceCode"];
            string language = body.value("language", "cpp"); 

            // phase 0 -> checking if code is compilable
            if (language == "cpp" && !checkCompilation(sourceCode)) {
                json errorRes;
                errorRes["success"] = false;
                errorRes["error"] = "Compilation Error: Code contains syntax bugs. Cannot analyze.";
                res.set_content(errorRes.dump(), "application/json");
                return;
            }

            // phase 1 -> tokenization of the entire source code which we recieve as a string in http form(i may change it later to use grpc)
            vector<Token> tokens = Tokenizer::tokenize(sourceCode);
            
            
            json jsonTokens = json::array();
            for (const auto& token : tokens) {
                jsonTokens.push_back({
                    {"type", static_cast<int>(token.type)},
                    {"value", token.value}
                });
            }

            
            // json finalResponse;
            // finalResponse["success"] = true;
            // finalResponse["phase0_syntax_check"] = "PASSED";
            // finalResponse["tokens_extracted"] = tokens.size();
            // finalResponse["tokens"] = jsonTokens;
            // res.status = 400;

            // 4. PHASE 2: Call Graph & Topological Sort
      TopoResult topoData = CallGraph::analyzeCallGraph(sourceCode);

      json callGraphJson;
      callGraphJson["topoOrder"] = topoData.topoOrder;
      callGraphJson["hasRecursionCycle"] = topoData.hasCycle;
      callGraphJson["totalFunctionsDetected"] = topoData.totalNodes;

      json finalResponse;
      finalResponse["success"] = true;
      finalResponse["phase0_syntax_check"] = "PASSED";
      finalResponse["tokens_extracted"] = tokens.size();
      finalResponse["callGraph"] = callGraphJson; // Naya data yahan attach kiya!

      res.set_content(finalResponse.dump(), "application/json");

            
      // res.set_content(finalResponse.dump(), "application/json");

        } catch (const std::exception& e) {
            // Agar JSON parsing fail hoti hai
            json errorRes;
            errorRes["success"] = false;
            errorRes["error"] = e.what();
            res.set_content(errorRes.dump(), "application/json");
        }
    });

    std::cout << "[C++ Engine] Starting server on http://localhost:8080" << std::endl;
    
    // Server ko port 8080 par sunne ke liye chalu karo
    svr.listen("0.0.0.0", 8080);

    return 0;
}

