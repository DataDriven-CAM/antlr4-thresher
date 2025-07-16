#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <tuple>
#include <filesystem>
#include <cstddef>
#include <cstdarg>
#include <cwchar>
#include <typeinfo>
#include <stacktrace>

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/xchar.h"
#include "fmt/ranges.h"

template <>
struct fmt::formatter<std::vector<std::string>>{
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
        return ctx.begin();
    }
    
     auto format(const std::vector<std::string>& v, format_context& ctx) const -> format_context::iterator{
        std::string indentation(8, ' ');
         //std::cout<<"tokens "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            //auto iArg=fmt::arg("indent", indentation);
            std::string iA=v[i];
            if(i<v.size()-1)
                fmt::vformat_to(ctx.out(), "{}{},\n", fmt::make_format_args(indentation, iA));
            else
                fmt::vformat_to(ctx.out(), "{}{}\n", fmt::make_format_args(indentation, iA));
        }
        constexpr typename std::string::value_type* fmt={""};
        return fmt::format_to(ctx.out(), fmt);
    }
    // ...
    bool        curly{false};
    std::string value_format;
};

template <>
struct fmt::formatter<std::vector<std::tuple<std::string, std::string>>>{
    auto parse(format_parse_context& ctx) -> format_parse_context::iterator{
        return ctx.begin();
    }
    
     auto format(const std::vector<std::tuple<std::string, std::string>>& v, format_context& ctx) const -> format_context::iterator{
        std::string indentation(8, ' ');
         //std::cout<<"tokens "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            std::string iA=std::get<0>(v[i]);
            std::string eA=std::get<1>(v[i]);
            auto iArg=fmt::arg("indent", indentation);
            auto fArg=fmt::arg("function", iA);
            auto eArg=fmt::arg("expression", eA);
            fmt::vformat_to(ctx.out(), "{indent}std::function<bool(std::u16string::const_iterator&)> {function} = [&](std::u16string::const_iterator& it) {{std::u16string::const_iterator temp=it; bool ret={expression}; if(ret)it=temp;return ret;}};\n\n", fmt::make_format_args(iArg, fArg, eArg));
        }
        constexpr typename std::string::value_type* fmt={"\n"};
        return fmt::format_to(ctx.out(), fmt);
    }
    // ...
    bool        curly{false};
    std::string value_format;
};

template <>
struct fmt::formatter<std::vector<std::tuple<std::string, std::string, std::string>>>{
    auto parse(format_parse_context& ctx) -> format_parse_context::iterator{
        return ctx.begin();
    }
    
     auto format(const std::vector<std::tuple<std::string, std::string, std::string>>& v, format_context& ctx) const -> format_context::iterator{
        std::string indentation(8, ' ');
         //std::cout<<"tokens "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            std::string iA=std::get<0>(v[i]);
            std::string fA=std::get<1>(v[i]);
            std::string eA=std::get<2>(v[i]);
            std::string ps=iA;
            std::transform(ps.cbegin(), ps.cend(), ps.begin(), [](const char& c){return std::toupper(c);});
            auto iArg=fmt::arg("indent", indentation);
            auto fArg=fmt::arg("function", iA);
            auto eArg=fmt::arg("expression", eA);
            auto psArg=fmt::arg("parser_token", ps);
            fmt::vformat_to(ctx.out(), "{indent}std::function<bool(LG&, graph::container::csr_row<unsigned int>&)> {function} = [&](LG& ldagGraph, graph::container::csr_row<unsigned int>& source) {{\n{indent}    graph::container::csr_row<unsigned int> s=source;/*if(s.index+1<size(graph::vertices(ldagGraph)))s=ldagGraph[s.index+1]*/;std::cout<<\"{function} \"<<source.index<<\" \"<<(graph::vertex_value(ldagGraph, ldagGraph[source.index]).token)<<std::endl; bool ret={expression}; if(ret){{source=s;vertices.push_back({{.parser_token=PARSER_{parser_token}, .token=graph::vertex_value(ldagGraph, ldagGraph[source.index]).token, .id=source.index}});if(vertices.size()>1)edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));}}return ret;\n{indent}}};\n\n", fmt::make_format_args(iArg, fArg, eArg, psArg));
        }
        constexpr typename std::string::value_type* fmt={"\n"};
        return fmt::format_to(ctx.out(), fmt);
    }
    // ...
    bool        curly{false};
    std::string value_format;
};

