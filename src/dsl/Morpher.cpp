
#include <typeinfo>
#include <cxxabi.h>

#include "dsl/Morpher.h"
#include "publishing/CodeGenerator.h"
#include "parse/G4Reader.h"
#include "parse/LineColumnFinder.h"
 
namespace sylvanmats::dsl{
    Morpher::Morpher(std::filesystem::path& directory, sylvanmats::publishing::CodeGenerator<std::string>& codeGenerator) : directory (directory), codeGenerator (codeGenerator) {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
        if(!codeGenerator.getTokenVocab().empty()){
            lexerInstance=cv.from_bytes(codeGenerator.getTokenVocab())+u".";
            lexerInstance.at(0)=std::tolower(lexerInstance.at(0));
            tempInc=u"if(s.index+1<size(graph::vertices(ldagGraph)))s=ldagGraph[s.index+1]";
        }
    }

    void Morpher::operator()(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph){
        auto itDag = std::ranges::find_if(graph::vertices(dagGraph),
                                 [&](auto& u) { return graph::vertex_value(dagGraph, u).token == sylvanmats::antlr4::parse::ROOT; });
        graph::vertex_id_t<sylvanmats::antlr4::parse::G> vid=static_cast<graph::vertex_id_t<sylvanmats::antlr4::parse::G>>(itDag - begin(graph::vertices(dagGraph)));
        auto& v=dagGraph[vid];
        //std::cout<<"vid "<<vid<<" "<<graph::num_vertices(dagGraph)<<" "<<graph::num_edges(dagGraph)<<std::endl;
        this->operator()(g4Buffer, dagGraph, v);
        // for (auto&& oe : graph::edges(dagGraph, v)) {
        //     graph::container::csr_row<unsigned int>& o=dagGraph[graph::target_id(dagGraph, oe)];
        //     /*int status;
        //     char*realname = abi::__cxa_demangle(typeid(o).name(), nullptr, nullptr, &status);
        //     std::cout<<"o type: "<<typeid(o).name()<< " => "<<realname<<std::endl;
        //     free(realname);*/
        //     auto& nu=graph::vertex_value(dagGraph, v);
        //     auto& nv=graph::vertex_value(dagGraph, o);
        //     if(nv.token==sylvanmats::antlr4::parse::FRAGMENT){
        //         this->operator()(g4Buffer, dagGraph, o, true);
        //     }
        // }
    }

