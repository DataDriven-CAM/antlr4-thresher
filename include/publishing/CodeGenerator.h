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
            auto iArg=fmt::arg("indent", indentation);
            auto fArg=fmt::arg("function", iA);
            auto eArg=fmt::arg("expression", eA);
            fmt::vformat_to(ctx.out(), "{indent}std::function<bool(std::u16string::const_iterator&, LG&)> {function} = [&](std::u16string::const_iterator& it, LG& dagGraph) {{std::u16string::const_iterator temp=it; bool ret={expression}; if(ret)it=temp;return ret;}};\n\n", fmt::make_format_args(iArg, fArg, eArg));
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
            if(!v.empty())fmt::vformat_to(ctx.out(), "{indent}while(it!=utf16.end()){{\n", fmt::make_format_args(iArg));
//         std::cout<<"tokens "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            std::string iA=std::get<0>(v[i]);
            std::string eA=std::get<1>(v[i]);
            std::string tA=std::get<2>(v[i]);
            auto fArg=fmt::arg("function", iA);
            auto tArg=fmt::arg("token", tA);
            //auto eArg=fmt::arg("expression", eA);
            if(i==0)
                fmt::vformat_to(ctx.out(), "{indent}if({function}(it)){{\n{indent}   vertices.push_back({{.start=&(*temp), .stop=&(*it), .token={token}}});\n{indent}   edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));\n{indent}   depth++;\n", fmt::make_format_args(iArg, fArg, tArg));
            else
                fmt::vformat_to(ctx.out(), "{indent}}}else if({function}(it)){{\n{indent}   vertices.push_back({{.start=&(*temp), .stop=&(*it), .token={token}}});\n{indent}   edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));\n{indent}   depth++;\n", fmt::make_format_args(iArg, fArg, tArg));
        }
        if(!v.empty())return fmt::vformat_to(ctx.out(), "{indent}}}else{{it++;}}\ntemp=it;}}\n", fmt::make_format_args(iArg));
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
        std::vector<std::tuple<T, T>> lexerRuleClasses;
         std::vector<std::tuple<std::string, std::string, std::string, bool>> ladderRules;
        std::vector<std::tuple<T, T, T>> parserRuleClasses;
        //std::vector<std::tupe<std::string, std::args>> rules;

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
            auto lrArg=fmt::arg("lexer_rules", lexerRuleClasses);
            auto prArg=fmt::arg("parser_rules", parserRuleClasses);
            auto rlArg=fmt::arg("rules_ladder", ladderRules);
              T ret=render(!tokenVocab.empty() ? parserGrammarTemplate : lexerGrammarTemplate, fmt::make_format_args(bcArg, tliArg, nsArg, classArg, tlcArg, tliiArg, tArg, lrArg, prArg, rlArg));
              return ret;
          };

        void setBlockComment(T blockComment){this->blockComment=blockComment;};
        void setParserClass(T parserClass){this->parserClass=parserClass;};
        T& getParserClass(){return parserClass;};
        void setTokenVocab(T tokenVocab){this->tokenVocab=tokenVocab;};
        T& getTokenVocab(){return tokenVocab;};
        void setTokenPrefix(T tokenPrefix){this->tokenPrefix=tokenPrefix;};
        void appendToken(T t){tokens.push_back(t);};
        void appendLexerRuleClass(T t, T mode, T token, bool frag, T expr){
            lexerRuleClasses.push_back(std::make_tuple(t, expr));
            if(!frag)ladderRules.push_back(std::make_tuple(t, mode, token, true));
        };
        void appendParserRuleClass(T t, T mode, T token, bool frag, T expr){
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
