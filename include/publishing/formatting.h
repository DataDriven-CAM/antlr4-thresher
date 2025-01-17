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

template <>
struct std::formatter<std::vector<std::string>>{
    template<class format_parse_context>
    constexpr format_parse_context::iterator parse(format_parse_context& ctx) {
        return ctx.begin();
    }
    
    template<class format_context>
     format_context::iterator format(const std::vector<std::string>& v, format_context& ctx) const {
         //std::cout<<"tokens "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            std::string iA=v[i];
            if(i<v.size()-1)
                std::vformat_to(ctx.out(), "\t\t{},\n", std::make_format_args(iA));
            else
                std::vformat_to(ctx.out(), "\t\t{}\n", std::make_format_args(iA));
        }
        constexpr typename std::string::value_type* fmt={"\n"};
        return std::format_to(ctx.out(), fmt);
    }
};


template <>
struct std::formatter<std::vector<std::wstring>>{
    template<class wformat_parse_context>
    constexpr wformat_parse_context::iterator parse(wformat_parse_context& ctx) {
        return ctx.begin();
    }
    
    template<class wformat_context>
     wformat_context::iterator format(const std::vector<std::wstring>& v, wformat_context& ctx) const {
         //std::cout<<"tokens "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            std::wstring iA=v[i];
            if(i<v.size()-1)
                std::vformat_to(ctx.out(), L"\t\t{},\n", std::make_wformat_args(iA));
            else
                std::vformat_to(ctx.out(), L"\t\t{}\n", std::make_wformat_args(iA));
        }
        constexpr typename std::wstring::value_type* fmt={L"\n"};
        return std::format_to(ctx.out(), fmt);
    }
};


