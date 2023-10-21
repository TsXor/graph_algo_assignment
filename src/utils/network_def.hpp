#ifndef __GAU_NETWORK_DEF_HPP__
#define __GAU_NETWORK_DEF_HPP__

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <memory>


struct network {
    struct connection {
        size_t connected_a;
        size_t connected_b;
        void* info;
    };
    using node_conn_map = std::unordered_multimap<size_t, std::shared_ptr<connection>>;
    static inline size_t get_point_to(node_conn_map::value_type& p) {
        return p.first == p.second->connected_a ? p.second->connected_b : p.second->connected_a;
    }

    node_conn_map connmap;

    std::weak_ptr<connection> insert_conn(size_t a, size_t b, void* info) {
        auto conn_ptr = std::make_shared<connection>();
        conn_ptr->connected_a = a;
        conn_ptr->connected_b = b;
        conn_ptr->info = info;
        connmap.emplace(a, conn_ptr);
        connmap.emplace(b, conn_ptr);
        return conn_ptr;
    }
    
    void erase_conn(size_t a, size_t b) {
        for (auto a_iter = connmap.find(a); a_iter != connmap.end(); ++a_iter) {
            if (get_point_to(*a_iter) == b) {
                connmap.erase(a_iter); break;
            }
        }
        for (auto b_iter = connmap.find(b); b_iter != connmap.end(); ++b_iter) {
            if (get_point_to(*b_iter) == a) {
                connmap.erase(b_iter); break;
            }
        }
    }
    void erase_conn(const connection& conn) {
        this->erase_conn(conn.connected_a, conn.connected_b);
    }
    
    void del_node(size_t n) {
        connmap.erase(n);
    }
};

#endif // __GAU_NETWORK_DEF_HPP__
