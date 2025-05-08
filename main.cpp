#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <memory>
#include <iomanip>


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
   NONE = 0,
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
   Type type = NONE;

   Token() = default;
   Token(const Token &) = default;
   Token(Token &&) = default;

   Token(Type type, std::string value = "")
   {
      // std::cout <<  __LINE__ << ": new token has " << type << std::endl;
      this->type = type;
      this->value = std::move(value); 
   }
   Token(char type, std::string value)
   {
      // std::cout <<  __LINE__ << ": new token has " << type << std::endl;
      this->type = static_cast<Type>(type);
      this->value = std::move(value);
   }
   Token(char type)
   {
      // std::cout <<  __LINE__ << ": new token has " << type << std::endl;
      this->type = (Type)type;
   }
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

std::vector<Token> Tokens;
void tokenize(const std::string &str)
{
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
         Tokens.emplace_back(str[i]);
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
         Tokens.emplace_back(ID, word);
      }
   }
}

std::string readFileToString(const std::string &filePath)
{
   std::ifstream file(filePath, std::ios::binary);
   if (!file) throw std::runtime_error("Failed to open file: " + filePath);

   file.seekg(0, std::ios::end);
   std::streamsize size = file.tellg();
   file.seekg(0, std::ios::beg);

   std::string content;
   content.resize(size);
   file.read(&content[0], size);

   return content;
}

struct Node 
{
   std::unique_ptr<Token> token;
   std::unique_ptr<Node> left;
   std::unique_ptr<Node> right;

   Node(){};
   ~Node(){}
   explicit Node(Token *token) : token(token) {};

   // Disable copying
   Node(const Node &) = delete;
   Node &operator=(const Node &) = delete;

   // Enable moving
   Node(Node &&) = default;
   Node &operator=(Node &&) = default;

   void add_left(std::unique_ptr<Node> node)
   {
      left = std::move(node);
   }

   void add_right(std::unique_ptr<Node> node)
   {
      right = std::move(node);
   }
};

void print_node(std::ostream &out, const Node *node, int indent = 0)
{
   if (!node) return;
   out << std::setw(indent) << ' ';
   if (node->token) out << *node->token;
   else out << "null";
   out << '\n';
   print_node(out, node->left.get(), indent + 4);
   print_node(out, node->right.get(), indent + 4);
}

std::ostream &operator<<(std::ostream &out, const Node &node)
{
   print_node(out, &node);
   return out;
}

void parse()
{
   
}

int main()
{
   try
   {
      tokenize(readFileToString("home.jsx"));
      for (auto &e : Tokens) std::cout << e << std::endl;
   }
   catch (const std::exception &e)
   {
      std::cerr << "Error: " << e.what() << std::endl;
      return 1;
   }
   return 0;
}