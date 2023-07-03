#ifndef PARSER_HELPER_H
#define PARSER_HELPER_H

#include "../lexer/lexer.h"
#include "../common.h"

class Parser {
private:
    typedef enum {
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        RESET
    } Color;

    const char* colorize(Color color) {
        switch (color) {
            case RED: return "\033[0;31m";
            case GREEN: return "\033[0;32m";
            case YELLOW: return "\033[0;33m";
            case BLUE: return "\033[0;34m";
            case MAGENTA: return "\033[0;35m";
            case CYAN: return "\033[0;36m";
            case WHITE: return "\033[0;37m";
            case RESET: return "\033[0m";
            default: return "\033[0m";
        }
    }

public:
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;

    Parser() : had_error(false), panic_mode(false) {}

    void error_parser(Token* token, const char* message) {
        if(panic_mode) return;
        panic_mode = true;

        std::printf("\n[%sline: %s%d%s] [%spos: %s%d%s] Error: ", 
        colorize(YELLOW), colorize(CYAN), token->line, colorize(RESET), 
            colorize(YELLOW), colorize(CYAN), token->column - 1, colorize(RESET));

        if (token->kind == EOF_TOKEN) std::printf("at end \n");
        else if (token->kind == ERROR_TOKEN) {}
        else std::printf("at %s'%.*s'%s\n", colorize(RED), token->length, token->start, colorize(RESET));

        // iterator over the tokens in the current line
        const char* line_start = token->start;
        const char* line_end = line_start;
        while (*line_end != '\n' && *line_end != '\0') line_end++;

        // Print the line
        std::cout << colorize(MAGENTA) << std::string(line_start, line_end - line_start) << "\n";

        // Print to the error
        int num_spaces = token->column - 1;
        for (int i = 0; i < num_spaces; i++) std::cout << " ";
        std::cout << colorize(RED) << "^" << colorize(RESET);

        // Print the error message
        std::cout << message << "\n";
        had_error = true;
    }

    void error_at_current(const char* message) { error_parser(&current, message); }
    void error(const char* message) { error_parser(&previous, message); }

    bool check(TokenKind kind) { return current.kind == kind; }

    void advance() {
        previous = current;
        for(;;) {
            current = scan_token();
            if(current.kind != ERROR_TOKEN) break;
            error_at_current(current.start);
        }
    }

    void consume(TokenKind kind, const char* message) {
        if (current.kind == kind) {
            advance();
            return;
        }

        error_at_current(message);
    }

    bool match(TokenKind kind) {
        if (!check(kind)) return false;
        advance();
        return true;
    }
};

typedef void (*parse_fn)(bool can_assign);

enum Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT,  // :=
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
};

struct ParseRule {
    parse_fn prefix;
    parse_fn infix;
    Precedence precedence;
};

struct Local {
    Token name;
    int depth;
};

struct Compiler {
    Local locals[UINT8_COUNT];
    int local_count;
    int scope_depth;
};

struct Parser parser;
struct Compiler* current = nullptr;
struct Chunk* compiling_chunk;


void grouping(bool can_assign);
void _number(bool can_assign);
void _string(bool can_assign);
void _variable(bool can_assign);
void and_(bool can_assign);
void or_(bool can_assign);
void unary(bool can_assign);
void binary(bool can_assign);
void literal(bool can_assign);

std::unordered_map<TokenKind, ParseRule> rules = {
    {LEFT_PAREN,    {grouping,  nullptr,   PREC_NONE}},
    {RIGHT_PAREN,   {nullptr,   nullptr,   PREC_NONE}},
    {LEFT_BRACE,    {nullptr,   nullptr,   PREC_NONE}},
    {RIGHT_BRACE,   {nullptr,   nullptr,   PREC_NONE}},
    {COMMA,         {nullptr,   nullptr,   PREC_NONE}},
    {DOT,           {nullptr,   nullptr,   PREC_NONE}},
    {MINUS,         {unary,     binary,     PREC_TERM}},
    {PLUS,          {nullptr,   binary,     PREC_TERM}},
    {SEMICOLON,     {nullptr,   nullptr,   PREC_NONE}},
    {SLASH,         {nullptr,   binary,     PREC_FACTOR}},
    {STAR,          {nullptr,   binary,     PREC_FACTOR}},
    {MODULO,        {nullptr,   binary,     PREC_FACTOR}},
    {POWER,         {nullptr,   binary,     PREC_FACTOR}},
    {BANG,          {unary,     nullptr,   PREC_NONE}},
    {BANG_EQUAL,    {nullptr,   binary,     PREC_EQUALITY}},
    {EQUAL,         {nullptr,   binary,     PREC_COMPARISON}},
    {EQUAL_EQUAL,   {nullptr,   binary,     PREC_COMPARISON}},
    {GREATER,       {nullptr,   binary,     PREC_COMPARISON}},
    {GREATER_EQUAL, {nullptr,   binary,     PREC_COMPARISON}},
    {LESS,          {nullptr,   binary,     PREC_COMPARISON}},
    {LESS_EQUAL,    {nullptr,   binary,     PREC_COMPARISON}},
    {IDENTIFIER,    {_variable,  nullptr,   PREC_NONE}},
    {STRING,        {_string,    nullptr,   PREC_NONE}},
    {NUMBER,        {_number,    nullptr,   PREC_NONE}},
    {AND,           {nullptr,       and_,   PREC_AND}},
    {CLASS,         {nullptr,   nullptr,   PREC_NONE}},
    {ELSE,          {nullptr,   nullptr,   PREC_NONE}},
    {FALSE,         {literal,   nullptr,   PREC_NONE}},
    {FUNC,          {nullptr,   nullptr,   PREC_NONE}},
    {FOR,           {nullptr,   nullptr,   PREC_NONE}},
    {IF,            {nullptr,   nullptr,   PREC_NONE}},
    {NIL,           {literal,   nullptr,   PREC_NONE}},
    {OR,            {nullptr,        or_,   PREC_OR}},
    {INFO,          {nullptr,   nullptr,   PREC_NONE}},
    {RETURN,        {nullptr,   nullptr,   PREC_NONE}},
    {SUPER,         {nullptr,   nullptr,   PREC_NONE}},
    {THIS,          {nullptr,   nullptr,   PREC_NONE}},
    {TRUE,          {literal,   nullptr,   PREC_NONE}},
    {HAVE,          {nullptr,   nullptr,   PREC_NONE}},
    {WHILE,         {nullptr,   nullptr,   PREC_NONE}},
    {MODULE,        {nullptr,   nullptr,   PREC_NONE}},
    {USING,         {nullptr,   nullptr,   PREC_NONE}},
    {ERROR_TOKEN,   {nullptr,   nullptr,   PREC_NONE}},
    {EOF_TOKEN,     {nullptr,   nullptr,   PREC_NONE}},
};

const std::unordered_map<TokenKind, bool> return_context = {
    {CLASS, true},
    {FUNC, true},
    {HAVE, true},
    {FOR, true},
    {IF, true},
    {WHILE, true},
    {INFO, true},
    {RETURN, true}
};

#endif // PARSER_HELPER_H