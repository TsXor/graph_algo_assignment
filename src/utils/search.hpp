#ifndef __GAU_SEARCH_HPP__
#define __GAU_SEARCH_HPP__

#include <vector>
#include <unordered_set>
#include "network_def.hpp"


// 若observer返回true，立刻结束遍历
// using Ob = bool (size_t, std::vector<size_t>);
template <typename Ob>
void dfs_iter(network& net, size_t entrance, Ob observer) {
    std::vector<network::node_conn_map::iterator> iter_stack{net.connmap.find(entrance)};
    std::vector<size_t> history{entrance};
    std::unordered_set<size_t> visited{entrance};
    if (observer(entrance, history)) return;
    while (!iter_stack.empty()) {
        auto& last_iter_ref = iter_stack.back();
        if (last_iter_ref == net.connmap.end()) {
            iter_stack.pop_back();
            history.pop_back();
        } else {
            size_t n = network::get_point_to(*last_iter_ref);
            ++last_iter_ref;
            if (visited.insert(n).second) {
                // observe when not visited
                if (observer(n, history)) return;
                iter_stack.push_back(net.connmap.find(n));
                history.push_back(n);
            }
        }
    }  
}

//using Ob = bool (size_t, size_t);
template <typename Ob>
void bfs_iter(network& net, size_t entrance, Ob observer) {
    std::vector<network::node_conn_map::iterator> avail_iters{net.connmap.find(entrance)};
    std::unordered_set<size_t> visited{entrance};
    size_t n_layer = 0;
    if (observer(entrance, n_layer)) return;
    while (!avail_iters.empty()) {
        n_layer++;
        decltype(avail_iters) next_iters;
        for (auto iter : avail_iters) {
            for (; iter != net.connmap.end(); ++iter) {
                size_t n = network::get_point_to(*iter);
                if (visited.insert(n).second) {
                    // observe when not visited
                    if (observer(n, n_layer)) return;
                    next_iters.push_back(net.connmap.find(n));
                }
            }
        }
        avail_iters = std::move(next_iters);
    }
}

#endif // __GAU_SEARCH_HPP__
