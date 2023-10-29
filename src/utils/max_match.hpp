#ifndef __GAU_MAX_MATCH_HPP__
#define __GAU_MAX_MATCH_HPP__

#include <vector>
#include <unordered_set>
#include "network_def.hpp"
#include "search.hpp"


namespace max_match {

using node_set = std::unordered_set<size_t>;

// 用染色法拆分二部图
auto split_bigraph(network net) -> std::pair<node_set, node_set> {
    if (net.connmap.empty()) throw std::invalid_argument("empty graph");
    std::pair<node_set, node_set> result;
    auto& black_painted = result.first;
    auto& white_painted = result.second;
    // dfs
    using dest_set_it_pair = std::pair<network::dest_set::iterator, network::dest_set::iterator>;
    auto entrance_kv_it = net.connmap.begin();
    std::vector<dest_set_it_pair> iter_stack{std::pair(entrance_kv_it->second.begin(), entrance_kv_it->second.end())};
    black_painted.insert(entrance_kv_it->first);
    while (!iter_stack.empty()) {
        auto& last_iter_ref = iter_stack.back();
        if (last_iter_ref.first == last_iter_ref.second) {
            iter_stack.pop_back();
        } else {
            size_t dest = last_iter_ref.first->first;
            ++last_iter_ref.first;
            auto& should_put     = iter_stack.size() % 2 ? white_painted : black_painted;
            auto& should_not_put = iter_stack.size() % 2 ? black_painted : white_painted;
            if (should_not_put.count(dest)) throw std::invalid_argument("not a bigraph");
            if (should_put.insert(dest).second) {
                // observe when not visited
                iter_stack.push_back(std::pair(net.connmap[dest].begin(), net.connmap[dest].end()));
            }
        }
    }
    return result;
}

using __path_pointer = std::pair<network::node_conn_map::iterator, network::dest_set::iterator>;
using __extendable_path_info = std::vector<__path_pointer>;

// 注：对于正确输入的无向图，并不会抛出std::out_of_range
auto __find_extendable_path(
    network& unmarked, network& marked, size_t entrance
) -> __extendable_path_info { // dfs
    __extendable_path_info result;
    {
        auto first_dests = unmarked.connmap.find(entrance);
        if (first_dests != unmarked.connmap.end())
            result.emplace_back(first_dests, first_dests->second.begin());
    }
    std::unordered_set<size_t> visited{entrance};
    while (!result.empty()) {
        auto& last_iter_ref = result.back();
        if (last_iter_ref.second == last_iter_ref.first->second.end()) {
            result.pop_back();
            // 路径长度为奇数，找到增广路，结束查找
            if (result.size() % 2) break;
        } else {
            size_t last_end = last_iter_ref.second->first;
            ++last_iter_ref.second;
            if (visited.insert(last_end).second) {
                // 如果当前路径长度为奇数，从已标记图中查找，否则从未标记中查找
                network& find_from = result.size() % 2 ? marked : unmarked;
                auto next_dests = find_from.connmap.find(last_end);
                if (next_dests != find_from.connmap.end())
                    result.emplace_back(next_dests, next_dests->second.begin());
            }
        }
    }
    return result; // 注：结果是单向的
}

// 注：为了避免重复访问，通过将每对路径指向其两边来翻转路径
void __flip_extendable_path(
    network& unmarked, network& marked, __extendable_path_info info
) {
    for (size_t i = 0; i < info.size(); i++) {
        auto& cur_info = info[i];
        size_t conn_start = cur_info.first->first;
        size_t conn_end   = cur_info.second->first;
        network& net_from = i % 2 ? marked : unmarked;
        auto bwd_info = net_from.find_conn(conn_start, conn_end);

        auto fwd_dest = cur_info.second;
        if (i != 0) {
            size_t last_start = info[i - 1].first->first;
            fwd_dest->first = last_start;
        } else { // first
            auto& fwd_list = cur_info.first->second;
            auto& to = marked.connmap.at(conn_start);
            to.splice(to.end(), fwd_list, fwd_dest);
        }
        
        auto bwd_dest = bwd_info.second;
        if (i != info.size() - 1) {
            size_t next_end   = info[i + 1].second->first;
            bwd_dest->first = next_end;
        } else { // last
            auto& bwd_list = *bwd_info.first;
            auto& to = marked.connmap.at(conn_end);
            to.splice(to.end(), bwd_list, bwd_dest);
        }
    }
}

// 已知其中一个点集的情况下使用KM算法
template <typename InputIt>
network kuhn_munkres(network& net, InputIt one_part_begin, InputIt one_part_end) {
    network input = net;
    network result;
    for (InputIt it = one_part_begin; it != one_part_end; ++it) {
        auto ext = __find_extendable_path(input, result, *it);
        if (!ext.empty()) __flip_extendable_path(input, result, ext);
    }
    return result;
}

// 未知点集时使用KM算法，图不是二部图时抛出std::invalid_argument
network kuhn_munkres(network& net) {
    auto bisplit = split_bigraph(net);
    return kuhn_munkres(net, bisplit.first.begin(), bisplit.first.end()); 
}

} // namespace max_match

#endif // __GAU_MAX_MATCH_HPP__
