#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <locale>
#include <codecvt>
#include <cstring>
#include <cwchar>
#include <sstream>
#include <regex>
#include <iterator>
#include <numbers>
#include <cassert>
#include <ranges>
#include <chrono>
#include <algorithm>

#define FMT_HEADER_ONLY
#include "graph/container/compressed_graph.hpp"

#define protected public
#include "parse/G4Reader.h"
#include "dsl/Morpher.h"

#include "mio/mmap.hpp"

TEST_SUITE ("grammars"){

TEST_CASE("test u16 fmt formatting"){
    /*std::u16string h=u"Hello";
    std::u16string w=u"world";
    std::u16string s=u"something";
    std::u16string fs=u"{} {}!";
    std::u16string hw=std::format(fs, h, w, s);
    CHECK_EQ(hw, u"Hello world!");*/
    //std::vector<std::string> tokens={"DOUBLE", "MINUS", "PLUS", "DIGIT"};
    std::vector<std::wstring> tokens={L"DOUBLE", L"MINUS", L"PLUS", L"DIGIT"};
    //std::arg("tokens", tokens);
    std::wstring content=fmt::format(L" {}\n", tokens);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;
    //std::cout<<"content "<<content<<std::endl;
    std::cout<<"content "<<cv.to_bytes(content)<<std::endl;
}

TEST_CASE("test graph-v2"){
    std::string content=R"(lexer grammar NLPLexer;
tokens { STRING }
)";
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
    std::u16string utf16 = utf16conv.from_bytes(content);
    std::u16string::const_iterator it = utf16.begin();
    std::vector<sylvanmats::antlr4::parse::ast_node> vertices;
    vertices.push_back(sylvanmats::antlr4::parse::ast_node{.start=&(*it), .stop=&(*it)+4, .token=sylvanmats::antlr4::parse::STRING_LITERAL});
    vertices.push_back(sylvanmats::antlr4::parse::ast_node{.start=&(*it)+6, .stop=&(*it)+12, .token=sylvanmats::antlr4::parse::STRING_LITERAL});
    vertices.push_back(sylvanmats::antlr4::parse::ast_node{.start=&(*it)+14, .stop=&(*it)+21, .token=sylvanmats::antlr4::parse::STRING_LITERAL});
    graph::container::compressed_graph<int, sylvanmats::antlr4::parse::ast_node> astGraph{{0,1,1}, {1,2,1}};
    //astGraph.reserve(3);
    astGraph.load_vertices(vertices, [&vertices](sylvanmats::antlr4::parse::ast_node& nm) {
        auto uid = static_cast<graph::vertex_id_t<graph::container::compressed_graph<int, sylvanmats::antlr4::parse::ast_node>>>(&nm - vertices.data());
        std::cout<<"uid "<<uid<<std::endl;
        return graph::copyable_vertex_t< graph::vertex_id_t<graph::container::compressed_graph<int, sylvanmats::antlr4::parse::ast_node>>, sylvanmats::antlr4::parse::ast_node>{uid, nm};
    });
    std::cout<<"size: "<<graph::num_vertices(astGraph)<<" "<<graph::vertices(astGraph).size()<<std::endl;
    CHECK_EQ(graph::num_vertices(astGraph), 3);
}

TEST_CASE("test small lexer"){
    auto start = std::chrono::high_resolution_clock::now();
    std::string content=R"(// just a start

lexer grammar SmallLexer;

UnsignedInteger : ( Digit )+;
PlusPlus : Plus Plus;
MinusMinus : Minus Minus;
Double : '=';
Minus : '-';
Plus : '+';
Digit : '0' .. '9';
)";
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
    std::u16string utf16 = utf16conv.from_bytes(content);
    sylvanmats::antlr4::parse::G4Reader g4Reader;
    g4Reader(utf16, [](std::u16string& utf16, sylvanmats::antlr4::parse::G& dagGraph){
        std::string ns="sylvanmats::antlr4::parse";
        sylvanmats::publishing::CodeGenerator<std::string> codeGenerator(ns);
        std::filesystem::path directory="./";
        sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
        morpher(utf16, dagGraph);
        std::string&& content=codeGenerator();
        std::ofstream os("../tmp/"+codeGenerator.getParserClass()+".h");
        std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
        os.close();
    });
    g4Reader.display();
    CHECK_EQ(graph::num_vertices(g4Reader.dagGraph), 39);
    CHECK_EQ(graph::num_edges(g4Reader.dagGraph), 38);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1.0e-9 << "s\n";
}

