#ifndef __GAU_MINIMUM_SPANNING_TREE_HPP__
#define __GAU_MINIMUM_SPANNING_TREE_HPP__

#include <unordered_map>
#include <unordered_set>
#include "network_def.hpp"
#include "union_find_def.hpp"


namespace minimum_spanning_tree {

network prim(network& net, network::node_conn_map::iterator entrace_it) {
    network mst; // mst.connmap: from -> (to, info)
    if (entrace_it == net.connmap.end()) return mst;
    // candidates: to -> (from, info)
    std::unordered_map<size_t, std::pair<size_t, void*>> candidates;
    {
        const auto& [first_begin, first_end_set] = *entrace_it;
        mst.connmap[first_begin];
        for (const auto& [first_end, first_info] : first_end_set) {
            auto& ref = candidates[first_end];
            ref.first = first_begin;
            ref.second = first_info;
        }
    }
    while (!candidates.empty()) {
        using candidates_t = decltype(candidates);
        auto min_candidate_it = std::min_element(candidates.begin(), candidates.end(),
            [](const candidates_t::value_type& a, const candidates_t::value_type& b) -> bool {
                return reinterpret_cast<ssize_t>(a.second.second)
                     < reinterpret_cast<ssize_t>(b.second.second);
            }
        );
        mst.insert_conn(
            min_candidate_it->second.first,
            min_candidate_it->first,
            min_candidate_it->second.second
        );
        candidates.erase(min_candidate_it);
        for (const auto& [start, _] : mst.connmap) {
            for (const auto& [end, info] : net.connmap[start]) {
                if (mst.connmap.contains(end)) continue;
                auto [cand_it, cand_new] = candidates.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(end),
                    std::forward_as_tuple()
                );
                ssize_t conn_dist = reinterpret_cast<ssize_t>(info);
                ssize_t cand_dist = reinterpret_cast<ssize_t>(cand_it->second.second);
                if (cand_new || cand_dist > conn_dist) {
                    // reset candidate
                    cand_it->second.first = start;
                    cand_it->second.second = info;
                }
            }
        }
    }
    return mst;
}

// 这个重载会自动选择net的第一个迭代器作为入口
// 连通图用这个就行
network prim(network& net) {
    return prim(net, net.connmap.begin());
}

// 这个重载会在entrance不存在于net中时插入
// 传入entrace是为了应对非连通图
// 对于非连通图，返回值为entrance所在连通部分的MST
network prim(network& net, size_t entrance) {
    return prim(net, net.connmap.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(entrance),
        std::forward_as_tuple()
    ).first);
}

// 返回MST和结果是否有效
std::pair<network, bool> kruskal(network& net) {
    std::pair<network, bool> result;
    network& mst = result.first;
    union_find_squashpath uf;
    struct full_connection {
        size_t from;
        size_t to;
        void* info;
    };
    std::vector<full_connection> conns;
    for (const auto& [start, end_set] : net.connmap) {
        uf.add_node(start);
        for (const auto& [end, info] : end_set) {
            auto& cur_conn = conns.emplace_back();
            cur_conn.from = start;
            cur_conn.to   = end;
            cur_conn.info = info;
        }
    }
    std::sort(conns.begin(), conns.end(),
        [](const full_connection& a, const full_connection& b) -> bool {
            return reinterpret_cast<ssize_t>(a.info)
                 < reinterpret_cast<ssize_t>(b.info);
        }
    );
    // 对于树，边数 = 顶点数 - 1
    size_t inserted_conns = 0, expected_conns = net.connmap.size() - 1;
    for (const auto& [start, end, info] : conns) {
        if (!uf.merge(start, end)) continue;
        mst.insert_conn(start, end, info);
        inserted_conns++;
        if (inserted_conns == expected_conns) {result.second = true; return result;}
    }
    result.second = false; return result;
}

} // namespace minimum_spanning_tree

#endif // __GAU_MINIMUM_SPANNING_TREE_HPP__