template <>
struct fmt::formatter<std::vector<std::tuple<std::string, std::string, std::string, bool>>>{
    auto parse(format_parse_context& ctx) -> format_parse_context::iterator{
        return ctx.begin();
    }
    
     auto format(const std::vector<std::tuple<std::string, std::string, std::string, bool>>& v, format_context& ctx) const -> format_context::iterator{
        std::string indentation(12, ' ');
            auto iArg=fmt::arg("indent", indentation);
            if(!v.empty())fmt::vformat_to(ctx.out(), "{indent}while(it!=utf16.end()){{\n{indent}   bool hitToken=false;LEXER_TOKEN winningToken;MODE winningMode=mode;std::u16string::const_iterator winningIt=it;bool winningPoppable=poppable;\n", fmt::make_format_args(iArg));
//         std::cout<<"tokens "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            std::string iA=std::get<0>(v[i]);
            std::string mA=(!std::get<1>(v[i]).empty())? "mode=="+std::get<1>(v[i])+" && " : "";
            std::string tA=std::get<2>(v[i]);
            auto fArg=fmt::arg("function", iA);
            auto mArg=fmt::arg("mode", mA);
            auto tArg=fmt::arg("token", tA);
            //auto eArg=fmt::arg("expression", eA);
            // if(i==0)
                fmt::vformat_to(ctx.out(), "{indent}nextMode=mode;poppable=false;\n{indent}if(std::u16string::const_iterator itTry=it;{mode}{function}(itTry) && std::distance(temp, winningIt)<std::distance(temp, itTry)){{\n{indent}   hitToken=true;winningToken={token};winningIt=itTry;winningMode=nextMode;winningPoppable=poppable;\n{indent}}}\n", fmt::make_format_args(iArg, fArg, mArg, tArg));
            // else
            //     fmt::vformat_to(ctx.out(), "{indent}}}else if({mode}{function}(it)){{\n{indent}   vertices.push_back({{.start=&(*temp), .stop=&(*it), .token={token}}});\n{indent}   edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));\n{indent}   depth++;\n", fmt::make_format_args(iArg, fArg, mArg, tArg));
        }
        if(!v.empty())return fmt::vformat_to(ctx.out(), "{indent}if(hitToken){{\n{indent}    vertices.push_back({{.start=&(*temp), .stop=&(*winningIt), .token=winningToken}});\n{indent}   edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));\n{indent}    it=winningIt;\n{indent}    if(winningMode!=mode)pushMode(winningMode);if(winningPoppable)popMode();\n{indent}}}else{{it++;}}\n{indent}    temp=it;\n{indent}}}\n", fmt::make_format_args(iArg));
        else return fmt::format_to(ctx.out(), "");
//        if(curly){
//            constexpr char* fmt={"}}"};
//            return std::format_to(ctx.out(), fmt);
//        }
//        else{
//            constexpr char* fmt={"]"};
//            return std::format_to(ctx.out(), fmt);
//        }
    }
    // ...
    bool        curly{false};
    std::string value_format;
};

template <>
struct fmt::formatter<std::vector<std::wstring>>{
    template<class wformat_parse_context>
    constexpr auto parse(wformat_parse_context& ctx) -> wformat_parse_context::iterator {
        return ctx.begin();
    }
    