TEST_CASE("test antlr4 lex basic parse"){
    std::filesystem::path filePath="../../cpp_modules/grammars-v4/antlr/antlr4/LexBasic.g4";
    std::filesystem::path directory="./";
    if(filePath.has_parent_path())
        directory=filePath.parent_path();
    const int fdAntlr = open(filePath.c_str(), O_RDONLY);
    mio::mmap_source mmapAntlr(fdAntlr, 0, mio::map_entire_file);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
    std::u16string utf16 = utf16conv.from_bytes(mmapAntlr.begin());
    sylvanmats::antlr4::parse::G4Reader g4Reader;
    g4Reader(utf16, [&directory](std::u16string& utf16, sylvanmats::antlr4::parse::G& dagGraph){
        std::string ns="sylvanmats::antlr4::parse";
        std::filesystem::path directory="./";
        sylvanmats::publishing::CodeGenerator<std::string> codeGenerator(ns);
        sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
        morpher(utf16, dagGraph);
        std::string&& content=codeGenerator();
        std::ofstream os("../tmp/"+codeGenerator.getParserClass()+".h");
        std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
        os.close();
    });
    g4Reader.display();
    CHECK_EQ(graph::num_vertices(g4Reader.dagGraph), 39);
    CHECK_EQ(graph::num_edges(g4Reader.dagGraph), 38);
}

TEST_CASE("test antlr4 self parse"){
    std::filesystem::path filePath="../cpp_modules/grammars-v4/antlr/antlr4/ANTLRv4Lexer.g4";
    std::filesystem::path directory="./";
    if(filePath.has_parent_path())
        directory=filePath.parent_path();
    sylvanmats::antlr4::parse::G4Reader g4Reader;
    g4Reader(filePath, [&directory](std::u16string& utf16, sylvanmats::antlr4::parse::G& dagGraph){
        std::string ns="sylvanmats::antlr4::parse";
        sylvanmats::publishing::CodeGenerator<std::string> codeGenerator(ns);
        sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
        morpher(utf16, dagGraph);
        std::string&& content=codeGenerator();
        std::cout<<"codeGenerator.getParserClass() "<<codeGenerator.getParserClass()<<std::endl;
        std::ofstream os("../tmp/"+codeGenerator.getParserClass()+".h");
        std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
        os.close();
    });
    g4Reader.display();
    std::filesystem::path stFilePath="../cpp_modules/grammars-v4/antlr/antlr4/ANTLRv4Parser.g4";
    sylvanmats::antlr4::parse::G4Reader g4PReader;
    g4PReader(stFilePath, [&directory](std::u16string& utf16, sylvanmats::antlr4::parse::G& dagGraph){
        std::string ns="sylvanmats::antlr4::parse";
        sylvanmats::publishing::CodeGenerator<std::string> codeGenerator(ns);
        sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
        morpher(utf16, dagGraph);
        std::string&& content=codeGenerator();
        std::cout<<"p codeGenerator.getParserClass() "<<codeGenerator.getParserClass()<<std::endl;
        std::ofstream os("../tmp/"+codeGenerator.getParserClass()+".h");
        std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
        os.close();
    });
}

TEST_CASE("test xpath 3.1"){
    std::filesystem::path filePath="../cpp_modules/grammars-v4/xpath/xpath31/XPath31Lexer.g4";
    std::filesystem::path directory="./";
    if(filePath.has_parent_path())
        directory=filePath.parent_path();
    sylvanmats::antlr4::parse::G4Reader g4Reader;
    g4Reader(filePath, [&directory](std::u16string& utf16, sylvanmats::antlr4::parse::G& dagGraph){
        std::string ns="sylvanmats::antlr4::parse";
        sylvanmats::publishing::CodeGenerator<std::string> codeGenerator(ns);
        sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
        morpher(utf16, dagGraph);
        std::string&& content=codeGenerator();
        std::cout<<"p codeGenerator.getParserClass() "<<codeGenerator.getParserClass()<<std::endl;
        std::ofstream os("../tmp/"+codeGenerator.getParserClass()+".h");
        std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
        os.close();
   });
    std::filesystem::path stFilePath="../cpp_modules/grammars-v4/xpath/xpath31/XPath31Parser.g4";
    g4Reader(stFilePath, [&directory](std::u16string& utf16, sylvanmats::antlr4::parse::G& dagGraph){
        std::string ns="sylvanmats::antlr4::parse";
        sylvanmats::publishing::CodeGenerator<std::string> codeGenerator(ns);
        sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
        morpher(utf16, dagGraph);
        std::string&& content=codeGenerator();
        std::cout<<"p codeGenerator.getParserClass() "<<codeGenerator.getParserClass()<<std::endl;
        std::ofstream os("../tmp/"+codeGenerator.getParserClass()+".h");
        std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
        os.close();
    });
}

}
