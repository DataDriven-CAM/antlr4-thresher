#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <locale>
#include <codecvt>
#include <format>

#define protected public
#include "publishing/formatting.h"

TEST_SUITE ("formatting"){
    
TEST_CASE("test streambuf"){
    auto start = std::chrono::high_resolution_clock::now();
        std::filesystem::path path="/home/roger/sylvanmats/cifio/db/components.cif";
        std::ifstream is(path);
        std::string components((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        is.close();
        std::remove("../tmp/components_sb.cif");
        std::remove("../tmp2/components_fmt.cif");
    auto startOutput = std::chrono::high_resolution_clock::now();
        std::ofstream os("../tmp/components_sb.cif");
    std::copy(components.begin(), components.end(), std::ostreambuf_iterator<char>(os));
    os.close();
    auto startFMTOutput = std::chrono::high_resolution_clock::now();
    /*std::FILE* fp=std::fopen( "../tmp2/components_fmt.cif", "w+" );
    fmt::string_view sv(components);
    fmt::detail::print(fp, sv);
    std::fclose(fp);*/
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "streambuf output time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(startFMTOutput-startOutput).count()*1.0e-9 << "s\n";
    std::cout << "fmt output time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-startFMTOutput).count()*1.0e-9 << "s\n";
    std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1.0e-9 << "s\n";

}

TEST_CASE("test std vector formatting"){
    std::string hw=std::format("{} {}!", "Hello", "world", "something");
    CHECK_EQ(hw, "Hello world!");
    std::vector<int> v = {1, 2, 3};
    //std::wstring results=std::format(L"{}\n", v);
    //CHECK_EQ(results, L"[1, 2, 3]");
}

TEST_CASE("test u16 formatting"){
    /*std::u16string h=u"Hello";
    std::u16string w=u"world";
    std::u16string s=u"something";
    std::u16string fs=u"{} {}!";
    std::u16string hw=std::format(fs, h, w, s);
    CHECK_EQ(hw, u"Hello world!");*/
    std::vector<std::string> tokens={"DOUBLE", "MINUS", "PLUS", "DIGIT"};
    //std::vector<std::wstring> tokens={L"DOUBLE", L"MINUS", L"PLUS", L"DIGIT"};
    //std::arg("tokens", tokens);
    std::string content=std::format(std::runtime_format(" {}\n"), tokens);
    //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;
    std::cout<<"content "<<content<<std::endl;
    //std::cout<<"content "<<cv.to_bytes(content)<<std::endl;
}

}