#ifndef __GAU_SHORTEST_PATH_HPP__
#define __GAU_SHORTEST_PATH_HPP__

#include <unordered_map>
#include "network_def.hpp"

namespace shortest_path {

struct path {
    std::vector<std::pair<size_t, void*>> conns;
    ssize_t distance;
};

// 返回到所有节点的最低权路径以及是否有负权环
auto bellman_ford(network& net, size_t entrance) -> std::pair<std::unordered_map<size_t, path>, bool> {
    std::pair<std::unordered_map<size_t, path>, bool> result;
    auto& paths = result.first;
    paths[entrance].distance = 0; // entrance have empty path and 0 distance

    auto do_shorten = [&]() -> bool {
        bool modified = false;
        for (const auto& [start, connset] : net.connmap) {
            auto path_to_start_it = paths.find(start);
            if (path_to_start_it == paths.end()) continue;
            for (const auto& conn : connset) {
                // try to insert
                auto [path_to_end_it, end_new] = paths.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(conn.first),
                    std::forward_as_tuple()
                );
                ssize_t conn_dist = reinterpret_cast<ssize_t>(conn.second);
                if (end_new || path_to_end_it->second.distance > path_to_start_it->second.distance + conn_dist) {
                    // reset end to start + conn
                    modified = true;
                    (path_to_end_it->second.conns = path_to_start_it->second.conns).push_back(conn);
                    path_to_end_it->second.distance = path_to_start_it->second.distance + conn_dist;
                }
            }
        }
        return modified;
    };
    
    result.second = false;
    for (size_t i = 0; i < net.connmap.size() - 1; i++) {
        if (!do_shorten()) return result;
    }
    // 如果减短成功，那么存在负权环
    result.second = do_shorten();
    return result;
}

auto dijkstra(network& net, size_t entrance) -> std::unordered_map<size_t, path> {
    using pathmap_t = std::unordered_map<size_t, path>;
    pathmap_t paths, candidates;
    paths[entrance].distance = 0; // entrance have empty path and 0 distance
    for (const auto& conn : net.connmap[entrance]) {
        auto& dst_ref = candidates[conn.first];
        dst_ref.conns.push_back(conn);
        dst_ref.distance = reinterpret_cast<ssize_t>(conn.second);
    }
    while (!candidates.empty()) {
        auto min_candidate_it = std::min_element(candidates.begin(), candidates.end(),
            [](const pathmap_t::value_type& a, const pathmap_t::value_type& b) -> bool {
                return a.second.distance < b.second.distance;
            }
        );
        min_candidate_it = paths.insert(candidates.extract(min_candidate_it)).position;
        for (const auto& conn : net.connmap[min_candidate_it->first]) {
            if (paths.contains(conn.first)) continue;
            // try to insert
            auto [dst_it, dst_new] = candidates.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(conn.first),
                std::forward_as_tuple()
            );
            ssize_t conn_dist = reinterpret_cast<ssize_t>(conn.second);
            if (dst_new || dst_it->second.distance > min_candidate_it->second.distance + conn_dist) {
                // reset end to start + conn
                (dst_it->second.conns = min_candidate_it->second.conns).push_back(conn);
                dst_it->second.distance = min_candidate_it->second.distance + conn_dist;
            }
        }
    }
    return paths;
}

} // namespace shortest_path

#endif // __GAU_SHORTEST_PATH_HPP__
