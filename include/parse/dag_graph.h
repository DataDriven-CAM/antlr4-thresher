#pragma once


#include "graph/container/compressed_graph.hpp"

namespace sylvanmats::antlr4::parse {
    enum TOKEN{
        ROOT,
        DOC_COMMENT,
        BLOCK_COMMENT,
        LINE_COMMENT,
        INT,
        STRING_LITERAL,
        OPTIONS,
        TOKENS,
        CHANNELS,
        SKIP,
        IMPORT,
        LEXER,
        PARSER,
        GRAMMAR,
        MODE,
        TYPE,
        COLON,
        SEMI,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACK,
        RBRACK,
        ESCSEQ,
        ESC,
        ESCANY,
        RARROW,
        NOT,
        ID,
        ARGUMENT,
        TOK_DOC_COMMENT,
        TOK_LBRACE,
        TOK_RBRACE,
        TOK_ID,
        TOK_DOT,
        TOK_COMMA,
        CHN_DOC_COMMENT,
        CHN_LBRACE,
        CHN_RBRACE,
        CHN_ID,
        CHN_DOT,
        CHN_COMMA,
        OPT_DOC_COMMENT,
        OPT_LBRACE,
        OPT_RBRACE,
        OPT_ID,
        OPT_ASSIGN,
        OPT_SEMI,
        MINUS,
        STAR,
        PLUS,
        DOT,
        QUESTION,
        PIPE,
        RANGE
    };

    enum PARSE_MODE{
        DEFAULT,
        Options,
        Tokens,
        Channels
    };

    struct ast_node{
      const char16_t* start;
      const char16_t* stop;
      TOKEN token;
      PARSE_MODE mode=PARSE_MODE::DEFAULT;
      bool frag=false;
    };

    using G = graph::container::compressed_graph<int, ast_node>;
}
