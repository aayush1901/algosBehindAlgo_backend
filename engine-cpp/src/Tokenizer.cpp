#include "Tokenizer.h"
#include<sstream>  // to break string in the tokens
#include<cctype>

std::vector<Token> Tokenizer ::tokenize(const std::string &code){
   std::vector<Token> tokens;

   size_t i = 0;

   while(i<code.length()){
      char c = code[i];

      if(isspace(c)){
         i++;
         continue;
      }

      if (c == '{') {
            tokens.push_back({TokenType::BRACE_OPEN, "{"});
            i++;
            continue;
        }
      if (c == '}') {
            tokens.push_back({TokenType::BRACE_CLOSE, "}"});
            i++;
            continue;
        }

      if (isalpha(c)) {
            std::string word = "";
            
            while (i < code.length() && (isalnum(code[i]) || code[i] == '_')) {
                word += code[i];
                i++;
            }
            if (word == "for") tokens.push_back({TokenType::FOR_LOOP, word});
            else if (word == "while") tokens.push_back({TokenType::WHILE_LOOP, word});
            else if (word == "if") tokens.push_back({TokenType::IF_COND, word});
            else if (word == "else") tokens.push_back({TokenType::ELSE_COND, word});
            else tokens.push_back({TokenType::IDENTIFIER, word}); // Normal variable naam
            
            continue;
      }

      i++;
   }

   return tokens;
}