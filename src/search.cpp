#include <iostream>
#include "utils/search.hpp"


int main(void) {
    size_t n;
    std::cin >> n;
    size_t a, b;
    network net;
    for (size_t i = 0; i < n; i++) {
        std::cin >> a >> b;
        net.insert_conn(a, b, nullptr);
        net.insert_conn(b, a, nullptr);
    }
    
    std::cout << "*";
    search::dfs_iter(net, 1,
        [](size_t n, std::vector<size_t> history) -> bool {
            std::cout << " -> " << n;
            return false;
        }
    );
    std::cout << std::endl;
    
    std::cout << "*";
    search::bfs_iter(net, 1,
        [](size_t n, size_t depth) -> bool {
            std::cout << " -> " << n;
            return false;
        }
    );
    std::cout << std::endl;
}
