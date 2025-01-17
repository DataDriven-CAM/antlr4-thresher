
#include <typeinfo>
#include <cxxabi.h>

#include "dsl/Morpher.h"
#include "publishing/CodeGenerator.h"
#include "parse/G4Reader.h"
 
namespace sylvanmats::dsl{
    Morpher::Morpher(std::filesystem::path& directory, sylvanmats::publishing::CodeGenerator<std::string>& codeGenerator) : directory (directory), codeGenerator (codeGenerator) {
    }

    void Morpher::operator()(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph){
        auto itDag = std::ranges::find_if(graph::vertices(dagGraph),
                                 [&](auto& u) { return graph::vertex_value(dagGraph, u).token == sylvanmats::antlr4::parse::ROOT; });
        graph::vertex_id_t<sylvanmats::antlr4::parse::G> vid=static_cast<graph::vertex_id_t<sylvanmats::antlr4::parse::G>>(itDag - begin(graph::vertices(dagGraph)));
        auto& v=dagGraph[vid];
        //std::cout<<"vid "<<vid<<" "<<graph::num_vertices(dagGraph)<<" "<<graph::num_edges(dagGraph)<<std::endl;
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
        std::wstring expr{};
        std::wstring slabel{};
        std::wstring label{};
        bool frag=false;
        for (auto&& oe : graph::edges(dagGraph, v)) {
            graph::container::csr_row<unsigned int>& o=dagGraph[graph::target_id(dagGraph, oe)];
            /*int status;
            char*realname = abi::__cxa_demangle(typeid(o).name(), nullptr, nullptr, &status);
            std::cout<<"o type: "<<typeid(o).name()<< " => "<<realname<<std::endl;
            free(realname);*/
            auto& nu=graph::vertex_value(dagGraph, v);
            auto& nv=graph::vertex_value(dagGraph, o);
            //std::cout<<" "<<(nv.stop-nv.start)<<" "<<nv.mode<<" "<<nu.token<<"\t\t"<<cv.to_bytes(label)<<" o inc edges#: "<<size(graph::edges(dagGraph, o))<<std::endl;
            if(nv.token==sylvanmats::antlr4::parse::ID){
                if(!expr.empty())expr.clear();
                slabel.assign(nu.start, nu.stop);
                label.assign(nv.start, nv.stop);
            }
            else if(nv.token==sylvanmats::antlr4::parse::COLON && !label.empty()){
                recurseLexerRule(dagGraph, o, expr);
                if(expr.empty())expr=L"false";
                if(std::isupper(label.at(0)))codeGenerator.appendLexerRuleClass(cv.to_bytes(label), "", frag, cv.to_bytes(expr));
                std::transform(label.cbegin(), label.cend(), label.begin(), [](const wchar_t& c){return std::toupper(c);});
                codeGenerator.appendToken(cv.to_bytes(label));
                frag=false;
            }
            else if(nv.token==sylvanmats::antlr4::parse::FRAGMENT){
                frag=true;
            }
            else if(nv.token==sylvanmats::antlr4::parse::IMPORT){
                std::cout<<"token==IMPORT "<<size(graph::edges(dagGraph, o))<<std::endl;
                for (auto&& ge : graph::edges(dagGraph, o)){
                    auto& g=dagGraph[graph::target_id(dagGraph, ge)];
                    auto& gv=graph::vertex_value(dagGraph, g);
                    if(gv.token==sylvanmats::antlr4::parse::ID){
                    std::wstring lexStr(gv.start, gv.stop);
                    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
                    std::filesystem::path importFile=cv.to_bytes(lexStr)+".g4";
                    std::filesystem::path importPath=directory/importFile;
                    std::string primaryParserClass=codeGenerator.getParserClass();
                    std::cout<<primaryParserClass<<" token==IMPORT "<<importPath.string()<<" "<<size(graph::edges(dagGraph, o))<<std::endl;
                    sylvanmats::antlr4::parse::G4Reader g4Reader;
                    g4Reader(importPath, [&](std::u16string& utf16, sylvanmats::antlr4::parse::G& dagGraph){
                        sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
                        morpher(utf16, dagGraph);
                    });
                    codeGenerator.setParserClass(primaryParserClass);

                    }
                }
            }
            else if(nv.token==sylvanmats::antlr4::parse::LEXER || nv.token==sylvanmats::antlr4::parse::PARSER){
                //std::wstring lexStr(nv.start, nv.stop);
                //std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
                //std::cout<<"token==LEXER "<<cv.to_bytes(lexStr)<<" "<<size(graph::edges(dagGraph, o))<<std::endl;
                for (auto&& ge : graph::edges(dagGraph, o)){
                auto& g=dagGraph[graph::target_id(dagGraph, ge)];
                auto& gv=graph::vertex_value(dagGraph, g);
                if(gv.token==sylvanmats::antlr4::parse::GRAMMAR){
                    for (auto&& ie : graph::edges(dagGraph, g)){
                    auto& i=dagGraph[graph::target_id(dagGraph, ie)];
                    auto& iv=graph::vertex_value(dagGraph, i);
                    std::wstring parserClass(iv.start, iv.stop);
                    codeGenerator.setParserClass(cv.to_bytes(parserClass));
                    }
                }
                }
            }
        }
    }

