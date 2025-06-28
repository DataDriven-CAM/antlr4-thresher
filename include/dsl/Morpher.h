#pragma once

#include <codecvt>
#include <cstring>
#include <cwchar>
#include <string>
#include <functional>
#include <filesystem>
#include <tuple>

#include "parse/dag_graph.h"
#include "publishing/CodeGenerator.h"
#include "dsl/Exception.h"

namespace sylvanmats::dsl{
    class Morpher{
    protected:
        sylvanmats::publishing::CodeGenerator<std::string>& codeGenerator;
        std::filesystem::path directory="./";
        std::u16string lexerInstance{};
        std::u16string tokenPrefix=u"LEXER_";
        std::u16string tempInc=u"temp++";
    public:
        Morpher() = delete;
        Morpher(std::filesystem::path& directory, sylvanmats::publishing::CodeGenerator<std::string>& codeGenerator);
        Morpher(const Morpher& orig) = delete;
        virtual ~Morpher() = default;
        
        void operator()(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph);

    private:
        bool orOn=false;
        void operator()(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph, graph::container::csr_row<unsigned int>& v);
        bool recurseLexerRule(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph, graph::container::csr_row<unsigned int>& source, std::vector<std::u16string>& expr);
    };
}
