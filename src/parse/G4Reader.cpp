#include "parse/G4Reader.h"

#include <ranges>
#include "mio/mmap.hpp"

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include <fmt/ranges.h>

namespace std{
    size_t u16ncmp(const char16_t* a, const char16_t* b, size_t n){
        for(size_t i=0;i<n;i++){
            if(a[i]>b[i])return 1;
            else if(a[i]<b[i])return -1;
        }
        return 0;
    }

}

namespace sylvanmats::antlr4::parse {

    void G4Reader::operator()(std::filesystem::path& filePath, std::function<void(std::u16string& utf16, std::unordered_map<std::u16string, std::u16string>& options, G& dagGraph)> apply){
        try {
            if(filePath.has_parent_path())
                directory=filePath.parent_path();
            const int fdAntlr = open(filePath.c_str(), O_RDONLY);
            mio::mmap_source mmapAntlr(fdAntlr, 0, mio::map_entire_file);
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
            std::u16string utf16 = utf16conv.from_bytes(mmapAntlr.begin());
            this->operator()(utf16, apply);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void G4Reader::operator()(std::u16string& utf16, std::function<void(std::u16string& utf16, std::unordered_map<std::u16string, std::u16string>& options, G& dagGraph)> apply){
        try {
            std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cv;
            //std::cout<<"modeMap size "<<modeMap.size()<<" "<<tokenMap.size()<<std::endl;
            //lemon::ListGraph astGraph;
            //lemon::ListGraph::NodeMap<ast_node> astNode(astGraph);
            std::u16string::const_iterator it = utf16.begin();
            int count=0;
            int depth=0;
            vertices.push_back({.start=&(*it), .stop=&(*it), .token=ROOT});
            if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
            depthProfile[depth].push_back(vertices.size()-1);
            depth++;
            itEnd=utf16.end();
            std::u16string::const_iterator temp=it;
            while(it!=utf16.end()){
            if(DocComment(DOC_COMMENT, DEFAULT, it)){
                /*if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(0, vertices.size()-1, 1));*/
                std::u16string label(&(*temp), &(*it));
                std::cout<<"DocComment size: "<<(vertices.back().stop-vertices.back().start)<<" "<<cv.to_bytes(label)<<std::endl;
            }
            else if(BlockComment(it)){
                /*vertices.push_back({.start=&(*temp), .stop=&(*it), .token=LINE_COMMENT});
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));*/
                std::u16string label(&(*temp), &(*it));
                std::cout<<"BlockComment size: "<<(vertices.back().stop-vertices.back().start)<<" "<<cv.to_bytes(label)<<std::endl;
            }
            else if(LineComment(it)){
                /*vertices.push_back({.start=&(*temp), .stop=&(*it), .token=LINE_COMMENT});
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                depth=1;*/
                std::u16string label(&(*temp), &(*it));
                std::cout<<"LineComment size: "<<(vertices.back().stop-vertices.back().start)<<" "<<cv.to_bytes(label)<<std::endl;
            }
            else if((*it)==u'0' || ((*it)>=u'1' && (*it)<=u'9')){
                vertices.push_back({.start=&(*it), .token=INT});
                //lemon::ListGraph::Node n=astGraph.addNode();
                //astNode[n].start=&(*it);
                //astNode[n].token=INT;
                ++it;
                bool hitStop=false;
                while(!hitStop && it!=utf16.end()){
                    if((*it)>=u'0' && (*it)<=u'9'){
                        ++it;
                    }
                    else hitStop=true;
                }
                //astNode[n].stop=&(*it);
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                depth=1;
            }
            else if([&]()->bool{
                temp=it;
                if((*temp)==u'\''){
//                    std::u16string v(u"'\r\n\\");
                    ++temp;
                    bool hitEscapes=false;
                    while(!EndOfFile(temp) && (*temp)!=u'\''){
                        if((*temp)==u'\\' && (*std::next(temp))==u'\'' && (*std::prev(temp))!=u'\\')++temp;
                        ++temp;
                    }
                    if((*temp)==u'\''){
                    ++temp;
                    return true;
                    }
                }
                return false;
            }()){
                vertices.push_back({.start=&(*it), .token=STRING_LITERAL});
                it=temp;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                depth++;
            }
            else if(std::u16ncmp(&(*it), u"options", 7)==0){
                vertices.push_back({.start=&(*it), .token=OPTIONS, .mode=Options});
                std::advance(it, 7);
                vertices.back().stop=&(*it);
                while(it!=utf16.end()){
                    if(DocComment(OPT_DOC_COMMENT, Options, it)){
                    }
                    else if(BlockComment(it)){
                    }
                    else if(LineComment(it)){
                    }
                    else if((*it)==u'{'){
                        vertices.push_back({.start=&(*it), .token=OPT_LBRACE, .mode=Options});
                        ++it;
                        temp=it;
                        vertices.back().stop=&(*it);
                        if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                        depthProfile[depth].push_back(vertices.size()-1);
                        edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                        depth++;
                        while(it!=utf16.end() && (*it)!=u'}'){
                            it++;
                        }
                        vertices.push_back({.start=&(*temp), .stop=&(*it), .token=ARGUMENT});
                        edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                        std::u16string arg(vertices.back().start, vertices.back().stop);
                        std::u16string delim=u";";
                        for (const auto& word : arg | std::views::split(delim)){
                                std::u16string_view wv(word.begin(), word.end());
                                wv.remove_prefix(std::min(wv.find_first_not_of(u" \t\r\v\n"), wv.size()));
                                wv.remove_suffix(std::min(wv.size() - wv.find_last_not_of(u" \t\r\v\n") - 1, wv.size()));
                            std::u16string w(wv.begin(), wv.end());
                            if(w.empty())continue;
                            std::u16string v{};
                            std::u16string p{};
                            bool hitV=true;
                                std::u16string idelim=u"=";
                            for (const auto& iword : w | std::views::split(idelim)){
                                std::u16string_view iw(iword.begin(), iword.end());
                                iw.remove_prefix(std::min(iw.find_first_not_of(u" \t\r\v\n"), iw.size()));
                                iw.remove_suffix(std::min(iw.size() - iw.find_last_not_of(u" \t\r\v\n") - 1, iw.size()));
                                //std::trim(iw);
                                //std::cout<<"option: "<<cv.to_bytes(iw)<<std::endl;
                                if(hitV)v=std::u16string(iw.begin(), iw.end());
                                else p=std::u16string(iw.begin(), iw.end());
                                hitV=!hitV;
                            }
                            std::cout<<"option: "<<cv.to_bytes(v)<<" "<<cv.to_bytes(p)<<std::endl;
                            options[v]=p;
                        }                        
                        vertices.push_back({.start=&(*it), .token=OPT_RBRACE});
                        ++it;
                        vertices.back().stop=&(*it);
                        if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                        depthProfile[depth].push_back(vertices.size()-1);
                        bool hit=false;
                        size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                        if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                        depth=1;
                        break;
                    }
                    else ++it;
                    count++;
                }
            }
            else if(std::u16ncmp(&(*it), u"tokens", 6)==0){
                vertices.push_back({.start=&(*it), .token=TOKENS, .mode=Tokens});
                std::advance(it, 6);
                vertices.back().stop=&(*it);
                while(it!=utf16.end()){
                    if(DocComment(TOK_DOC_COMMENT, Tokens, it)){
                    }
                    else if(BlockComment(it)){
                    }
                    else if(LineComment(it)){
                    }
                    else if((*it)==u'{'){
                        vertices.push_back({.start=&(*it), .token=TOK_LBRACE, .mode=Tokens});
                        ++it;
                        temp=it;
                        vertices.back().stop=&(*it);
                        if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                        depthProfile[depth].push_back(vertices.size()-1);
                        edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                        depth++;
                        while(it!=utf16.end() && (*it)!=u'}'){
                            it++;
                        }
                        vertices.push_back({.start=&(*temp), .stop=&(*it), .token=ARGUMENT});
                        edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                        std::u16string arg(vertices.back().start, vertices.back().stop);
                        std::u16string delim=u",";
                        for (const auto& word : arg | std::views::split(delim)){
                                std::u16string_view wv(word.begin(), word.end());
                                wv.remove_prefix(std::min(wv.find_first_not_of(u" \t\r\v\n"), wv.size()));
                                wv.remove_suffix(std::min(wv.size() - wv.find_last_not_of(u" \t\r\v\n") - 1, wv.size()));
                            std::u16string w(wv.begin(), wv.end());
                            if(w.empty())continue;
                            std::cout<<"token: "<<cv.to_bytes(w)<<std::endl;
                            tokens.push_back(w  );
                        }                        
                        vertices.push_back({.start=&(*it), .token=TOK_RBRACE});
                        ++it;
                        vertices.back().stop=&(*it);
                        if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                        depthProfile[depth].push_back(vertices.size()-1);
                        bool hit=false;
                        size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                        if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                        depth=1;
                        break;
                    }
                    else ++it;
                    count++;
                }

            }
            else if(std::u16ncmp(&(*it), u"channels", 8)==0){
                vertices.push_back({.start=&(*it), .token=CHANNELS, .mode=Channels});
                std::advance(it, 8);
                vertices.back().stop=&(*it);
                while(it!=utf16.end()){
                    if(DocComment(CHN_DOC_COMMENT, Channels, it)){
                        std::u16string label(&(*temp), &(*it));
                        std::cout<<"DocComment size: "<<(vertices.back().stop-vertices.back().start)<<" "<<cv.to_bytes(label)<<std::endl;
                    }
                    else if(BlockComment(it)){
                        std::u16string label(&(*temp), &(*it));
                        std::cout<<"BlockComment size: "<<(vertices.back().stop-vertices.back().start)<<" "<<cv.to_bytes(label)<<std::endl;
                    }
                    else if(LineComment(it)){
                        std::u16string label(&(*temp), &(*it));
                        std::cout<<"LineComment size: "<<(vertices.back().stop-vertices.back().start)<<" "<<cv.to_bytes(label)<<std::endl;
                    }
                    else if((*it)==u'{'){
                        vertices.push_back({.start=&(*it), .token=CHN_LBRACE, .mode=Channels});
                        ++it;
                        vertices.back().stop=&(*it);
                        if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                        depthProfile[depth].push_back(vertices.size()-1);
                        bool hit=false;
                        size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                        if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                        depth++;
                    }
                    else if((*it)==u'}'){
                        vertices.push_back({.start=&(*it), .token=CHN_RBRACE, .mode=Channels});
                        ++it;
                        vertices.back().stop=&(*it);
                        if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                        depthProfile[depth].push_back(vertices.size()-1);
                        bool hit=false;
                        size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                        if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                        depth=1;
                        break;
                    }
                    else if([&]()->bool{
                        if(NameStartChar(it)){
                            vertices.push_back({.start=&(*it), .token=CHN_ID, .mode=Channels});
                            do{
                            ++it;                    
                            }while(NameChar(it));
                            vertices.back().stop=&(*it);
                            std::u16string label(vertices.back().start, vertices.back().stop);
                                    //std::cout<<"ID size: "<<(vertices.back().stop-vertices.back().start)<<" "<<cv.to_bytes(label)<<std::endl;
                            if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                            depthProfile[depth].push_back(vertices.size()-1);
                            if(depth==0)edges.push_back(std::make_tuple(0, vertices.size()-1, 1));
                            else{
                                bool hit=false;
                                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                            }
                            return true;
                        }
                        return false;
                    }()){

                    }
                    else if(Dot(it)){
                        vertices.push_back({.start=&(*temp), .stop=&(*it), .token=CHN_DOT, .mode=Channels});
                        ++it;
                        vertices.back().stop=&(*it);
                    }
                    else if((*it)==u','){
                        vertices.push_back({.start=&(*it), .token=CHN_COMMA, .mode=Channels});
                       ++it;
                        vertices.back().stop=&(*it);
                        if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                        depthProfile[depth].push_back(vertices.size()-1);
                        bool hit=false;
                        size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                        if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                    }
                    else ++it;
                    count++;
                }

            }
            else if(std::u16ncmp(&(*it), u"import", 6)==0){
                std::cout<<"g4 import "<<" "<<vertices.size()<<std::endl;
                vertices.push_back({.start=&(*it), .token=IMPORT});
                std::advance(it, 6);
                vertices.back().stop=&(*it);
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(0, vertices.size()-1, 1));
                depth++;
            }
            else if(std::u16ncmp(&(*it), u"parser", 6)==0){
                std::cout<<"parser: "<<std::endl;
                vertices.push_back({.start=&(*it), .token=PARSER});
                std::advance(it, 6);
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(0, vertices.size()-1, 1));
                depth++;
            }
            else if(std::u16ncmp(&(*it), u"lexer", 5)==0){
                vertices.push_back({.start=&(*it), .token=LEXER});
                std::advance(it, 5);
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(0, vertices.size()-1, 1));
                depth++;
            }
            else if(std::u16ncmp(&(*it), u"grammar", 7)==0){
                std::cout<<"grammar: "<<std::endl;
                vertices.push_back({.start=&(*it), .token=GRAMMAR});
                std::advance(it, 7);
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                depth++;
            }
            else if(std::u16ncmp(&(*it), u"fragment", 8)==0){
                vertices.push_back({.start=&(*it), .token=FRAGMENT});
                std::advance(it, 8);
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(0, vertices.size()-1, 1));
                depth++;
            }
            else if(std::u16ncmp(&(*it), u"mode", 4)==0){
                vertices.push_back({.start=&(*it), .token=MODE});
                std::advance(it, 4);
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(0, vertices.size()-1, 1));
                //depth=1;
            }
            else if(std::u16ncmp(&(*it), u"type", 4)==0){
                vertices.push_back({.start=&(*it), .token=TYPE});
                std::advance(it, 4);
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth=1;
            }
            else if((*it)==u':'){
                vertices.push_back({.start=&(*it), .token=COLON});
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                depth++;
            }
            else if(Semi(it)){
                vertices.push_back({.start=&(*temp), .stop=&(*it), .token=SEMI});
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                depth=1;
            }
            else if((*it)==u'('){
                vertices.push_back({.start=&(*it), .token=LPAREN});
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                depth++;
            }
            else if((*it)==u')'){
                vertices.push_back({.start=&(*it), .token=RPAREN});
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                if(depth>0)depth--;
                //depth++;
            }
            else if((*it)==u'['){
                vertices.push_back({.start=&(*it), .token=LBRACK});
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                depth++;
                temp=it;
                while(it!=utf16.end() && (*it)!=u']'){
                     if((*std::next(it))==u']' && (*it)==u'\\' && (*std::prev(it))!=u'\\'){it++;it++;}
                    else it++;
                }
                vertices.push_back({.start=&(*temp), .stop=&(*it), .token=ARGUMENT});
                edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                vertices.push_back({.start=&(*it), .token=RBRACK});
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                if(depth>0)depth--;
                //depth++;
            }
            else if((*it)==u'{'){
                vertices.push_back({.start=&(*it), .token=LBRACE});
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                depth++;
                while(it!=utf16.end() && (*it)!=u'}'){
                    it++;
                }
                vertices.push_back({.start=&(*temp), .stop=&(*it), .token=ARGUMENT});
                edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                vertices.push_back({.start=&(*it), .token=RBRACE});
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                if(depth>0)depth--;
            }
            else if((*it)==u'|'){
                vertices.push_back({.start=&(*it), .token=PIPE});
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth++;
            }
            else if(Star(it)){
                vertices.push_back({.start=&(*temp), .stop=&(*it), .token=STAR});
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth++;
            }
            else if(Plus(it)){
                vertices.push_back({.start=&(*temp), .stop=&(*it), .token=PLUS});
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth++;
            }
            else if(Dot(it)){
                vertices.push_back({.start=&(*temp), .stop=&(*it), .token=DOT});
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth++;
            }
            else if(Question(it)){
                vertices.push_back({.start=&(*temp), .stop=&(*it), .token=QUESTION});
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth++;
            }
            else if(Not(it)){
                vertices.push_back({.start=&(*temp), .stop=&(*it), .token=NOT});
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth++;
            }
            else if((*it)==u'\\' && (*(it+1))!=u'\\'){
                vertices.push_back({.start=&(*it), .token=ESCSEQ});
                ++it;
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth++;
            }
            else if((*it)==u'\\' && (*(it+1))==u'\\'){
                vertices.push_back({.start=&(*it), .token=ESC});
                ++it;
                ++it;
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth++;
            }
            else if(std::u16ncmp(&(*it), u"..", 2)==0){
                vertices.push_back({.start=&(*it), .token=RANGE});
                std::advance(it, 2);
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                bool hit=false;
                size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                //depth++;
            }
            else if(std::u16ncmp(&(*it), u"->", 2)==0){
                vertices.push_back({.start=&(*it), .token=RARROW});
                std::advance(it, 2);
                vertices.back().stop=&(*it);
                if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                depthProfile[depth].push_back(vertices.size()-1);
                edges.push_back(std::make_tuple(vertices.size()-2, vertices.size()-1, 1));
                //depth++;
            }
            else if([&]()->bool{
                if(NameStartChar(it)){
                    vertices.push_back({.start=&(*it), .token=ID});
                    do{
                    ++it;                    
                    }while(NameChar(it));
                    vertices.back().stop=&(*it);
                    std::u16string label(vertices.back().start, vertices.back().stop);
                    //if(std::islower(label.at(0)))
                    //        std::cout<<"ID size: "<<(vertices.back().stop-vertices.back().start)<<" "<<cv.to_bytes(label)<<std::endl;
                    if(depth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                    depthProfile[depth].push_back(vertices.size()-1);
                    if(depth==0)edges.push_back(std::make_tuple(0, vertices.size()-1, 1));
                    else{
                        bool hit=false;
                        size_t parentIndex=bisect(depth-1, vertices.size()-1, hit);
                        if(hit)edges.push_back(std::make_tuple(parentIndex, vertices.size()-1, 1));
                    }
                    //depth++;
                    return true;
                }
                return false;
            }()){

            }
            else ++it;
            temp=it;
            count++;
            }
            std::sort(edges.begin(), edges.end(), [](std::tuple<graph::vertex_id_t<G>, graph::vertex_id_t<G>, int>& a, std::tuple<graph::vertex_id_t<G>, graph::vertex_id_t<G>, int>& b){return std::get<0>(a)<std::get<0>(b) || std::get<1>(a)<std::get<1>(b);});
            using value = std::ranges::range_value_t<decltype(edges)>;
            graph::vertex_id_t<G> N = static_cast<graph::vertex_id_t<G>>(size(graph::vertices(dagGraph)));
            using edge_desc  = graph::edge_info<graph::vertex_id_t<G>, true, void, int>;
            dagGraph.reserve_vertices(vertices.size());
            dagGraph.reserve_edges(edges.size());
            dagGraph.load_edges(edges, [](const value& val) -> edge_desc {
//                    std::cout<<"edge "<<std::get<0>(val)<<" "<<std::get<1>(val)<<" "<<std::get<2>(val)<<std::endl;
                return edge_desc{std::get<0>(val), std::get<1>(val), std::get<2>(val)};
              }, N);
            dagGraph.load_vertices(vertices, [&](ast_node& nm) {
                auto uid = static_cast<graph::vertex_id_t<G>>(&nm - vertices.data());
//                std::cout<<"vertex "<<uid<<std::endl;
                return graph::copyable_vertex_t<graph::vertex_id_t<G>, ast_node>{uid, nm};
              });
              apply(utf16, options, dagGraph);

        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void G4Reader::display(){
        //std::string depthText=fmt::format("{}\n", depthList);
        //std::cout<<depthText;
        for(std::vector<std::vector<size_t>>::iterator it=depthProfile.begin();it!=depthProfile.end();it++){
            std::cout<<(std::distance(depthProfile.begin(), it))<<std::endl;
            std::vector<size_t>& dp=(*it);
            std::string depthProfile=fmt::format("{}\n", fmt::join(dp, ", "));
            std::cout<<"\t"<<depthProfile;
        }
    }
}
