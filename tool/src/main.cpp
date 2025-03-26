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


int main(int argc, char** argv, char **envp) {
    try{
        std::string home=(std::getenv("HOME")!=nullptr) ? std::getenv("HOME") : "c:/Users/Roger";
        std::string cantlrHome=(std::getenv("CANTLR_HOME")!=nullptr) ?std::getenv("CANTLR_HOME") : ".";
        CLI::App app{"C++ parser generator"};
        std::vector<std::string> positional;
        std::string namespaceName="sylvanmats::antlr4::parse";
        std::filesystem::path directory="./";
        std::filesystem::path outDirectory="./";
        //app.set_help_flag("-h,--help", "Print this help message and exit");
        //app.set_help_all_flag("--help-all", "Expand all help");
        app.get_formatter()->column_width(25);
        app.add_option("-f,--file,custom", positional, "input grammars");
        app.add_option("--ns,--namespace", namespaceName, "namespace name");
        app.add_option("-o,--out", outDirectory, "output directory");
        CLI11_PARSE(app, argc, argv);

        if(positional.size()>1){
            if(!std::filesystem::exists(outDirectory))std::filesystem::create_directories(outDirectory);
            std::filesystem::path filePath=positional.front();
            if(std::filesystem::exists(filePath)){
            if(filePath.has_parent_path())
                directory=filePath.parent_path();
            sylvanmats::antlr4::parse::G4Reader g4Reader;
            g4Reader(filePath, [&namespaceName, &directory, &outDirectory](std::u16string& utf16, std::unordered_map<std::u16string, std::u16string>& options, sylvanmats::antlr4::parse::G& dagGraph){
                sylvanmats::publishing::CodeGenerator<std::string> codeGenerator(namespaceName);
                sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
                morpher(utf16, dagGraph);
                std::string&& content=codeGenerator();
                std::ofstream os(outDirectory.string()+"/"+codeGenerator.getParserClass()+".h");
                std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
                os.close();
            });
            }
            else std::cout<<filePath<<" does not exist"<<std::endl;
        }
        if(!positional.empty()){
            std::filesystem::path stFilePath=positional.back();
            if(std::filesystem::exists(stFilePath)){
            if(stFilePath.has_parent_path())
                directory=stFilePath.parent_path();
            sylvanmats::antlr4::parse::G4Reader g4PReader;
            g4PReader(stFilePath, [&namespaceName, &directory, &outDirectory](std::u16string& utf16, std::unordered_map<std::u16string, std::u16string>& options, sylvanmats::antlr4::parse::G& dagGraph){
                sylvanmats::publishing::CodeGenerator<std::string> codeGenerator(namespaceName);
                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
                if(options.count(u"tokenVocab")){
                    //std::cout<<"toml "<<utf16conv.to_bytes(options[u"tokenVocab"])<<" "<<codeGenerator.getParserClass()<<std::endl;
                    codeGenerator.setTokenVocab(utf16conv.to_bytes(options[u"tokenVocab"]));
                }
                sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
                morpher(utf16, dagGraph);
                std::string&& content=codeGenerator();
                std::ofstream os(outDirectory.string()+"/"+codeGenerator.getParserClass()+".h");
                std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(os));
                os.close();
            });
            }
            else std::cout<<stFilePath<<" does not exist"<<std::endl;
        }
    }
    catch(std::filesystem::filesystem_error &e) {
        std::cout<<"what? "<<e.what() << std::endl;
        return  EXIT_FAILURE;
    }
    catch(const std::exception& e) {
        print_exception(e);
        return  EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

