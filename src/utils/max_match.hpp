#ifndef __GAU_MAX_MATCH_HPP__
#define __GAU_MAX_MATCH_HPP__

#include <vector>
#include <unordered_set>
#include "network_def.hpp"


namespace max_match {

using node_set = std::unordered_set<size_t>;

// 用染色法拆分二部图
auto split_bigraph(network net) -> std::pair<node_set, node_set> {
    if (net.connmap.empty()) throw std::invalid_argument("empty graph");
    std::pair<node_set, node_set> result;
    auto& black_painted = result.first;
    auto& white_painted = result.second;
    // dfs
    auto entrance_it = net.connmap.begin();
    std::vector<network::conn_it> iter_stack;
    iter_stack.emplace_back(entrance_it);
    black_painted.insert(entrance_it->first);
    while (!iter_stack.empty()) {
        auto& last_iter = iter_stack.back();
        if (!last_iter.more()) {
            iter_stack.pop_back();
        } else {
            size_t dest = last_iter.dest(); last_iter.to_next();
            auto& should_put     = iter_stack.size() % 2 ? white_painted : black_painted;
            auto& should_not_put = iter_stack.size() % 2 ? black_painted : white_painted;
            if (should_not_put.count(dest)) throw std::invalid_argument("not a bigraph");
            if (should_put.insert(dest).second) {
                // observe when not visited
                auto connmap_it = net.connmap.find(dest);
                if (connmap_it != net.connmap.end())
                    iter_stack.emplace_back(connmap_it);
            }
        }
    }
    return result;
}

using __extendable_path_info = std::vector<network::conn_it>;

auto __find_extendable_path(
    network& unmarked, network& marked, size_t entrance
) -> __extendable_path_info { // dfs
    __extendable_path_info result;
    if (marked.connmap.contains(entrance)) return result;
    {
        auto entrance_it = unmarked.connmap.find(entrance);
        if (entrance_it != unmarked.connmap.end())
            result.emplace_back(entrance_it);
    }
    std::unordered_set<size_t> path_as_set{entrance};
    while (!result.empty()) {
        auto& last_iter = result.back();
        if (!last_iter.more()) {
            path_as_set.erase(last_iter.start());
            result.pop_back();
            // 回退时不能触发结束
            // if (result.size() % 2) break;
            if (!result.empty()) result.back().to_next();
        } else {
            size_t last_end = last_iter.dest();
            if (!path_as_set.count(last_end)) {
                // 如果当前路径长度为奇数，从已标记图中查找，否则从未标记中查找
                network& find_from = result.size() % 2 ? marked : unmarked;
                auto connmap_it = find_from.connmap.find(last_end);
                if (connmap_it != find_from.connmap.end() && !connmap_it->second.empty()) {
                    result.emplace_back(connmap_it);
                    path_as_set.insert(last_end);
                } else {
                    // 路径长度为奇数，找到增广路，结束查找
                    if (result.size() % 2) break;
                    last_iter.to_next();
                }
            } else last_iter.to_next();
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
        size_t conn_start = cur_info.start();
        size_t conn_dest  = cur_info.dest();
        network& net_from = i % 2 ? marked : unmarked;
        auto bwd_info = net_from.find_conn(conn_dest, conn_start);

        auto fwd_dest = cur_info.destset_it;
        if (i != 0) {
            fwd_dest->first = info[i - 1].start();
        } else { // first
            auto& fwd_list = cur_info.dests();
            auto& to = marked.connmap[conn_start];
            to.splice(to.end(), fwd_list, fwd_dest);
        }
        
        auto bwd_dest = bwd_info.destset_it;
        if (i != info.size() - 1) {
            bwd_dest->first = info[i + 1].dest();
        } else { // last
            auto& bwd_list = bwd_info.dests();
            auto& to = marked.connmap[conn_dest];
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

network blossom(network& net) {
    network input = net;
    network result;
    while (true) {
        bool have_extendable_path = false;
        for (auto& [vertice, _] : net.connmap) {
            auto ext = __find_extendable_path(input, result, vertice);
            if (!ext.empty()) {
                have_extendable_path = true;
                __flip_extendable_path(input, result, ext);
                break;
            }
        }
        if (!have_extendable_path) break;
    }
    return result;
}

} // namespace max_match

#endif // __GAU_MAX_MATCH_HPP__
