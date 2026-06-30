#pragma once

#include "AST.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>

namespace modelchecklib::algorithms::ltl {

    enum class TokenType {
        ATOM,

        // boolean values
        TRUE_VALUE,
        FALSE_VALUE,

        // boolean operation flags
        NOT,
        AND,
        OR,
        IMPLIES,

        // ltl operation flags
        NEXT,
        GLOBALLY,
        EVENTUALLY,
        UNTIL,
        RELEASE,
        
        // left and right parenthesis
        LPAREN,
        RPAREN,

        END_OF_STRING
    };

    struct Token {
        TokenType type;
        std::string value;
    };

    class Lexer {
        private:
            std::string input_;
            size_t pos_ = 0;

            char peek() const { return pos_ < input_.length() ? input_[pos_] : '\0'; }
            char consume() { return pos_ < input_.length() ? input_[pos_++] : '\0'; }

        public:
            explicit Lexer(std::string input) : input_(std::move(input)) {}

            std::vector<Token> tokenize() {
                std::vector<Token> tokens;
                
                while (pos_ < input_.length()) {
                    char c = peek();

                    if (std::isspace(c)) {
                        consume(); // ignores the space
                    } else if (c == '(') {
                        consume();
                        tokens.push_back({TokenType::LPAREN, "("});
                    } else if (c == ')') {
                        consume();
                        tokens.push_back({TokenType::RPAREN, ")"});
                    } else if (c == '!') {
                        consume();
                        tokens.push_back({TokenType::NOT, "!"});
                    } else if (c == '&') {
                        consume();
                        if (peek() == '&') consume(); // support both '&' and '&&'
                        tokens.push_back({TokenType::AND, "&&"});
                    } else if (c == '|') {
                        consume();
                        if (peek() == '|') consume();
                        tokens.push_back({TokenType::OR, "||"});
                    } else if (c == '-') {
                        consume();
                        if (peek() == '>') {
                            consume();
                            tokens.push_back({TokenType::RPAREN, ")"});
                        }
                        else {
                            throw std::runtime_error("Lexer Error: Expected '>' after '-'");
                        }
                    } else if (std::isalpha(c) || c == '_') { // read entire word
                        std::string word;
                        while(std::isalnum(peek()) || peek() == '_') {
                            word += consume();
                        }
                        
                        if (word == "true") tokens.push_back({TokenType::TRUE_VALUE, word});
                        else if (word == "false") tokens.push_back({TokenType::FALSE_VALUE, word});
                        else if (word == "X") tokens.push_back({TokenType::NEXT, word});
                        else if (word == "G") tokens.push_back({TokenType::GLOBALLY, word});
                        else if (word == "F") tokens.push_back({TokenType::EVENTUALLY, word});
                        else if (word == "U") tokens.push_back({TokenType::UNTIL, word});
                        else if (word == "R") tokens.push_back({TokenType::RELEASE, word});
                        else tokens.push_back({TokenType::ATOM, word}); // if not a word with specific meaning
                    } else {
                        throw std::runtime_error(std::string("Lexer Error: Unknown character '") + c + "'");
                    }
                }
                tokens.push_back({TokenType::END_OF_STRING, ""});
                return tokens;
            }
    };

    class Parser {
        private:
            std::vector<Token> tokens_;
            size_t pos_ = 0;

            const Token& peek() const { return tokens_[pos_]; }
            const Token& consume() { return tokens_[pos_++]; }

            // grammer rules (lowest to highest precedence)
            // technique: Recursive Descent Parsing (RDP)
            
            // IMPLIES '->' (lowest precedence)
            LTLPtr parseImplies() {
                LTLPtr left = parseOr();
                while (peek().type == TokenType::IMPLIES) {
                    consume();
                    LTLPtr right = parseOr();
                    left = makeImplies(std::move(left), std::move(right));
                }
                return left;
            }

            // OR '||'
            LTLPtr parseOr() {
                LTLPtr left = parseAnd();
                while(peek().type == TokenType::OR) {
                    consume();
                    LTLPtr right = parseAnd();
                    left = makeOr(std::move(left), std::move(right));
                }
                return left;
            }

            // AND '&&'
            LTLPtr parseAnd() {
                LTLPtr left = parseUntilRelease();
                while(peek().type == TokenType::AND) {
                    consume();
                    LTLPtr right = parseUntilRelease();
                    left = makeOr(std::move(left), std::move(right));
                }
                return left;
            }

            // binary temporals: UNTIL 'U' | RELEASE 'R'
            LTLPtr parseUntilRelease() {
                LTLPtr left = parseUnary();
                while (peek().type == TokenType::UNTIL || peek().type == TokenType::RELEASE) {
                    TokenType op = consume().type;
                    LTLPtr right = parseUnary();
                    if (op == TokenType::UNTIL) left = makeUntil(std::move(left), std::move(right));
                    else left = makeRelease(std::move(left), std::move(right));
                }
                return left;
            }

            // unary operators: NOT '!' | NEXT 'X' | EVENTUALLY 'F' | GLOBALLY 'G'
            LTLPtr parseUnary() {
                TokenType op = peek().type;
                switch (op) {
                    case TokenType::NOT:
                        consume();
                        return makeNot(parseUnary());
                    case TokenType::NEXT:
                        consume();
                        return makeNext(parseUnary());
                    case TokenType::EVENTUALLY:
                        consume();
                        return makeEventually(parseUnary());
                    case TokenType::GLOBALLY:
                        consume();
                        return makeGlobally(parseUnary());
                }
                return parsePrimary();
            }

            // primary: ATOM, TRUE_VALUE, FALSE_VALUE, LPAREN, RPAREN
            LTLPtr parsePrimary() {
                Token t = consume();
                if (t.type == TokenType::ATOM) return makeAtom(t.value);
                else if (t.type == TokenType::TRUE_VALUE) return makeBool(true);
                else if (t.type == TokenType::FALSE_VALUE) return makeBool(false);
                else if (t.type == TokenType::LPAREN) {
                    LTLPtr inside = parseImplies(); // parse the inside of the parentheses
                    if (consume().type != TokenType::RPAREN) {
                        throw std::runtime_error("Parser Error: Missing closing parenthesis ')'");
                    }
                    return inside;
                } 
                throw std::runtime_error("Parser Error: Unexpected token '" + t.value + "'");
            }

        public:
            explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

            static LTLPtr parse(const std::string& str) {
                Lexer lexer(str);
                Parser parser(lexer.tokenize());
                LTLPtr ast = parser.parseImplies();

                if (parser.peek().type != TokenType::END_OF_STRING) {
                    throw std::runtime_error("Parser Error: Unexpected input at the end of the string");
                }
                return ast;
            }
    };
}