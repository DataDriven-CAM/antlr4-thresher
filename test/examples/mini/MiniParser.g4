parser grammar MiniParser;

options {
    tokenVocab = MiniLexer;
}

path : (literal | additiveexpr)+;

literal : StringLiteral;

additiveexpr
    : multiplicativeexpr ((PLUS | MINUS) multiplicativeexpr)*
    ;

multiplicativeexpr : minteger (STAR minteger)*;

minteger : IntegerLiteral;