    void Morpher::operator()(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph, graph::container::csr_row<unsigned int>& v){
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
        std::vector<std::u16string> expr{std::u16string{}};
        std::u16string slabel{};
        std::u16string label{};
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
                if(expr.size()>=2)expr.resize(1);
                if(!expr.back().empty())expr.back().clear();
                slabel.assign(nu.start, nu.stop);
                label.assign(nv.start, nv.stop);
                frag=nv.frag;
            }
            else if(nv.token==sylvanmats::antlr4::parse::COLON && !label.empty()){
                expr.push_back(std::u16string{});
                recurseLexerRule(g4Buffer, dagGraph, o, expr);
                if(skip){skip=false;continue;}
                if(expr.back().empty())expr.back()=u"false";
                std::u16string exprLine{};
                for(std::u16string& entry : expr){exprLine.append(entry);}
                std::u16string cT=tokenPrefix+label;
                std::transform(cT.cbegin(), cT.cend(), cT.begin(), [](const char16_t& c){return std::toupper(c);});
                // std::cout<<cv.to_bytes(label)<<" COLON: "<<" "<<frag<<std::endl;
                if(std::isupper(label.at(0)))
                    codeGenerator.appendLexerRuleClass(cv.to_bytes(label), "", cv.to_bytes(cT), frag, cv.to_bytes(exprLine));
                else
                    codeGenerator.appendParserRuleClass(cv.to_bytes(label), "", cv.to_bytes(cT), frag, cv.to_bytes(exprLine));
                codeGenerator.appendToken(cv.to_bytes(cT));
                label.clear();
                frag=false;
            }
            else if(nv.token==sylvanmats::antlr4::parse::IMPORT){
                std::cout<<"token==IMPORT "<<size(graph::edges(dagGraph, o))<<std::endl;
                for (auto&& ge : graph::edges(dagGraph, o)){
                    auto& g=dagGraph[graph::target_id(dagGraph, ge)];
                    auto& gv=graph::vertex_value(dagGraph, g);
                    if(gv.token==sylvanmats::antlr4::parse::ID){
                    std::u16string lexStr(gv.start, gv.stop);
                    std::filesystem::path importFile=cv.to_bytes(lexStr)+".g4";
                    std::filesystem::path importPath=directory/importFile;
                    std::string primaryParserClass=codeGenerator.getParserClass();
                    std::cout<<primaryParserClass<<" token==IMPORT "<<importPath.string()<<" "<<size(graph::edges(dagGraph, o))<<std::endl;
                    sylvanmats::antlr4::parse::G4Reader g4Reader;
                    g4Reader(importPath, [&](std::u16string& utf16, std::unordered_map<std::u16string, std::u16string>& options, sylvanmats::antlr4::parse::G& dagGraph){
                        sylvanmats::dsl::Morpher morpher(directory, codeGenerator);
                        morpher(utf16, dagGraph);
                    });
                    codeGenerator.setParserClass(primaryParserClass);

                    }
                }
            }
            else if(nv.token==sylvanmats::antlr4::parse::LEXER || nv.token==sylvanmats::antlr4::parse::PARSER){
                //std::u16string lexStr(nv.start, nv.stop);
                //std::cout<<"token==LEXER/PARSER "<<cv.to_bytes(lexStr)<<" "<<size(graph::edges(dagGraph, o))<<std::endl;
                if(nv.token==sylvanmats::antlr4::parse::PARSER){
                    tokenPrefix=u"PARSER_";
                    codeGenerator.setTokenPrefix(cv.to_bytes(tokenPrefix));
                }
                codeGenerator.appendToken(cv.to_bytes(tokenPrefix+u"ROOT"));
                for (auto&& ge : graph::edges(dagGraph, o)){
                auto& g=dagGraph[graph::target_id(dagGraph, ge)];
                auto& gv=graph::vertex_value(dagGraph, g);
                if(gv.token==sylvanmats::antlr4::parse::GRAMMAR){
                    //std::cout<<"token==GRAMMAR "<<" "<<size(graph::edges(dagGraph, g))<<std::endl;
                    for (auto&& ie : graph::edges(dagGraph, g)){
                    auto& i=dagGraph[graph::target_id(dagGraph, ie)];
                    auto& iv=graph::vertex_value(dagGraph, i);
                    if(iv.token==sylvanmats::antlr4::parse::SEMI)continue;
                    std::u16string parserClass(iv.start, iv.stop);
                    //std::cout<<"parserClass "<<cv.to_bytes(parserClass)<<std::endl;
                    codeGenerator.setParserClass(cv.to_bytes(parserClass));
                    }
                }
                }
            }
        }
    }

    bool Morpher::recurseLexerRule(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph, graph::container::csr_row<unsigned int>& source, std::vector<std::u16string>& expr){
        bool ret=false;
        bool rangeOn=false;
            auto& sv=graph::vertex_value(dagGraph, source);
        int count=0;
        for (auto&& se : graph::edges(dagGraph, source)){
            graph::container::csr_row<unsigned int>& v=dagGraph[graph::target_id(dagGraph, se)];
            auto& vv=graph::vertex_value(dagGraph, v);
            std::u16string vvStr(vv.start, vv.stop);
            // {
            //     std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
            // std::u16string vvStr(vv.start, vv.stop);
            // std::cout<<" to "<<vv.token<<" "<<cv.to_bytes(vvStr)<<" "<<sylvanmats::antlr4::parse::PLUS<<" "<<(vv.token==sylvanmats::antlr4::parse::PLUS)<<std::endl;
            // }
            if(vv.token==sylvanmats::antlr4::parse::ID){
                auto& vp=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)+1]);
                auto& vm=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-1]);
                std::u16string expr2(vv.start, vv.stop);
                if(expr2.compare(u"EOF")==0)expr2=u"EndOfFile";
                if(vm.token!=sylvanmats::antlr4::parse::COLON && vm.token!=sylvanmats::antlr4::parse::PIPE && vm.token!=sylvanmats::antlr4::parse::LPAREN && vm.token!=sylvanmats::antlr4::parse::NOT && vm.token!=sylvanmats::antlr4::parse::ROOT)expr.back()+=u" && ";
                else if(vm.token==sylvanmats::antlr4::parse::RPAREN)expr.back()+=u" && ";
                if(vp.token==sylvanmats::antlr4::parse::PLUS)expr.back()+=u"[&]()->bool{bool ret=false;while(";
                else if(vp.token==sylvanmats::antlr4::parse::STAR)expr.back()+=u"[&]()->bool{bool ret=true;while(";
                else if(vp.token==sylvanmats::antlr4::parse::QUESTION)expr.back()+=u"[&]()->bool{bool ret=";
                if(!lexerInstance.empty())
                    if(std::isupper(expr2.at(0))){
                        std::transform(expr2.cbegin(), expr2.cend(), expr2.begin(), [](const char16_t& c){return std::toupper(c);});
                        expr.back()+=u"[&]()->bool{bool ret=graph::vertex_value(ldagGraph, ldagGraph[s.index]).token==LEXER_"+expr2+u"? true : false; if(ret){"+tempInc+u";return true;}else{ return false;}}()";
                        }
                    else
                        expr.back()+=expr2+u"(ldagGraph, s)";
                else
                    expr.back()+=expr2+u"(temp)";
