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
    
    network matches = max_match::blossom(net);
    std::vector<std::pair<size_t, size_t>> unique_matches;
    for (auto& [from, dests] : matches.connmap) {
        for (auto& [to, _] : dests) {
            unique_matches.emplace_back(std::min(from, to), std::max(from, to));
        }
    }
    std::sort(unique_matches.begin(), unique_matches.end(),
        [](std::pair<size_t, size_t>& a, std::pair<size_t, size_t>& b) {
            return a.first == b.first ? a.second < b.second : a.first < b.first;
        }
    );
    std::unique(unique_matches.begin(), unique_matches.end(),
        [](std::pair<size_t, size_t>& a, std::pair<size_t, size_t>& b) {
            return a.first == b.first && a.second == b.second;
        }
    );
    unique_matches.resize(unique_matches.size() / 2);
    for (auto& match : unique_matches) {
        std::cout << match.first << " <-> " << match.second << '\n';
    }
}
