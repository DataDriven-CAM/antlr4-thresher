parser grammar MiniParser;

options {
    tokenVocab = MiniLexer;
}

path : literal;

literal : StringLiteral;