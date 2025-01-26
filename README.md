# antlr4-thresher lexer/parser in c++ 

Attempting g4 reader and parser generator using modern c++ advantages.

## To build and test

Note: currently working on a new package manager to use on this package.  Other managers should still 
work(as far as they do) as https://github.com/DataDriven-CAM/cnpm.git is based on the package.json npm format.

### cnpm building

```
cnpm  install

cnpm lib

#building and running unit tests
cnpm test

```
This also compiles a runner tool called cantlrtool(name may change).

Example use
```
cantlrtool ../grammars-v4/xpath/xpath31/XPath31Lexer.g4 ../grammars-v4/xpath/xpath31/XPath31Parser.g4 -o ./include/io/xpath

cantlrtool --help

```
default namspace "sylvanmats::antlr4::parse"
