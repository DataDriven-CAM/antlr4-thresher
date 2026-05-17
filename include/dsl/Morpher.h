#pragma once

#include <codecvt>
#include <cstring>
#include <cwchar>
#include <string>
#include <functional>
#include <filesystem>
#include <tuple>

#include "parse/dag_graph.h"
#include "publishing/CodeGenerator.h"
#include "dsl/Exception.h"

namespace sylvanmats::dsl{
    class Morpher{
    protected:
        sylvanmats::publishing::CodeGenerator<std::string>& codeGenerator;
        std::filesystem::path directory="./";
        std::u16string lexerInstance{};
        std::u16string tokenPrefix=u"LEXER_";
        std::u16string whileInc{};
        std::u16string tempInc=u"temp++";
        std::u16string orReset=u"";
        sylvanmats::antlr4::parse::TOKEN tokens=sylvanmats::antlr4::parse::TOKEN::ROOT;

    public:
        Morpher() = delete;
        Morpher(std::filesystem::path& directory, sylvanmats::publishing::CodeGenerator<std::string>& codeGenerator);
        Morpher(const Morpher& orig) = delete;
        virtual ~Morpher() = default;
        
        void operator()(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph);

    private:
        bool skip=false;
        std::vector<std::u16string> currentMode;
        void operator()(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph, graph::vertex_id_t<sylvanmats::antlr4::parse::G> vid);
        bool recurseLexerRule(std::u16string& g4Buffer, sylvanmats::antlr4::parse::G& dagGraph, graph::vertex_id_t<sylvanmats::antlr4::parse::G> sourceid, std::vector<std::u16string>& expr);
    };

        template <typename T, typename E>
     std::vector<std::string_view> inject_graph(const T& obj, const E& enumeration) {
        std::cout << "injecting graph..." << std::endl;

        std::vector<std::string_view> enumList; 
        template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^E))) {
            // 3. Compare the runtime value with the reflected enumerator value
            //if (value == [:e:]) { 
            // 4. Extract the name as a string_view
            enumList.push_back(std::meta::identifier_of(e));
            //}
        }
        return enumList;
        // for (const auto& value : enumList) {
        //     std::cout << "Enum Value: " << value << std::endl;
        // }
        // template for (constexpr auto base : std::define_static_array(std::meta::bases_of(^^T, std::meta::access_context::unchecked()))) {
        //     // Use type_of(base) to get the type reflection from the base reflection
        //     constexpr auto base_type = std::meta::type_of(base);
        //     if constexpr (std::meta::is_type(base_type) && std::is_class_v<typename [: base_type :]>) {
        //     std::cout<<" is type? "<<std::boolalpha<<std::meta::is_type(base_type)<<" is class? "<<std::is_class_v<typename [: base_type :]><<" "<<std::define_static_array(std::meta::nonstatic_data_members_of(base_type, std::meta::access_context::unchecked())).size()<<std::endl;
        //     template for (constexpr auto m : std::define_static_array(std::meta::nonstatic_data_members_of(base_type, std::meta::access_context::unchecked()))) {
        //         std::cout<< "Field: " << std::meta::identifier_of(m) << " = ";
        //         if constexpr (std::is_class_v<typename [: std::meta::type_of(m) :]> ) {
        //         constexpr auto m_type = std::meta::type_of(m);
        //             constexpr auto name = std::meta::display_string_of(m_type);
        //             if constexpr (name.starts_with("std::vector")){
        //                 std::cout << " (vector " << name << "): " << std::endl;
        //             }
        //         }
        //         else {
        //             std::cout << obj.[:m:] << std::endl;
        //         }
        //     }
        // }
        // }
    };
}
