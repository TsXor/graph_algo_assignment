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
    struct conn_it {
        network::node_conn_map::iterator connmap_it;
        network::dest_set::iterator destset_it;
        conn_it() = default;
        conn_it(network::node_conn_map::iterator it) {
            this->connmap_it = it;
            this->destset_it = it->second.begin();
        }
        size_t start() { return this->connmap_it->first; }
        size_t dest()  { return this->destset_it->first; }
        dest_set& dests() { return this->connmap_it->second; }
        void* info()   { return this->destset_it->second; }
        conn_it& to_next() { ++this->destset_it; return *this; }
        conn_it& to_last() { --this->destset_it; return *this; }
        bool more()    { return this->destset_it != this->connmap_it->second.end(); }
    };
    node_conn_map connmap;
    
    void insert_conn(size_t from, size_t to, void* info) {
        auto& conn_list_ref = connmap[from];
        conn_list_ref.emplace_back(to, info);
        connmap[to]; // make sure it exist
    }

    void insert_conn_bidi(size_t a, size_t b, void* info) {
        insert_conn(a, b, info);
        insert_conn(b, a, info);
    }
    
    void erase_conn(size_t from, size_t to) {
        auto conn_list_it = connmap.find(from);
        if (conn_list_it != connmap.end()) {
            conn_list_it->second.remove_if(
                [to](dest_set::value_type v) {return v.first == to;}
            );
        }
    }

    conn_it find_conn(size_t from, size_t to) {
        auto conn_list_it = connmap.find(from);
        if (conn_list_it == connmap.end()) throw std::out_of_range("");
        auto& conn_list = conn_list_it->second;
        for (auto conn_dest_it = conn_list.begin(); conn_dest_it != conn_list.end(); ++conn_dest_it) {
            if (conn_dest_it->first == to) {
                conn_it ret;
                ret.connmap_it = conn_list_it;
                ret.destset_it = conn_dest_it;
                return ret;
            }
        }
        throw std::out_of_range("");
    }
    
    dest_set& insert_node(size_t n) {
        return connmap[n];
    }

    void erase_node(size_t n) {
        connmap.erase(n);
    }
};

#endif // __GAU_NETWORK_DEF_HPP__
