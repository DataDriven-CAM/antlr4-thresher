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

elementtest
    : (elementnameorwildcard ( COMMA typename_ QM?)?)? 
    ;

elementnameorwildcard
    : STAR
    ;

typename_
    : eqname
    ;

eqname : KW_ELEMENT ;
