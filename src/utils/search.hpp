#ifndef __GAU_SEARCH_HPP__
#define __GAU_SEARCH_HPP__

#include <vector>
#include <unordered_set>
#include "network_def.hpp"


namespace search {

// 若observer返回true，立刻结束遍历
// using Ob = bool (size_t, std::vector<size_t>);
template <typename Ob>
void dfs_iter(network& net, size_t entrance, Ob observer) {
    using dest_set_it_pair = std::pair<network::dest_set::iterator, network::dest_set::iterator>;
    std::vector<dest_set_it_pair> iter_stack{std::pair(net.connmap[entrance].begin(), net.connmap[entrance].end())};
    std::vector<size_t> history{entrance};
    std::unordered_set<size_t> visited{entrance};
    if (observer(entrance, history)) return;
    while (!iter_stack.empty()) {
        auto& last_iter_ref = iter_stack.back();
        if (last_iter_ref.first == last_iter_ref.second) {
            iter_stack.pop_back();
            history.pop_back();
        } else {
            size_t n = last_iter_ref.first->first;
            ++last_iter_ref.first;
            if (visited.insert(n).second) {
                // observe when not visited
                if (observer(n, history)) return;
                iter_stack.push_back(std::pair(net.connmap[n].begin(), net.connmap[n].end()));
                history.push_back(n);
            }
        }
    }  
}

// using Ob = bool (size_t, size_t);
template <typename Ob>
void bfs_iter(network& net, size_t entrance, Ob observer) {
    std::vector<network::dest_set*> avail_dests{&net.connmap[entrance]};
    std::unordered_set<size_t> visited{entrance};
    size_t n_layer = 0;
    if (observer(entrance, n_layer)) return;
    while (!avail_dests.empty()) {
        n_layer++;
        decltype(avail_dests) next_dests;
        for (const auto& dest : avail_dests) {
            for (auto& [n, _] : *dest) {
                if (visited.insert(n).second) {
                    // observe when not visited
                    if (observer(n, n_layer)) return;
                    next_dests.push_back(&net.connmap[n]);
                }
            }
        }
        avail_dests = std::move(next_dests);
    }
}

} // namespace search

#endif // __GAU_SEARCH_HPP__
