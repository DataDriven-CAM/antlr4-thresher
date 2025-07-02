lexer grammar TinyLexer;

UnsignedInteger : ( Digit )+;
Times : '*';
Minus : '-';
Plus : '+';
fragment Digit : '0' .. '9';

STRING        : '"' ~[<"]* '"' | '\'' ~[<']* '\'';