//                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
//                std::cout<<"recurse ID "<<cv.to_bytes(expr2)<<" "<<size(graph::edges(dagGraph, v))<<" "<<(vp.token==sylvanmats::antlr4::parse::PLUS)<<std::endl;
                orOn=false;
            }
            else if(vv.token==sylvanmats::antlr4::parse::LPAREN){
                auto& vm=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-1]);
                if(vm.token!=sylvanmats::antlr4::parse::COLON && vm.token!=sylvanmats::antlr4::parse::PIPE && vm.token!=sylvanmats::antlr4::parse::LPAREN && vm.token!=sylvanmats::antlr4::parse::ROOT)expr.back()+=u" && ";
                expr.push_back(std::u16string{});
                expr.back()+=u"(";
                orOn=false;
            }
            else if(vv.token==sylvanmats::antlr4::parse::RPAREN){
                expr.back()+=u")";
                orOn=false;
                auto& vp=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)+1]);
                auto& vm=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-1]);
                if(expr.size()>=2 && (vp.token!=sylvanmats::antlr4::parse::PLUS && vp.token!=sylvanmats::antlr4::parse::STAR && vp.token!=sylvanmats::antlr4::parse::QUESTION)){
                    expr[expr.size()-2]+=expr.back();
                    expr.pop_back();
                }
            }
            else if(vv.token==sylvanmats::antlr4::parse::LBRACK){
                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
                std::u16string vvStr(vv.start, vv.stop);
                size_t endRBrack=size(graph::edges(dagGraph, v));
                if(endRBrack==0){
                    sylvanmats::antlr4::parse::LineColumnFinder lineColumnFinder;
                    auto&& [line, column]=lineColumnFinder(g4Buffer, vv.start);
                    throw Exception("morphing failure: "+cv.to_bytes(vvStr)+" has no edges. Ln "+std::to_string(line)+",Col "+std::to_string(column));
                }
                //for (auto&& be : graph::edges(dagGraph, v))endRBrack++;
                //std::cout<<cv.to_bytes(svStr)<<" start "<<cv.to_bytes(vvStr)<<" "<<endRBrack<<std::endl;
                auto& vp=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)+endRBrack]);
                auto& vm=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-1]);
                expr.push_back(std::u16string{});
                if(vm.token!=sylvanmats::antlr4::parse::COLON && vm.token!=sylvanmats::antlr4::parse::PIPE && vm.token!=sylvanmats::antlr4::parse::LPAREN && vm.token!=sylvanmats::antlr4::parse::LBRACK && vm.token!=sylvanmats::antlr4::parse::NOT && vm.token!=sylvanmats::antlr4::parse::ROOT)expr.back()+=u" && ";
                else if(vm.token==sylvanmats::antlr4::parse::RPAREN)expr.back()+=u" && ";
                else if(vm.token==sylvanmats::antlr4::parse::RBRACK)expr.back()+=u" && ";
                else if(vm.token==sylvanmats::antlr4::parse::STRING_LITERAL)expr.back()+=u" && ";
                if(vp.token==sylvanmats::antlr4::parse::PLUS)expr.back()+=u"[&]()->bool{bool ret=false;while(";
                else if(vp.token==sylvanmats::antlr4::parse::STAR)expr.back()+=u"[&]()->bool{bool ret=true;while(";
                else if(vp.token==sylvanmats::antlr4::parse::QUESTION)expr.back()+=u"[&]()->bool{bool ret=";
                else expr.back()+=u"[&]()->bool{bool ret=(";
                // std::u16string innerStr(vm.start, vp.stop);
                // sylvanmats::antlr4::parse::LineColumnFinder lineColumnFinder;
                // auto&& [line, column]=lineColumnFinder(g4Buffer, vv.start);
                // std::cout<<std::to_string(line)<<" "<<std::to_string(column)<<" lbrack "<<cv.to_bytes(innerStr)<<" "<<(vm.token==sylvanmats::antlr4::parse::NOT)<<" "<<size(graph::edges(dagGraph, v))<<" "<<size(graph::edges(dagGraph, v))<<std::endl;
                for (auto&& be : graph::edges(dagGraph, v)){
                    auto& b=dagGraph[graph::target_id(dagGraph, be)];
                    auto& bv=graph::vertex_value(dagGraph, b);
                    if(bv.token==sylvanmats::antlr4::parse::ARGUMENT){
                    std::u16string expr2(bv.start, bv.stop);
                std::u16string label2{};
                label2=expr2;
                std::cout<<endRBrack<<" arg "<<cv.to_bytes(label2)<<std::endl;

                    if(expr2.compare(u"]")==0)break;
                    for(std::u16string::iterator itC=expr2.begin();itC!=expr2.end();itC++){
                        bool esc=false;
                        if((*itC)==u'\\'){itC++;esc=true;}
                        if((*itC)==u'+' && (*std::next(itC))==u'-'){
                            expr.back()+=u"(*temp)==u'";
                            expr.back()+=std::u16string(1, (*itC));
                            expr.back()+=u"' || (*temp)==u'";
                            itC++;
                            expr.back()+=std::u16string(1, (*itC));
                            expr.back()+=u"'";
                        }
                        else if((*itC)==u'-' && (*std::next(itC))!=u']')expr.back()+=u"&& ";
                        else if((*std::next(itC))==u'-'){
                            expr.back()+=u"(*temp)>=u'";
                            if(esc || (*itC)==L'\'')expr.back()+=u"\\";
                            expr.back()+=std::u16string(1, (*itC));
                            expr.back()+=u"' ";
                        }
                        else if((*std::prev(itC))==u'-'){
                            expr.back()+=u"(*temp)<=u'";
                            if(esc || (*itC)==u'\'')expr.back()+=u"\\";
                            expr.back()+=std::u16string(1, (*itC));
                            expr.back()+=u"' ";
                            if(std::next(itC)!=expr2.end())expr.back()+=u" || ";
                        }
                        else {
                            expr.back()+=u"(*temp)==u'";
                            if(esc || (*itC)==u'\'')expr.back()+=(u"\\");
                            else if(esc || (*itC)==u'"')expr.back()+=(u"\\");
                            expr.back()+=std::u16string(1, (*itC));
                            expr.back()+=u"' ";
                            if(std::next(itC)!=expr2.end())expr.back()+=u" || ";
                            std::cout<<endRBrack<<"else arg "<<cv.to_bytes(expr.back())<<std::endl;
                        }
                    }
                }
                }
                if(vp.token==sylvanmats::antlr4::parse::PLUS)expr.back()+=u"){temp++;ret=true;}return ret;}()";
                else if(vp.token==sylvanmats::antlr4::parse::STAR)expr.back()+=u"){temp++;ret=true;}return ret;}()";
                else if(vp.token==sylvanmats::antlr4::parse::QUESTION)expr.back()+=u"? true : false; if(ret){return true;}else{ return true;}}()";
                else expr.back()+=u")? true:false; if(ret)temp++;return ret;}()";
                if(expr.size()>=2)expr[expr.size()-2]+=expr.back();
                if(expr.size()>=2){expr.pop_back();}
                //std::u16string label{};
                //label=expr.back();
                //std::cout<<endRBrack<<" lbrack "<<cv.to_bytes(label)<<std::endl;
            }
            else if(vv.token==sylvanmats::antlr4::parse::RBRACK || vv.token==sylvanmats::antlr4::parse::ARGUMENT){  
                //std::cout<<" rbrack "<<std::endl;
            }
            else if(vv.token==sylvanmats::antlr4::parse::PIPE){
                expr.back()+=u" || ";
                orOn=true;
            }
            else if(vv.token==sylvanmats::antlr4::parse::NOT){
                expr.back()+=u" !";
            }
            else if(vv.token==sylvanmats::antlr4::parse::PLUS){
                auto& vm=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-1]);