    bool Morpher::recurseLexerRule(sylvanmats::antlr4::parse::G& dagGraph, graph::container::csr_row<unsigned int>& source, std::wstring& expr){
        depth++;
        bool ret=false;
        std::wstring prevId{};
        bool rangeOn=false;
        for (auto&& se : graph::edges(dagGraph, source)){
            auto& v=dagGraph[graph::target_id(dagGraph, se)];
            auto& vv=graph::vertex_value(dagGraph, v);
            //std::cout<<"ID to "<<vv.token<<std::endl;
            if(vv.token==sylvanmats::antlr4::parse::ID){
                std::wstring expr2(vv.start, vv.stop);
                if(depth>2 && !orOn)expr+=L" && ";
                expr+=expr2+L"(temp)";
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
                //std::cout<<"recurse ID "<<cv.to_bytes(expr2)<<" "<<size(graph::edges(dagGraph, v))<<std::endl;
                orOn=false;
                if(size(graph::edges(dagGraph, v))>0)recurseLexerRule(dagGraph, v, expr);
            }
            else if(vv.token==sylvanmats::antlr4::parse::LPAREN){
                if(depth>2 && !orOn)expr+=L" && ";
                expr+=L"(";
                orOn=false;
                if(size(graph::edges(dagGraph, v))>0)recurseLexerRule(dagGraph, v, expr);
            }
            else if(vv.token==sylvanmats::antlr4::parse::RPAREN){
                expr+=L")";
                orOn=false;
                if(size(graph::edges(dagGraph, v))>0)recurseLexerRule(dagGraph, v, expr);
            }
            else if(vv.token==sylvanmats::antlr4::parse::LBRACK){
                expr+=L"(";
                std::cout<<"lbrack "<<size(graph::edges(dagGraph, v))<<std::endl;
                for (auto&& be : graph::edges(dagGraph, v)){
                    auto& b=dagGraph[graph::target_id(dagGraph, be)];
                    auto& bv=graph::vertex_value(dagGraph, b);
                    std::wstring expr2(bv.start, bv.stop);
                    if(expr2.compare(L"]")==0)break;
                    for(std::wstring::iterator itC=expr2.begin();itC!=expr2.end();itC++){
                        bool esc=false;
                        if((*itC)==L'\\'){itC++;esc=true;}
                        if((*itC)==L'-')expr+=L"&& ";
                        else if((*std::next(itC))==L'-'){
                            expr+=L"(*temp)>=u'";
                            if(esc || (*itC)==L'\'')expr+=L"\\";
                            expr+=std::wstring(1, (*itC));
                            expr+=L"' ";
                        }
                        else if((*std::prev(itC))==L'-'){
                            expr+=L"(*temp)<=u'";
                            if(esc || (*itC)==L'\'')expr+=L"\\";
                            expr+=std::wstring(1, (*itC));
                            expr+=L"' ";
                            if(std::next(itC)!=expr2.end())expr+=L" || ";
                        }
                        else {
                            expr+=L"(*temp)==u'";
                            if(esc || (*itC)==L'\'')expr+=(L"\\");
                            expr+=std::wstring(1, (*itC));
                            expr+=L"' ";
                            if(std::next(itC)!=expr2.end())expr+=L" || ";
                        }
                    }
                }
                expr+=L");temp++";
            }
            else if(vv.token==sylvanmats::antlr4::parse::PIPE){
                expr+=L" || ";
                orOn=true;
                if(size(graph::edges(dagGraph, v))>0)recurseLexerRule(dagGraph, v, expr);
            }
            else if(vv.token==sylvanmats::antlr4::parse::NOT){
                expr+=L" !";
                orOn=true;
                if(size(graph::edges(dagGraph, v))>0)recurseLexerRule(dagGraph, v, expr);
            }
            else if(vv.token==sylvanmats::antlr4::parse::PLUS){
                expr=L"false;while"+expr+L"ret=true";
            }
            else if(vv.token==sylvanmats::antlr4::parse::QUESTION){
                expr=L"false;if"+expr+L"ret=true";
            }
            else if(vv.token==sylvanmats::antlr4::parse::RANGE){
                rangeOn=true;
                std::cout<<"rangeOn "<<size(graph::edges(dagGraph, v))<<std::endl;
            }
            else if(vv.token==sylvanmats::antlr4::parse::STRING_LITERAL){
                std::wstring expr2(vv.start, vv.stop);
                if(expr2.size()==3){
                    if(rangeOn){
                        expr=L"(*temp)>=u"+prevId+L" && (*temp)<=u"+expr+L";temp++";
                    }
                    else
                        expr=L"(*temp)==u"+expr2+L";temp++";
                }
                else if(expr2.size()>3){
                    expr2.at(0)=L'"';
                    expr2.at(expr2.size()-1)=L'"';
                    expr+=L"std::u16ncmp(&(*temp), u"+expr2+L", "+std::to_wstring(expr2.size()-2)+L")==0";
                }
                prevId=expr2;
                orOn=false;
                rangeOn=false;
            }
        }
        depth--;
        return ret;
    }
}
