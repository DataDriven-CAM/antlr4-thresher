parser grammar TinyParser;

options {
    tokenVocab = TinyLexer;
}

eq : term +;

term : mconstant Times mconstant;
mconstant : UnsignedInteger;
