#ifndef __GAU_NETWORK_DEF_HPP__
#define __GAU_NETWORK_DEF_HPP__

#include <cstdint>
#include <list>
#include <unordered_map>
#include <functional>
#include <memory>


struct network {
    // maybe let dest_set = std::unordered_map<size_t, void*>
    using dest_set = std::list<std::pair<size_t, void*>>;
    using node_conn_map = std::unordered_map<size_t, dest_set>;
    node_conn_map connmap;
    
    void insert_conn(size_t from, size_t to, void* info) {
        auto& conn_list_ref = connmap[from];
        conn_list_ref.emplace_back(to, info);
        connmap[to]; // make sure it exist
    }
    
    void erase_conn(size_t from, size_t to) {
        auto conn_list_it = connmap.find(from);
        if (conn_list_it != connmap.end()) {
            conn_list_it->second.remove_if(
                [to](dest_set::value_type v) {return v.first == to;}
            );
        }
    }
    
    dest_set& insert_node(size_t n) {
        return connmap[n];
    }

    void erase_node(size_t n) {
        connmap.erase(n);
    }
};

#endif // __GAU_NETWORK_DEF_HPP__