//                std::cout<<"2 Plus "<<v.index<<" "<<vm.token<<" "<<sylvanmats::antlr4::parse::RPAREN<<" "<<expr.size()<<std::endl;
                /*if(vm.token==sylvanmats::antlr4::parse::RPAREN){
                    if(expr.size()>=2){expr[expr.size()-2]+=u"[&]()->bool{bool ret=false;while("+expr.back()+u"){ret=true;}return ret;}()";expr.pop_back();}
                }
                else */if(vm.token!=sylvanmats::antlr4::parse::RBRACK){
                    if(vm.token!=sylvanmats::antlr4::parse::ID){if(expr.size()>=2){expr[expr.size()-2]+=u"[&]()->bool{bool ret=false;while("+expr.back()+u"){ret=true;}return ret;}()";expr.pop_back();}}
                    else {expr.back()+=u"){ret=true;}return ret;}()";if(expr.size()>=2){expr[expr.size()-2]+=expr.back();expr.pop_back();}}
                }
            }
            else if(vv.token==sylvanmats::antlr4::parse::STAR){
                auto& vm=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-1]);
                if(vm.token!=sylvanmats::antlr4::parse::RBRACK){
                    if(vm.token!=sylvanmats::antlr4::parse::ID){if(expr.size()>=2)expr[expr.size()-2]+=u"[&]()->bool{bool ret=true;while("+expr.back()+u"){ret=true;}return ret;}()";}
                    else {expr.back()+=u"){ret=true;}return ret;}()";if(expr.size()>=2)expr[expr.size()-2]+=expr.back();}
                    //std::u16string label{};
                    //label=expr.back();
                    //std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
                    //std::cout<<" STAR "<<cv.to_bytes(label)<<std::endl;
                    if(expr.size()>=2){expr.pop_back();}
                }
            }
            else if(vv.token==sylvanmats::antlr4::parse::QUESTION){
                auto& vm=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-1]);
                if(vm.token==sylvanmats::antlr4::parse::RBRACE){}
                else if(vm.token!=sylvanmats::antlr4::parse::RBRACK){
                    if(vm.token!=sylvanmats::antlr4::parse::ID){if(expr.size()>=2)expr[expr.size()-2]+=u"[&]()->bool{bool ret="+expr.back()+u"? true : false; if(ret){return true;}else{ return true;}}()";}
                    else {expr.back()+=u"? true : false; if(ret){return true;}else{ return true;}}()";if(expr.size()>=2)expr[expr.size()-2]+=expr.back();}
                    if(expr.size()>=2){expr.pop_back();}
                }
            }
            else if(vv.token==sylvanmats::antlr4::parse::RANGE){
                rangeOn=true;
            }
            else if(vv.token==sylvanmats::antlr4::parse::RARROW){
                //rangeOn=true;
                auto& vp=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)+1]);
                std::cout<<vv.token<<" rarrow "<<size(graph::edges(dagGraph, v))<<" "<<vp.token<<" "<<(vp.token==sylvanmats::antlr4::parse::SKIP)<<std::endl;
                if(vp.token==sylvanmats::antlr4::parse::SKIP)skip=true;
                break;
            }
            else if(vv.token==sylvanmats::antlr4::parse::STRING_LITERAL){
                auto& vp=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)+1]);
                auto& vm=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-1]);
                std::u16string expr2(vv.start, vv.stop);
                if(vm.token!=sylvanmats::antlr4::parse::COLON && vm.token!=sylvanmats::antlr4::parse::PIPE && vm.token!=sylvanmats::antlr4::parse::LPAREN && vm.token!=sylvanmats::antlr4::parse::ROOT)expr.back()+=u" && ";
                if(expr2.size()<3){}
                else if(expr2.size()>=9 && expr2.at(1)==u'\\' && expr2.at(2)==u'u' && expr2.at(3)==u'{' && expr2.at(expr2.size()-2)==u'}'){
                    std::u16string digitCode=expr2.substr(4, expr2.size()-6);
                    std::u16string lowerDigitCode=digitCode.substr(digitCode.size()-4);
                    std::u16string upperDigitCode=digitCode.substr(0, digitCode.size()-4);
                    if(upperDigitCode.size()<4)upperDigitCode=std::u16string(4-upperDigitCode.size(), u'0')+upperDigitCode;
                    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
                    //std::cout<<"morph SL "<<cv.to_bytes(digitCode)<<" "<<cv.to_bytes(lowerDigitCode)<<" upperDigitCode:"<<cv.to_bytes(upperDigitCode)<<std::endl;
                    if(vp.token==sylvanmats::antlr4::parse::RANGE){
                //auto& vm3=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-2]);
                //if(vm3.token!=sylvanmats::antlr4::parse::COLON && vm3.token!=sylvanmats::antlr4::parse::PIPE && vm3.token!=sylvanmats::antlr4::parse::LPAREN && vm3.token!=sylvanmats::antlr4::parse::ROOT)expr.back()+=u" && ";
                //else if(vm3.token==sylvanmats::antlr4::parse::RPAREN)expr.back()+=u" && ";
                //auto& vp3=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)+3]);
                //if(vp3.token==sylvanmats::antlr4::parse::PLUS)expr.back()+=u"[&]()->bool{bool ret=false;while(";
                //else if(vp3.token==sylvanmats::antlr4::parse::STAR)expr.back()+=u"[&]()->bool{bool ret=true;while(";
                //else if(vp3.token==sylvanmats::antlr4::parse::QUESTION)expr.back()+=u"[&]()->bool{bool ret=";
                         expr.back()+=u"[&]()->bool{if((*temp)>'\\u"+upperDigitCode+u"' || ((*temp)=='\\u"+upperDigitCode+u"' && (*(temp+1))>='\\u"+lowerDigitCode+u"')){return true;}else return false;}()";
                    }
                    else if(vm.token==sylvanmats::antlr4::parse::RANGE){
                        expr.back()+=u"[&]()->bool{if((*temp)<'\\u"+upperDigitCode+u"' || ((*temp)=='\\u"+upperDigitCode+u"' && (*(temp+1))<='\\u"+lowerDigitCode+u"')){temp+=2;return true;}else return false;}()";
                    }
                    else
                        expr.back()+=u"[&]()->bool{ibool ret=((*temp)=='\\u"+upperDigitCode+u"' && (*(temp+1))=='\\u"+lowerDigitCode+u"')? true:false;if(ret{temp+=2;return true;}else return false;}()";
                }
                else if(expr2.size()==3 || (expr2.size()==4 && expr2.at(1)==u'\\') || (expr2.size()==8 && expr2.at(1)==u'\\' && expr2.at(2)==u'u')){
                    //std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
                    //std::cout<<"morph SL "<<cv.to_bytes(expr2)<<" "<<(graph::target_id(dagGraph, se)+1)<<" "<<graph::target_id(dagGraph, se)<<" "<<(graph::target_id(dagGraph, se)-1)<<" "<<vp.token<<" "<<vm.token<<" "<<sylvanmats::antlr4::parse::RANGE<<std::endl;
                    if(vp.token==sylvanmats::antlr4::parse::RANGE){
                //auto& vm3=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-2]);
                //if(vm3.token!=sylvanmats::antlr4::parse::COLON && vm3.token!=sylvanmats::antlr4::parse::PIPE && vm3.token!=sylvanmats::antlr4::parse::LPAREN && vm3.token!=sylvanmats::antlr4::parse::ROOT)expr.back()+=u" && ";
                //else if(vm3.token==sylvanmats::antlr4::parse::RPAREN)expr.back()+=u" && ";
                //auto& vp3=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)+3]);
                //if(vp3.token==sylvanmats::antlr4::parse::PLUS)expr.back()+=u"[&]()->bool{bool ret=false;while(";
                //else if(vp3.token==sylvanmats::antlr4::parse::STAR)expr.back()+=u"[&]()->bool{bool ret=true;while(";
                //else if(vp3.token==sylvanmats::antlr4::parse::QUESTION)expr.back()+=u"[&]()->bool{bool ret=";
                       expr.back()+=u"[&]()->bool{if((*temp)>=u"+expr2+u"){temp++;return true;}else return false;}()";
                    }
                    else if(vm.token==sylvanmats::antlr4::parse::RANGE){
                        expr.back()+=u"[&]()->bool{if((*temp)<=u"+expr2+u"){temp++;return true;}else return false;}()";
                    }
                    else
                        expr.back()+=u"[&]()->bool{if((*temp)==u"+expr2+u"){temp++;return true;}else return false;}()";
                }
                else if(expr2.size()>3){
                            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
                    std::u16string text(std::next(expr2.begin()), std::prev(expr2.end()));
                    size_t offset=text.find(u"\"");
                    while(offset!=std::u16string::npos){
                        text.insert(text.find(u"\""), 1, u'\\');
                        offset=text.find(u"\"", offset+2);
                    }
                    //expr2.at(0)=u'"';
                    //expr2.at(expr2.size()-1)=u'"';
                    if(vp.token==sylvanmats::antlr4::parse::RANGE){
                //auto& vm3=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)-2]);
                //if(vm3.token!=sylvanmats::antlr4::parse::COLON && vm3.token!=sylvanmats::antlr4::parse::PIPE && vm3.token!=sylvanmats::antlr4::parse::LPAREN && vm3.token!=sylvanmats::antlr4::parse::ROOT)expr.back()+=u" && ";
                //else if(vm3.token==sylvanmats::antlr4::parse::RPAREN)expr.back()+=u" && ";
                //auto& vp3=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, se)+3]);
                //if(vp3.token==sylvanmats::antlr4::parse::PLUS)expr.back()+=u"[&]()->bool{bool ret=false;while(";
                //else if(vp3.token==sylvanmats::antlr4::parse::STAR)expr.back()+=u"[&]()->bool{bool ret=true;while(";
                //else if(vp3.token==sylvanmats::antlr4::parse::QUESTION)expr.back()+=u"[&]()->bool{bool ret=";
                        expr.back()+=u"std::u16ncmp(&(*temp), u\""+text+u"\", "+cv.from_bytes(std::to_string(text.size()))+u")==0";
                    }
                    else if(vm.token==sylvanmats::antlr4::parse::RANGE){
                        expr.back()+=u"std::u16ncmp(&(*temp), u\""+text+u"\", "+cv.from_bytes(std::to_string(text.size()))+u")==0";
                    }
                    else
                        expr.back()+=u"[&]()->bool{if(std::u16ncmp(&(*temp), u\""+text+u"\", "+cv.from_bytes(std::to_string(text.size()))+u")==0){std::advance(temp,"+cv.from_bytes(std::to_string(text.size()))+u");return true;}else return false;}()";
                }
                orOn=false;
                rangeOn=false;
            }
                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
            if(size(graph::edges(dagGraph, v))>0){recurseLexerRule(g4Buffer, dagGraph, dagGraph[graph::target_id(dagGraph, se)], expr);}
            count++;
        }
        return ret;
    }

}