    template<class wformat_context>
     auto format(const std::vector<std::wstring>& v, wformat_context& ctx) const -> wformat_context::iterator{
//        const auto&& buf=ctx.out();
//        if(curly){
//            constexpr char* fmt={"{{"};
//            std::format_to(ctx.out(), fmt);
//        }
//        else{
//            constexpr char* fmt{"["};
//            std::format_to(ctx.out(), fmt);            
//        }
 //        if (v.size() > 0){
//            const std::string_view vf=value_format;
//            size_t d=v[0];
//            std::format_to(ctx.out(), vf, d);
//        }
         //std::cout<<"tokens "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            fmt::vformat_to(ctx.out(), L"{:{}}", "", 8);
            std::wstring iA=v[i];
            if(i<v.size()-1)
                fmt::vformat_to(ctx.out(), L"{},\n", fmt::make_wformat_args(iA));
            else
                fmt::vformat_to(ctx.out(), L"{}\n", fmt::make_wformat_args(iA));
        }
        constexpr typename std::wstring::value_type* fmt={L"\n"};
        return fmt::format_to(ctx.out(), fmt);
//        if(curly){
//            constexpr char* fmt={"}}"};
//            return std::format_to(ctx.out(), fmt);
//        }
//        else{
//            constexpr char* fmt={"]"};
//            return std::format_to(ctx.out(), fmt);
//        }
    }
    // ...
    bool        curly{false};
    std::wstring value_format;
};

template <>
struct fmt::formatter<std::vector<std::tuple<std::wstring, std::wstring>>>{
    auto parse(wformat_parse_context& ctx) -> wformat_parse_context::iterator{
        return ctx.begin();
    }
    
     auto format(const std::vector<std::tuple<std::wstring, std::wstring>>& v, wformat_context& ctx) const -> wformat_context::iterator{
//        const auto&& buf=ctx.out();
//        if(curly){
//            constexpr char* fmt={"{{"};
//            std::format_to(ctx.out(), fmt);
//        }
//        else{
//            constexpr char* fmt{"["};
//            std::format_to(ctx.out(), fmt);            
//        }
 //        if (v.size() > 0){
//            const std::string_view vf=value_format;
//            size_t d=v[0];
//            std::format_to(ctx.out(), vf, d);
//        }
         //std::cout<<"tokens "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            std::wstring iA=std::get<0>(v[i]);
            std::wstring eA=std::get<1>(v[i]);
      auto fArg=fmt::arg(L"function", iA);
      auto eArg=fmt::arg(L"expression", eA);
                fmt::vformat_to(ctx.out(), L"\t\tstd::function<bool(std::u16string::const_iterator& it)> {function} = [](std::u16string::const_iterator& it) {{ /*{expression}*/return false; }};\n\n", fmt::make_wformat_args(fArg, eArg));
        }
        constexpr typename std::wstring::value_type* fmt={L"\n"};
        return fmt::format_to(ctx.out(), fmt);
//        if(curly){
//            constexpr char* fmt={"}}"};
//            return std::format_to(ctx.out(), fmt);
//        }
//        else{
//            constexpr char* fmt={"]"};
//            return std::format_to(ctx.out(), fmt);
//        }
    }
    // ...
    bool        curly{false};
    std::wstring value_format;
};


namespace sylvanmats::publishing{

    template<typename T>
    class CodeGenerator{
        protected:
        std::string ns;
        T blockComment{};
        T tokenVocab{};
        T tokenPrefix="LEXER_";
        T lexerGrammarTemplate{};
        T parserGrammarTemplate{};
        T parserClass{};
        std::vector<T> tokens;
        std::vector<T> modes{"DEFAULT", "Options", "Tokens", "Channels"};
        std::vector<std::tuple<T, T>> lexerRuleClasses;
        std::vector<std::tuple<std::string, std::string, std::string, bool>> ladderRules;
        std::vector<std::tuple<T, T, T>> parserRuleClasses;
        T primaryParserRule{};

