#pragma once

#include <codecvt>
#include <cstring>
#include <cwchar>
#include <string>
#include <functional>
#include <filesystem>

#include "parse/dag_graph.h"
#include "publishing/CodeGenerator.h"

namespace sylvanmats::dsl{
    class Morpher{
    protected:
        sylvanmats::publishing::CodeGenerator<std::string>& codeGenerator;
        std::filesystem::path directory="./";
    public:
        Morpher() = delete;
        Morpher(std::filesystem::path& directory, sylvanmats::publishing::CodeGenerator<std::string>& codeGenerator);
        Morpher(const Morpher& orig) = delete;
        virtual ~Morpher() = default;
        
        void operator()(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph);

    private:
        size_t depth=0;
        bool orOn=false;
        void operator()(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph, graph::container::csr_row<unsigned int>& v);
        bool recurseLexerRule(sylvanmats::antlr4::parse::G& dagGraph, graph::container::csr_row<unsigned int>& source, std::u16string& expr);
    };
}
