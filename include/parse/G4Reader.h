#pragma once


#include <stdlib.h>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <exception>
#include <string>
#include <locale>
#include <codecvt>
#include <cstring>
#include <cwchar>
#include <memory>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <functional>

#include <fcntl.h>
#include <algorithm>
#include <unordered_map>
#include <ranges>

#include "parse/dag_graph.h"

namespace std{
    size_t u16ncmp(const char16_t* a, const char16_t* b, size_t n);

}

namespace sylvanmats::antlr4::parse {

    class G4Reader{
    protected:
        G dagGraph;
        std::vector<ast_node> vertices;
        std::vector<std::tuple<graph::vertex_id_t<G>, graph::vertex_id_t<G>, int>> edges;
        std::vector<std::vector<size_t>> depthProfile;
        
        std::filesystem::path directory="./";
        std::unordered_map<std::u16string, std::u16string> options;
    
    public:
        G4Reader() = default;
        G4Reader(const G4Reader& orig) = delete;
        virtual ~G4Reader() = default;
        void operator()(std::filesystem::path& filePath, std::function<void(std::u16string& utf16, G& dagGraph)> apply);
        void operator()(std::u16string& utf16, std::function<void(std::u16string& utf16, G& dagGraph)> apply);
        void display();

    private:
        char16_t Esc=u'\\';

        std::function<bool(std::u16string::const_iterator&)> Colon = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u':'; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Question = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u'?';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Star = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u'*';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Plus = [](std::u16string::const_iterator& it)->bool{std::u16string::const_iterator temp=it; bool ret=(*temp)==u'+'; if(ret)it++;return ret;};

        std::function<bool(std::u16string::const_iterator&)> PlusAssign = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=std::u16ncmp(&(*temp), u"+=", 2)==0; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Underscore = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u'_';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Pipe = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u'|';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Dollar = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u'$';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Comma = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u',';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Semi = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u';';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Dot = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u'.';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Range = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=std::u16ncmp(&(*temp), u"..", 2)==0; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> At = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u'@';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Pound = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u'#';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Tilde = [](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=(*temp)==u'~';temp++; if(ret)it=temp;return ret;};

        std::function<bool(std::u16string::const_iterator&)> Not = [&](std::u16string::const_iterator& it) {std::u16string::const_iterator temp=it; bool ret=Tilde(temp); if(ret)it=temp;return ret;};

        bool DocComment(TOKEN token, PARSE_MODE mode, std::u16string::const_iterator& it, std::u16string::const_iterator itEnd){
                if(std::u16ncmp(&(*it), u"/**", 3)==0){
                    vertices.push_back({.start=&(*it), .token=token});
                    //lemon::ListGraph::Node n=astGraph.addNode();
                    //astNode[n].start=&(*it);
                    //astNode[n].token=token;
                    //astNode[n].mode=mode;
                    std::advance(it, 3);
                    bool hitStop=false;
                    while(!hitStop && it!=itEnd){
                        if(std::u16ncmp(&(*it), u"*/", 2)==0){
                            std::advance(it, 2);
                            //astNode[n].stop=&(*it);
                            vertices.back().stop=&(*it);
                            hitStop=true;
                        }
                        else ++it;
                    }
                    return true;
                }
                else return false;
        };

        bool BlockComment(std::u16string::const_iterator& it, std::u16string::const_iterator itEnd){
            if(std::u16ncmp(&(*it), u"/*", 2)==0){
                std::advance(it, 2);
                bool hitStop=false;
                while(!hitStop && it!=itEnd){
                    if(std::u16ncmp(&(*it), u"*/", 2)==0){
                        std::advance(it, 2);
                        hitStop=true;
                    }
                    else ++it;
                }
                return true;
            }
            else return false;
        };

        bool LineComment(std::u16string::const_iterator& it, std::u16string::const_iterator itEnd){
            if(std::u16ncmp(&(*it), u"//", 2)==0){
                std::advance(it, 2);
                bool hitStop=false;
                while(!hitStop && it!=itEnd){
                    if((*it)==u'\r'|| (*it)==u'\n'){
                        hitStop=true;
                    }
                    else ++it;
                }
                return true;
            }
            else return false;
        };

        inline bool HexDigit(std::u16string::const_iterator it){
            if((*it)>=u'1' && (*it)<=u'9') return true;
            return ((*it)>=u'0' && (*it)<=u'9') || ((*it)>=u'a' && (*it)<=u'f') || ((*it)>=u'A' && (*it)<=u'F');
        };
        bool UnicodeEsc(std::u16string::const_iterator& it){
            if((*it)==u'u' && HexDigit(it+1) && HexDigit(it+2) && HexDigit(it+3) && HexDigit(it+4)){
                std::advance(it, 5);
                return true;
            }
            return false;
        };

        std::u16string::const_iterator itEnd;
        bool EndOfFile(std::u16string::const_iterator& it){
            return it==itEnd;
        };

        bool EscSeq(std::u16string::const_iterator& it){
            if((*it)==Esc){
                ++it;
                std::u16string v(u"btnfr\"'\\");
                if(std::any_of(v.begin(), v.end(), [&it](char16_t d){return ((*it)==d);})){
                    ++it;
                    return true;
                }
                else if(UnicodeEsc(it))return true;
            }
            return false;
        };

        bool NameStartChar(std::u16string::const_iterator& it){
            return ((*it)>=u'A' && (*it)<=u'Z')
           || ((*it)>=u'a' && (*it)<=u'z')
           || ((*it)>=u'\u00C0' && (*it)<=u'\u00D6')
           || ((*it)>=u'\u00D8' && (*it)<=u'\u00F6')
           || ((*it)>=u'\u00F8' && (*it)<=u'\u02FF')
           || ((*it)>=u'\u0370' && (*it)<=u'\u037D')
           || ((*it)>=u'\u037F' && (*it)<=u'\u1FFF')
           || ((*it)>=u'\u200C' && (*it)<=u'\u200D')
           || ((*it)>=u'\u2070' && (*it)<=u'\u218F')
           || ((*it)>=u'\u2C00' && (*it)<=u'\u2FEF')
           || ((*it)>=u'\u3001' && (*it)<=u'\uD7FF')
           || ((*it)>=u'\uF900' && (*it)<=u'\uFDCF')
           || ((*it)>=u'\uFDF0' && (*it)<=u'\uFFFD');
        };

        bool NameChar(std::u16string::const_iterator& it){
            return NameStartChar(it)|| ((*it)>=u'/' && (*it)<=u'9')
                                || (*it)==u'_'
                                || (*it)==u'\u00B7'
                                || ((*it)>=u'\u0300' && (*it)<=u'\u036F')
                                || ((*it)>=u'\u203F' && (*it)<=u'\u2040');
        };

        size_t bisect(size_t currentDepth, size_t target, bool& hit){
            std::vector<size_t>& depthVector=depthProfile[currentDepth];
            int low = 0;
            int high = depthVector.size() - 1;

            if (target < depthVector[low]) {
                return 0; // Target is below the range of depthVector
            }

            while (low <= high) {
                int mid = low + (high - low) / 2;

                if (depthVector[mid] < target) {
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }

            // If the target is smaller than all elements, return -1
            if(high >= 0) hit=true;
            return high >= 0 ? depthVector[high] : 0; 
        };
        

    };
}
