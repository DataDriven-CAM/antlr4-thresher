lexer grammar MiniLexer;

MINUS      : '-';
PLUS       : '+';
STAR       : '*';

IntegerLiteral   : FragDigits;
DecimalLiteral   : '.' FragDigits | FragDigits '.' [0-9]*;
DoubleLiteral    : ('.' FragDigits | FragDigits ('.' [0-9]*)?) [eE] [+-]? FragDigits;
StringLiteral    : '"' (~["] | FragEscapeQuot)* '"' | '\'' (~['] | FragEscapeApos)* '\'';
BracedURILiteral : 'Q' '{' [^{}]* '}';

fragment FragEscapeQuot : '""';

fragment FragEscapeApos : '\'\'';

fragment FragDigits      : [0-9]+;

fragment FragChar:
    '\u0009'
    | '\u000a'
    | '\u000d'
    | '\u0020' ..'\ud7ff'
    | '\ue000' ..'\ufffd'
    | '\u{10000}' ..'\u{10ffff}'
;