        public:
        CodeGenerator() = delete;
        CodeGenerator(std::string& ns) : ns (ns){
            std::string templateLocation=(getenv("ANTLR4_DB_LOCATION")!=NULL) ? std::string(getenv("ANTLR4_DB_LOCATION"))+"/../templates/antlr4": "../templates/antlr4";
            std::filesystem::path path=templateLocation+"/lexer_grammar.txt";
    //        std::cout<<" "<<path.string()<<std::endl;
            //std::wstring_convert<std::codecvt_utf8_utf16<typename T::value_type>, typename T::value_type> cv;
            std::ifstream file(path);
            lexerGrammarTemplate=std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            std::filesystem::path ppath=templateLocation+"/parser_grammar.txt";
            std::ifstream pfile(ppath);
            parserGrammarTemplate=std::string((std::istreambuf_iterator<char>(pfile)), std::istreambuf_iterator<char>());
        };
        CodeGenerator(const CodeGenerator& orig) = delete;
        virtual ~CodeGenerator() = default;

        T operator()(){
            T bc=(!blockComment.empty())? blockComment : "\n";
            auto bcArg=fmt::arg("block_comment", bc);
            auto classArg=fmt::arg("class", parserClass);
            //T ns="code";
            auto nsArg=fmt::arg("namespace", ns);
            T lexerInclude=(!tokenVocab.empty())? "#include \""+tokenVocab+".h\"": "";
            auto tliArg=fmt::arg("token_vocab_include", lexerInclude);
            auto tlcArg=fmt::arg("token_vocab_class", tokenVocab);
            T tokenVocabInstance=tokenVocab;
            if(!tokenVocabInstance.empty())tokenVocabInstance.at(0)=std::tolower(tokenVocabInstance.at(0));
            auto tliiArg=fmt::arg("token_vocab_instance", tokenVocabInstance);
            auto tArg=fmt::arg("tokens", tokens);
            auto mArg=fmt::arg("modes", modes);
            std::cout<<"total modes#:  "<<modes.size()<<std::endl;
            auto lrArg=fmt::arg("lexer_rules", lexerRuleClasses);
            auto prArg=fmt::arg("parser_rules", parserRuleClasses);
            auto rlArg=fmt::arg("rules_ladder", ladderRules);
            auto pprArg=fmt::arg("primary_parser_rule", primaryParserRule);
              T ret=render(!tokenVocab.empty() ? parserGrammarTemplate : lexerGrammarTemplate, fmt::make_format_args(bcArg, tliArg, nsArg, classArg, tlcArg, tliiArg, tArg, mArg, lrArg, prArg, rlArg, pprArg));
              return ret;
          };

        void setBlockComment(T blockComment){this->blockComment=blockComment;};
        void setParserClass(T parserClass){this->parserClass=parserClass;};
        T& getParserClass(){return parserClass;};
        void setTokenVocab(T tokenVocab){this->tokenVocab=tokenVocab;};
        T& getTokenVocab(){return tokenVocab;};
        void setTokenPrefix(T tokenPrefix){this->tokenPrefix=tokenPrefix;};
        void appendToken(T t){tokens.push_back(t);};
        void appendLexerRuleClass(T t, T mode, T token, bool frag, bool skip , T expr){
            lexerRuleClasses.push_back(std::make_tuple(t, expr));
            if(!mode.empty() && std::none_of(modes.begin(), modes.end(), [&mode](std::string& s){return s.compare(mode)==0;}))modes.push_back(mode);
            if(!frag)ladderRules.push_back(std::make_tuple(t, mode, token, true));
        };
        void appendParserRuleClass(T t, T mode, T token, bool frag, bool skip, T expr){
            if(primaryParserRule.empty())primaryParserRule=t;
            parserRuleClasses.push_back(std::make_tuple(t, mode, expr));
            T cT=t;
            //std::transform(cT.cbegin(), cT.cend(), cT.begin(), [](const char& c){return std::toupper(c);});            
            //if(!frag)ladderRules.push_back(std::make_tuple(t, mode, cT, true));
        };  
        protected:

        T render(fmt::basic_string_view<typename T::value_type> users_fmt, fmt::format_args&& args){
           return fmt::vformat(users_fmt, args);
        }
    };
}
