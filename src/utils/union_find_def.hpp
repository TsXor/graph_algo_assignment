#ifndef __GAU_UNION_FIND_DEF_HPP__
#define __GAU_UNION_FIND_DEF_HPP__

#include <cstdint>
#include <unordered_map>

struct union_find_squashpath {
    std::unordered_map<size_t, size_t> parent_map;
    union_find_squashpath() = default;
    template <typename InputIt, typename Extractor>
    union_find_squashpath(InputIt begin_it, InputIt end_it, Extractor extractor) {
        for (auto it = begin_it; it != end_it; ++it) {
            size_t x = extractor(*it);
            auto& entry = parent_map[x];
            entry = x;
        }
    }
    void add_node(size_t n) {
        auto& entry = parent_map[n];
        entry = n;
    }
    size_t find_root(size_t x) {
        auto& parent = parent_map[x];
        return parent == x ? x : (parent = find_root(parent));
    }
    bool merge(size_t m, size_t n) {
        m = find_root(m), n = find_root(n);
        if (m == n) return false;
        parent_map[m] = n;
        return true;
    }
};

struct union_find_depthcount {
    std::unordered_map<size_t, std::pair<size_t, size_t>> parent_map;
    union_find_depthcount() = default;
    template <typename InputIt, typename Extractor>
    union_find_depthcount(InputIt begin_it, InputIt end_it, Extractor extractor) {
        for (auto it = begin_it; it != end_it; ++it) {
            size_t x = extractor(*it);
            auto& entry = parent_map[x];
            entry.first = x;
            entry.second = 1;
        }
    }
    void add_node(size_t n) {
        auto& entry = parent_map[n];
        entry.first = n;
        entry.second = 1;
    }
    size_t find_root(size_t x) {
        auto& [parent, _] = parent_map[x];
        return parent == x ? x : find_root(parent);
    }
    bool merge(size_t m, size_t n) {
        m = find_root(m), n = find_root(n);
        if (m == n) return false;
        auto& [m_parent, m_treesize] = parent_map[m];
        auto& [n_parent, n_treesize] = parent_map[n];
        if (m_treesize < n_treesize) {
            n_parent = m;
            m_treesize += n_treesize;
        } else {
            m_parent = n;
            n_treesize += m_treesize;
        }
        return true;
    }
};

#endif // __GAU_UNION_FIND_DEF_HPP__
