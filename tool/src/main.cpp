#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include <stdlib.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <filesystem>

#include "cantlrtool.h"

#define FMT_HEADER_ONLYS
#include "graph/container/compressed_graph.hpp"

#include "parse/G4Reader.h"
#include "dsl/Morpher.h"
#include "io/tikz/G4GraphPublisher.h"

int main(int argc, char** argv, char **envp) {
    try{
        std::string home=(std::getenv("HOME")!=nullptr) ? std::getenv("HOME") : "c:/Users/Roger";
        std::string cantlrHome=(std::getenv("CANTLR_HOME")!=nullptr) ?std::getenv("CANTLR_HOME") : ".";
        CLI::App app{"C++ parser generator"};
        std::vector<std::string> positional;
        std::string namespaceName="sylvanmats::antlr4::";
        std::filesystem::path directory="./";
        std::filesystem::path outDirectory="./";
        bool graphit{false};
        //app.set_help_flag("-h,--help", "Print this help message and exit");
        //app.set_help_all_flag("--help-all", "Expand all help");
        app.get_formatter()->column_width(25);
        app.add_option("-f,--file,custom", positional, "input grammars");
        app.add_option("--ns,--namespace", namespaceName, "namespace name");
        CLI::Option* out=app.add_option("-o,--out", outDirectory, "output directory");
        app.add_flag("-g,--graph", graphit, "Graph the rules'");
        CLI11_PARSE(app, argc, argv);

        namespaceName+=(out) ? outDirectory.filename().string(): "parse";
        sylvanmats::publishing::CodeGenerator<std::string> lexerGenerator(namespaceName);
        if(positional.size()>1){
            if(!std::filesystem::exists(outDirectory))std::filesystem::create_directories(outDirectory);
            std::filesystem::path filePath=positional.front();
            if(std::filesystem::exists(filePath)){
                std::cout<<"parsing "<<filePath<<" "<<std::endl;
            if(filePath.has_parent_path())
                directory=filePath.parent_path();
            sylvanmats::antlr4::parse::G4Reader g4Reader;
            g4Reader(filePath, [&lexerGenerator, &directory, &outDirectory, &graphit](std::u16string& utf16, std::unordered_map<std::u16string, std::u16string>& options, sylvanmats::antlr4::parse::G& dagGraph){
                sylvanmats::dsl::Morpher morpher(directory, lexerGenerator);
                morpher(utf16, dagGraph);
                lexerGenerator.appendToken("LEXER_ENDOFFILE");
             });
            }
            else std::cout<<filePath<<" does not exist"<<std::endl;
        }
        if(!positional.empty()){
            std::filesystem::path stFilePath=positional.back();
            if(std::filesystem::exists(stFilePath)){
                std::cout<<"parsing "<<stFilePath<<" "<<std::endl;
            if(stFilePath.has_parent_path())
                directory=stFilePath.parent_path();
            sylvanmats::antlr4::parse::G4Reader g4PReader;
            g4PReader(stFilePath, [&lexerGenerator, &namespaceName, &directory, &outDirectory, &graphit](std::u16string& utf16, std::unordered_map<std::u16string, std::u16string>& options, sylvanmats::antlr4::parse::G& dagGraph){
                sylvanmats::publishing::CodeGenerator<std::string> codeGenerator(namespaceName);
                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
                if(options.count(u"tokenVocab")){
                    //std::cout<<"toml "<<utf16conv.to_bytes(options[u"tokenVocab"])<<" "<<codeGenerator.getParserClass()<<std::endl;
                    codeGenerator.setTokenVocab(utf16conv.to_bytes(options[u"tokenVocab"]));
                }
                sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
                morpher(utf16, dagGraph);
                for(std::vector<std::tuple<std::u16string, std::u16string, std::u16string>>::iterator it=morpher.getImplicits().begin();it!=morpher.getImplicits().end();it++){
                    lexerGenerator.appendLexerRuleClass(utf16conv.to_bytes(std::get<0>(*it)), "", utf16conv.to_bytes(std::get<1>(*it)), false, false, utf16conv.to_bytes(std::get<2>(*it)));
                    lexerGenerator.appendToken(utf16conv.to_bytes(std::get<1>(*it)));
                }
                std::string&& content=codeGenerator();
                std::ofstream os(outDirectory.string()+"/"+codeGenerator.getParserClass()+".h");
                std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
                os.close();
                if(graphit){
                    //std::cout<<"g4 graph "<<std::endl;
                    sylanmats::io::tikz::G4GraphPublisher<sylvanmats::antlr4::parse::G> g4GraphPublisher;
                    std::string tikzContent=g4GraphPublisher(utf16, dagGraph);
                    std::ofstream osTokz("./"+codeGenerator.getParserClass()+".tex");
                    std::copy(tikzContent.begin(), tikzContent.end(), std::ostreambuf_iterator<char>(osTokz));
                    osTokz.close();
                }
            });
            }
            else std::cout<<stFilePath<<" does not exist"<<std::endl;
        }
        if(positional.size()>1){
            std::string&& content=lexerGenerator();
            std::ofstream os(outDirectory.string()+"/"+lexerGenerator.getParserClass()+".h");
            std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
            os.close();
            // if(graphit){
            //     sylanmats::io::tikz::G4GraphPublisher<sylvanmats::antlr4::parse::G> g4GraphPublisher;
            //     std::string tikzContent=g4GraphPublisher(utf16, dagGraph);
            //     std::ofstream osTokz("./"+lexerGenerator.getParserClass()+".tex");
            //     std::copy(tikzContent.begin(), tikzContent.end(), std::ostreambuf_iterator<char>(osTokz));
            //     osTokz.close();
            // }
        }

    }
    catch(std::filesystem::filesystem_error &e) {
        std::cout<<"what? "<<e.what() << std::endl;
        return  EXIT_FAILURE;
    }
    catch(const std::exception& e) {
        std::cout<<"Ex "<<e.what()<<std::endl;;
        print_exception(e);
        return  EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

