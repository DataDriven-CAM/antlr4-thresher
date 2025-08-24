#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <locale>
#include <codecvt>
#include <format>

#define protected public
#include "publishing/formatting.h"
//#include "io/parsers.h"

TEST_SUITE ("generator"){
    
TEST_CASE("generate grammar-v4 set"){
            std::filesystem::path g4Path="./include/io";
            for(auto& p: std::filesystem::directory_iterator(g4Path)){
                    //  std::cout<<p.path().filename()<<std::endl;
                     std::filesystem::path parserPath=p.path()/"*Parser.h";
                     std::filesystem::path lexerPath=p.path()/"*Lexer.h";
               if(parserPath.filename().string().find("Parser.h")!=std::string::npos && lexerPath.filename().string().find("Lexer.h")!=std::string::npos){
                }
            }
}

}