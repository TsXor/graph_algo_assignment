#include <iostream>
#include "../utils/max_match.hpp"

int main(void) {
    network net;
    net.insert_conn_bidi(1, 11, 0);
    net.insert_conn_bidi(2, 11, 0);
    net.insert_conn_bidi(2, 12, 0);
    net.insert_conn_bidi(3, 11, 0);
    net.insert_conn_bidi(3, 12, 0);
    net.insert_conn_bidi(3, 13, 0);
    net.insert_conn_bidi(4, 12, 0);
    net.insert_conn_bidi(4, 13, 0);
    net.insert_conn_bidi(4, 14, 0);
    net.insert_conn_bidi(5, 13, 0);
    net.insert_conn_bidi(5, 14, 0);
    net.insert_conn_bidi(5, 20, 0);
    net.insert_conn_bidi(6, 14, 0);
    net.insert_conn_bidi(6, 15, 0);
    net.insert_conn_bidi(6, 16, 0);
    net.insert_conn_bidi(7, 15, 0);
    net.insert_conn_bidi(7, 16, 0);
    net.insert_conn_bidi(7, 17, 0);
    net.insert_conn_bidi(8, 17, 0);
    net.insert_conn_bidi(8, 18, 0);
    net.insert_conn_bidi(9, 19, 0);
    
    std::vector<size_t> group{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    network matches = max_match::kuhn_munkres(net, group.begin(), group.end());
    for (auto i : group) {
        std::cout << i << " <-> " << matches.connmap.at(i).front().first << '\n';
    }
}
