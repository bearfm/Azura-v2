#pragma once

using namespace std;

typedef enum {
    // Single-character Tokens!
    LEFT_PAREN, RIGHT_PAREN,     // ( )
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    COMMA, DOT, MINUS, PLUS,     // , . - +
    SEMICOLON, SLASH, STAR,      // ; / *
    HASHTAG, COLON, MODULO,      // # : %
    DOLLAR,  POWER,              // $ ^

    // One or two character tokens!
    BANG, BANG_EQUAL,        // ! !=
    EQUAL, EQUAL_EQUAL,      // = ==
    GREATER, GREATER_EQUAL,  // > >=
    LESS, LESS_EQUAL,        // < <=
    WALRUS, INHERITANCE,     // := extends
    INCREMENT, DECREMENT,    // ++ --

    // Literals!
    IDENTIFIER, STRING, NUMBER, INAPPEND,

    // Keywords!
    AND, CLASS, ELSE, FALSE, FUNC, FOR, IF, NIL, OR,
    INFO, INPUT, RETURN, SUPER, THIS, TRUE, VAR, WHILE, MODULE,
    INCLUDE, CONTINUE, BREAK, BIT_AND, BIT_OR,

    // Types!
    I8, I16, I32, I64, I128, // ints
    F32, F64, F128,          // floats
    BOOL, STRING_TYPE,       // bool and string
    VOID,                    // void
    
    ERROR_TOKEN,
    // End of file!
    EOF_TOKEN
} TokenKind;

typedef struct {
    TokenKind kind;
    const char* start;
    int length;
    int line;
    int column;
} Token;

void init_tokenizer(const char* source);
const char* get_source_line_start(int line);
Token scan_token();
