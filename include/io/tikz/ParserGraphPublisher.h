#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <typeinfo>

#include "graph/views/depth_first_search.hpp"
#include "graph/views/vertexlist.hpp"

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace sylanmats::io::tikz{

    template<typename LG, typename PG>
    class ParserGraphPublisher{
    protected:
        std::string graphTemplate{};
    public:
        ParserGraphPublisher(){
            std::string templateLocation=(getenv("ANTLR4THRESHER_DB_LOCATION")!=nullptr) ? std::string(getenv("ANTLR4THRESHER_DB_LOCATION"))+"/templates/tikz": "./templates/tikz";
            std::filesystem::path path=templateLocation+"/tikz.txt";
            std::ifstream file(path);
            graphTemplate=std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        };
        ParserGraphPublisher(const ParserGraphPublisher& orig) = delete;
        virtual ~ParserGraphPublisher() = default;
    
        std::string operator ()(std::u16string& utf16, LG& ldagGraph, PG& dagGraph){
            std::string tree;
            std::vector<int> visited(graph::num_edges(dagGraph), 0);
            auto vlist = graph::views::vertexlist(dagGraph);
            for (auto&& [uid, u] : graph::views::vertexlist(dagGraph)) {
                if (visited[uid]>0 || size(graph::edges(dagGraph, uid))==0) {
                  continue;
                }
                visited[uid]++;
                auto uValue=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, uid));
                tree.append(std::to_string(uValue.obj_size));
                if(size(graph::edges(dagGraph, uid))>1)tree.append("{");
                auto dfs=graph::views::sourced_edges_depth_first_search(dagGraph, uid);
                size_t depth=dfs.depth();
                size_t notchDepth=depth;
                for (auto&& [vid, wid, wv] : dfs) {
                  //auto ev=edge_value(dagGraph, wv);
                  //std::cout<<typeid(wv).name()<<" "<<depth<<" "<<dfs.depth()<<" "<<size(graph::edges(dagGraph, vid))<<" "<<visited[vid]<<" "<<size(graph::edges(dagGraph, wid))<<" "<<visited[wid]<<std::endl;
                  //if (!visited[ev]) {
                  if(depth>dfs.depth()){
                    if(!tree.empty() && size(graph::edges(dagGraph, vid))>1 && size(graph::edges(dagGraph, vid))==visited[vid])tree.append("}");
                    if(!tree.empty())tree.append(";\n");
                    auto vValue=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, vid));
                    tree.append(std::to_string(vValue.obj_size));
                  }
                    auto& wValue=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, wid));
                    tree.append(" -> ");
                    if(depth<dfs.depth() && size(graph::edges(dagGraph, vid))>1 && visited[vid]==0)tree.append("{");
                    tree.append("\"");
                    if(wValue.parser_token==sylvanmats::antlr4::parse::PAIR_KEY){
                        std::string pairKeyName(substr_view(jsonBinder.jsonContent, wValue.start, wValue.end));
                        size_t pos=std::string::npos;
                        while ((pos = pairKeyName.find("_")) != std::string::npos) {
                            pairKeyName.replace(pos, 1, R"(-)");
                        }
                        tree.append(pairKeyName);
                    }
                    tree.append(std::to_string(wValue.obj_size));
                    tree.append("\"");
                    if(size(graph::edges(dagGraph, wid))==0)tree.append(" [mark]");
                    visited[vid]++;
                  //}
                  if(depth>dfs.depth())notchDepth=dfs.depth();
                  depth=dfs.depth();
                }
                    tree.append("};\n");
            }
            auto cArg=fmt::arg("tree", tree);
            //auto dArg=fmt::arg("leafs", indices);
            std::string ret=fmt::vformat(graphTemplate, fmt::make_format_args(cArg));
            return ret;
        };
    };
}

