#pragma once

#include <deque>
#include <string>
#include <iostream>
#include <filesystem>
#include <cwchar>
#include <typeinfo>

#include "graph/views/dfs.hpp"
#include "graph/views/vertexlist.hpp"

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace sylanmats::io::tikz{

    struct labelled_nodes {
        std::u16string name{};
        std::u16string label{};
    };

    struct transparent_string_hash {
        using is_transparent = void; 

        size_t operator()(std::u16string_view sv) const noexcept {
            return std::hash<std::u16string_view>{}(sv);
        }
        size_t operator()(const std::u16string& str) const noexcept {
            return std::hash<std::u16string>{}(str);
        }
    };

    inline std::u16string makeUnique(std::u16string& originalLabel, std::deque<labelled_nodes>& nodes, std::unordered_map<std::u16string, int, transparent_string_hash, std::equal_to<>>& labelCounts) {
        labelled_nodes& newNode = nodes.emplace_back(labelled_nodes{
            .name = u"", 
            .label = originalLabel//std::move(originalLabel)
        });
        std::u16string_view baseView = newNode.label;
        if (auto it = labelCounts.find(baseView); it != labelCounts.end()) {
            // Duplicate detected: Calculate the next unique suffix ID
            int suffix_id = it->second++;
            
            // Name gets the unique variation; label stays exactly as the original
            std::string suffix_str = std::to_string(suffix_id);
            if(newNode.label.compare(u";")==0)
                newNode.name = u"semi_"+ std::u16string(suffix_str.begin(), suffix_str.end());
            else if(newNode.label.compare(u"{")==0)
                newNode.name = u"lbrace_"+ std::u16string(suffix_str.begin(), suffix_str.end());
            else if(newNode.label.compare(u"}")==0)
                newNode.name = u"rbrace_"+ std::u16string(suffix_str.begin(), suffix_str.end());
            else
               newNode.name = newNode.label +u"_"+ std::u16string(suffix_str.begin(), suffix_str.end());
            
            // Track the new mutated name in the map to prevent downstream collisions
            labelCounts.emplace(newNode.name, 1);
        } else {
            // First time seeing this string; name matches the label perfectly
            if(newNode.label.compare(u";")==0)
                newNode.name = u"semi";
            else if(newNode.label.compare(u"{")==0)
                newNode.name = u"lbrace";
            else if(newNode.label.compare(u"}")==0)
                newNode.name = u"rbrace";
            else
                newNode.name = newNode.label;
            labelCounts.emplace(newNode.label, 1);
        }
        return newNode.name;
    };

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
            std::deque<labelled_nodes> nodes;
            std::unordered_map<std::u16string, int, transparent_string_hash, std::equal_to<>> labelCounts;
            std::string tree;
            std::vector<int> visited(graph::num_vertices(dagGraph), 0);
            auto vlist = graph::views::vertexlist(dagGraph);
            for (auto&& [uid, u] : graph::views::vertexlist(dagGraph)) {
                if (visited[uid]>0 || graph::out_degree(dagGraph, uid)==0) {
                  continue;
                }
                visited[uid]++;
                auto uValue=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, uid));
                if(uValue.token_start==sylvanmats::antlr4::parse::SEMI)continue;
                std::u16string label{};
                label.assign(uValue.start, uValue.stop);
                //auto uniqueLabel = makeUnique(label, nodes, labelCounts);
                //if(!label.empty())std::cout<<label.size()<<" label "<<cv.to_bytes(label)<<"|"<<size(graph::edges(dagGraph, uid))<<std::endl;
                //if(label.empty())std::cout<<label.size()<<" label root "<<"|"<<std::endl;
                (!label.empty()) ? tree.append(cv.to_bytes(label)) : tree.append("root");
                //tree.append(std::to_string(uValue.obj_size));
                if(graph::out_degree(dagGraph, uid)>0)tree.append("-> {");
                auto dfs=graph::views::edges_dfs(dagGraph, uid);
                size_t depth=dfs.depth();
                size_t notchDepth=depth;
                bool first=true;
                for (auto&& [e] : dfs) {
                auto vid = static_cast<size_t>(graph::adj_list::source_id(dagGraph, e));
                auto wid = static_cast<size_t>(graph::adj_list::target_id(dagGraph, e));
                  auto ev=edge_value(dagGraph, e);
                //   std::cout<<typeid(ev).name()<<" "<<depth<<" "<<dfs.depth()<<" "<<size(graph::edges(dagGraph, vid))<<" "<<visited[vid]<<" "<<size(graph::edges(dagGraph, wid))<<" "<<visited[wid]<<std::endl;
                  //if (!visited[ev]) {
                  if(depth>dfs.depth()){
                    if(!tree.empty() && graph::out_degree(dagGraph, vid)>1 && graph::out_degree(dagGraph, vid)==visited[vid])tree.append("}");
                    auto vValue=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, vid));
                    label.assign(vValue.start, vValue.stop);
                    //if(!label.empty())std::cout<<"\tdepth>label "<<cv.to_bytes(label)<<std::endl;
                    if(!label.empty()){
                        std::u16string uniqueLabel = makeUnique(label, nodes, labelCounts);
                        tree.append(cv.to_bytes(uniqueLabel));
                        if(!uniqueLabel.empty() && uniqueLabel.size()!=label.size())tree.append(" as=[" + cv.to_bytes(label) + "]");
                    }
                  }
                    auto& wValue=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, wid));
                    if(!first)tree.append(" -> ");
                    first=false;
                    // if(graph::out_degree(dagGraph, vid)==0 && visited[vid]==0)tree.append("{");
                    // tree.append("\"");
                //     if(wValue.token_start==sylvanmats::antlr4::parse::SEMI){
                //          label.assign(wValue.start, wValue.stop);
                //         std::string pairKeyName=cv.to_bytes(label);
                // std::cout<<"semi "<<pairKeyName<<std::endl;
                //         size_t pos=std::string::npos;
                //         while ((pos = pairKeyName.find("_")) != std::string::npos) {
                //             pairKeyName.replace(pos, 1, R"(-)");
                //         }
                //         tree.append(pairKeyName);
                //     }
                    label.assign(wValue.start, wValue.stop);
                    std::u16string uniqueLabel = makeUnique(label, nodes, labelCounts);
                    if(!uniqueLabel.empty() && uniqueLabel.compare(label)!=0 && graph::out_degree(dagGraph, wid)==0){
                        tree.append("\"");
                        tree.append(cv.to_bytes(uniqueLabel));
                        tree.append("\"");
                        if(label.compare(u"{")==0)tree.append(" [as=\\{, mark]");
                        else if(label.compare(u"}")==0)tree.append(" [as=\\}, mark]");
                        else tree.append(" [as=" + cv.to_bytes(label) + ", mark]");
                    }
                    else if (!uniqueLabel.empty() && uniqueLabel.compare(label)!=0){
                        tree.append("\"");
                        tree.append(cv.to_bytes(uniqueLabel));
                        tree.append("\"");
                        if(label.compare(u"{")==0)tree.append(" [as=\\{]");
                        else if(label.compare(u"}")==0)tree.append(" [as=\\}]");
                        else tree.append(" [as=" + cv.to_bytes(label) + "]");
                    }
                    else if(graph::out_degree(dagGraph, wid)==0){
                        tree.append("\"");
                        tree.append(cv.to_bytes(uniqueLabel));
                        tree.append("\"");
                        tree.append(" [mark]");
                    }
                    else{
                        tree.append("\"");
                        tree.append(cv.to_bytes(uniqueLabel));
                        tree.append("\"");
                    }
                    if(wValue.token_start==sylvanmats::antlr4::parse::SEMI || wValue.token_start==sylvanmats::antlr4::parse::RBRACE || wValue.token_start==sylvanmats::antlr4::parse::OPT_RBRACE){
                        tree.append(",\n");
                        first=true;
                    }
                    visited[vid]++;
                  //}
                  if(depth>dfs.depth())notchDepth=dfs.depth();
                  depth=dfs.depth();
                }
                if (graph::out_degree(dagGraph, uid)>1)tree.append("}\n");
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

