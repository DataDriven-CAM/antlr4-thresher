#pragma once

#include <tuple>
#include <cwchar>
#include <string>

namespace sylvanmats::antlr4::parse {
    class LineColumnFinder{
        public:
        LineColumnFinder()=default;
        LineColumnFinder(const LineColumnFinder& orig)=delete;
        virtual ~LineColumnFinder()=default;

        std::tuple<size_t, size_t> operator()(std::u16string& g4Buffer, const char16_t* offset){
            size_t line=1;
            size_t column=1;
            std::u16string::const_iterator it = g4Buffer.begin();
            char16_t* lineStart=(char16_t*)&(*it);
            while(it!=g4Buffer.end() && &(*it)<offset){
                if((*it)==u'\n'){
                    line++;
                    lineStart=(char16_t*)&(*it);
                }
                it++;
            }
            return std::make_tuple(line, offset-lineStart);

        };

    };
}