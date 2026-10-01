#pragma once

#include<string>
#include<vector>

enum class TokenType{
   FOR_LOOP,
   WHILE_LOOP,
   IF_COND,
   ELSE_COND,
   FUNC_DEF,
   BRACE_OPEN,
   BRACE_CLOSE,
   IDENTIFIER,
   UNKNOWN
};

struct Token{
   TokenType type;
   std:: string value;
};

class Tokenizer{
   public:
      static std::vector<Token> tokenize(const std::string&code);
};