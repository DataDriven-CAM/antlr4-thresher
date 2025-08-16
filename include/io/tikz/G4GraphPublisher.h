#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <cwchar>
#include <typeinfo>

#include "graph/views/depth_first_search.hpp"
#include "graph/views/vertexlist.hpp"

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace sylanmats::io::tikz{

    template<typename PG>
    class G4GraphPublisher{
    protected:
        std::string graphTemplate{};
    public:
        G4GraphPublisher(){
            std::string templateLocation=(getenv("ANTLR4_DB_LOCATION")!=nullptr) ? std::string(getenv("ANTLR4_DB_LOCATION"))+"/tikz": "./templates/tikz";
            std::filesystem::path path=templateLocation+"/tikz.txt";
            std::ifstream file(path);
            graphTemplate=std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        };
        G4GraphPublisher(const G4GraphPublisher& orig) = delete;
        virtual ~G4GraphPublisher() = default;
    
        std::string operator ()(std::u16string& utf16, PG& dagGraph){
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
            std::string tree;
            std::vector<int> visited(graph::num_vertices(dagGraph), 0);
            auto vlist = graph::views::vertexlist(dagGraph);
            for (auto&& [uid, u] : graph::views::vertexlist(dagGraph)) {
                if (visited[uid]>0 || size(graph::edges(dagGraph, uid))==0) {
                  continue;
                }
                visited[uid]++;
                auto uValue=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, uid));
                if(uValue.token_start==sylvanmats::antlr4::parse::SEMI)continue;
                std::u16string label{};
                label.assign(uValue.start, uValue.stop);
                //if(!label.empty())std::cout<<label.size()<<" label "<<cv.to_bytes(label)<<"|"<<size(graph::edges(dagGraph, uid))<<std::endl;
                //if(label.empty())std::cout<<label.size()<<" label root "<<"|"<<std::endl;
                (!label.empty()) ? tree.append(cv.to_bytes(label)) : tree.append("root");
                //tree.append(std::to_string(uValue.obj_size));
                if(size(graph::edges(dagGraph, uid))>1)tree.append("{");
                auto dfs=graph::views::sourced_edges_depth_first_search(dagGraph, uid);
                size_t depth=dfs.depth();
                size_t notchDepth=depth;
                for (auto&& [vid, wid, wv] : dfs) {
                  auto ev=edge_value(dagGraph, wv);
                  //std::cout<<typeid(wv).name()<<" "<<depth<<" "<<dfs.depth()<<" "<<size(graph::edges(dagGraph, vid))<<" "<<visited[vid]<<" "<<size(graph::edges(dagGraph, wid))<<" "<<visited[wid]<<std::endl;
                  //if (!visited[ev]) {
                  if(depth>dfs.depth()){
                    if(!tree.empty() && size(graph::edges(dagGraph, vid))>1 && size(graph::edges(dagGraph, vid))==visited[vid])tree.append("}");
                    if(!tree.empty())tree.append(";\n");
                    auto vValue=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, vid));
                    label.assign(vValue.start, vValue.stop);
                    //if(!label.empty())std::cout<<"\tdepth>label "<<cv.to_bytes(label)<<std::endl;
                    if(!label.empty())tree.append(cv.to_bytes(label));
                  }
                    auto& wValue=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, wid));
                    tree.append(" -> ");
                    if(depth<dfs.depth() && size(graph::edges(dagGraph, vid))>1 && visited[vid]==0)tree.append("{");
                    tree.append("\"");
                    if(wValue.token_start==sylvanmats::antlr4::parse::SEMI){
                         label.assign(wValue.start, wValue.stop);
                        std::string pairKeyName=cv.to_bytes(label);
                //std::cout<<"semi "<<pairKeyName<<std::endl;
                        size_t pos=std::string::npos;
                        while ((pos = pairKeyName.find("_")) != std::string::npos) {
                            pairKeyName.replace(pos, 1, R"(-)");
                        }
                        tree.append(pairKeyName);
                    }
                    label.assign(wValue.start, wValue.stop);
                    tree.append(cv.to_bytes(label));
                    tree.append("\"");
                    if(size(graph::edges(dagGraph, wid))==0)tree.append(" [mark]}");
                    visited[vid]++;
                  //}
                  if(depth>dfs.depth())notchDepth=dfs.depth();
                  depth=dfs.depth();
                }
                    if(size(graph::edges(dagGraph, uid))>1)tree.append("};\n");
            }
            //std::cout<<"tree "<<std::endl;
            auto cArg=fmt::arg("tree", tree);
            //auto dArg=fmt::arg("leafs", indices);
            //std::cout<<"render "<<graphTemplate<<std::endl;
            std::string ret=render(graphTemplate, fmt::make_format_args(cArg));
            return ret;
        };

        std::string render(fmt::basic_string_view<std::string::value_type> users_fmt, fmt::format_args&& args){
           return fmt::vformat(users_fmt, args);
        };
    };
}

