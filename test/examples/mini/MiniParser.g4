parser grammar MiniParser;

options {
    tokenVocab = MiniLexer;
}

path : literal;

literal : StringLiteral;

additiveexpr
    : multiplicativeexpr ((PLUS | MINUS) multiplicativeexpr)*
    ;

multiplicativeexpr : STAR;