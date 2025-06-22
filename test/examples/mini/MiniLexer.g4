lexer grammar MiniLexer;

IntegerLiteral   : FragDigits;
DecimalLiteral   : '.' FragDigits | FragDigits '.' [0-9]*;
DoubleLiteral    : ('.' FragDigits | FragDigits ('.' [0-9]*)?) [eE] [+-]? FragDigits;
StringLiteral    : '"' (~["] | FragEscapeQuot)* '"' | '\'' (~['] | FragEscapeApos)* '\'';

fragment FragEscapeQuot : '""';

fragment FragEscapeApos : '\'\'';

fragment FragDigits      : [0-9]+;
