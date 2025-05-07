#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>

using namespace std;

// Token types
enum TokenType
{
    NUMBER,
    IDENTIFIER,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    EQUAL,
    LPAREN,
    RPAREN,
    END
};

// Token structure
struct Token
{
    TokenType type;
    string value;
    Token(TokenType t, string v = "") : type(t), value(v) {}
};

// Parser class
class Parser
{
    string input;
    size_t pos = 0;
    Token current_token = {END};
    unordered_map<string, double> variables;

    Token get_next_token()
    {
        while (pos < input.size() && isspace(input[pos]))
            pos++;
        if (pos >= input.size())
            return {END};

        char c = input[pos];
        if (isdigit(c))
        {
            string num;
            while (pos < input.size() && isdigit(input[pos]))
                num += input[pos++];
            return {NUMBER, num};
        }
        if (isalpha(c))
        {
            string id;
            while (pos < input.size() && isalnum(input[pos]))
                id += input[pos++];
            return {IDENTIFIER, id};
        }
        pos++;
        switch (c)
        {
        case '+': return {PLUS};
        case '-': return {MINUS};
        case '*': return {STAR};
        case '/': return {SLASH};
        case '=': return {EQUAL};
        case '(': return {LPAREN};
        case ')': return {RPAREN};
        default: throw runtime_error("Invalid character");
        }
    }

    void eat(TokenType type)
    {
        if (current_token.type == type)
            current_token = get_next_token();
        else
            throw runtime_error("Unexpected token");
    }

    double factor()
    {
        Token token = current_token;
        if (token.type == NUMBER)
        {
            eat(NUMBER);
            return stod(token.value);
        }
        if (token.type == IDENTIFIER)
        {
            eat(IDENTIFIER);
            return variables[token.value];
        }
        if (token.type == LPAREN)
        {
            eat(LPAREN);
            double result = expr();
            eat(RPAREN);
            return result;
        }
        throw runtime_error("Syntax error in factor");
    }

    double term()
    {
        double result = factor();
        while (current_token.type == STAR || current_token.type == SLASH)
        {
            Token op = current_token;
            if (op.type == STAR)
            {
                eat(STAR);
                result *= factor();
            }
            else
            {
                eat(SLASH);
                result /= factor();
            }
        }
        return result;
    }

    double expr()
    {
        double result = term();
        while (current_token.type == PLUS || current_token.type == MINUS)
        {
            Token op = current_token;
            if (op.type == PLUS)
            {
                eat(PLUS);
                result += term();
            }
            else
            {
                eat(MINUS);
                result -= term();
            }
        }
        return result;
    }

    double assignment()
    {
        if (current_token.type == IDENTIFIER && pos < input.size() && input[pos] == '=')
        {
            string var_name = current_token.value;
            eat(IDENTIFIER);
            eat(EQUAL);
            double value = expr();
            variables[var_name] = value;
            return value;
        }
        return expr();
    }

public:
    double parse(const string &s)
    {
        input = s;
        pos = 0;
        current_token = get_next_token();
        return assignment();
    }
};

int main()
{
    Parser parser;
    string line;
    while (cout << "> ", getline(cin, line))
    {
        try
        {
            double result = parser.parse(line);
            cout << "= " << result << endl;
        }
        catch (const exception &e)
        {
            cerr << "Error: " << e.what() << endl;
        }
    }
    return 0;
}