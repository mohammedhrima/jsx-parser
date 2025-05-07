#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>

class Error : public std::exception
{
private:
   std::string message;

public:
   explicit Error(const std::string &message) : message(message) {}
   const char *what() const noexcept override { return message.c_str(); }
};

enum Type
{
   OPEN_TAG = '<', CLOSE_TAG = '>', SLASH = '/',
   LPAR = '(', RPAR = ')', LCBR = '{', RCBR = '}',
   LBR = '[', RBR = ']', ID = 'e', KEY = 's',
   FDEC, FCALL,
};

struct
{
   std::string value;
   Type type;
} Specials[] = {{"function", FDEC}, {"", (Type)0}};

struct Token
{
   std::string value;
   Type type;

   Token() = default;
   Token(const Token &) = default;
   Token(Token &&) = default;

   Token(Type type, std::string value = "") : type(type), value(std::move(value)) {}
   Token(char type) : type(static_cast<Type>(type)) {}
   Token(char type, std::string value) : type(static_cast<Type>(type)), value(std::move(value)) {}

   Token &operator=(const Token &) = default;
   Token &operator=(Token &&) = default;

   void setValue(std::string val)
   {
      value = std::move(val);
      for (int i = 0; Specials[i].type; i++)
      {
         if (Specials[i].value == value)
         {
            type = Specials[i].type;
            return;
         }
      }
   }
};

std::ostream &operator<<(std::ostream &out, Type type)
{
   out << "[";
   switch (type)
   {
   case FDEC: out << "FDEC"; break;
   case FCALL: out << "FCALL"; break;
   default: out << (char)type; break;
   }
   out << "]";
   return out;
}

std::ostream &operator<<(std::ostream &out, const Token &token)
{
   out << "token " << token.type;
   if (!token.value.empty()) out << " value: " << token.value;
   return out;
}

std::vector<Token> tokenize(const std::string &str)
{
   std::vector<Token> res;
   std::string special("<>/()[]{}");

   for (size_t i = 0; i < str.length();)
   {
      if (std::isspace(str[i]))
      {
         i++;
         continue;
      }

      if (special.find(str[i]) != std::string::npos)
      {
         res.emplace_back(str[i]);
         i++;
      }
      else
      {
         size_t s = i;
         while (
            i < str.size() && !std::isspace(str[i]) && 
            special.find(str[i]) == std::string::npos)
            i++;
         std::string word = str.substr(s, i - s);
         res.emplace_back(ID, word);
      }
   }
   return res;
}

std::string readFileToString(const std::string &filePath)
{
   std::ifstream file(filePath, std::ios::binary);
   if (!file)
      throw std::runtime_error("Failed to open file: " + filePath);

   file.seekg(0, std::ios::end);
   std::streamsize size = file.tellg();
   file.seekg(0, std::ios::beg);

   std::string content;
   content.resize(size);
   file.read(&content[0], size);

   return content;
}



int main()
{
   try
   {
      std::string content = readFileToString("home.jsx");
      std::vector<Token> res = tokenize(content);
      for (auto &e : res)
         std::cout << e << std::endl;
      

   }
   catch (const std::exception &e)
   {
      std::cerr << "Error: " << e.what() << std::endl;
      return 1;
   }
   return 0;
}