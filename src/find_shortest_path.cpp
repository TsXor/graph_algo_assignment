#include <clocale>
#include <iostream>
#include "utils/shortest_path.hpp"


void print_path_result(size_t entrance, std::unordered_map<size_t, shortest_path::path>& paths) {
    for (auto& [node, path] : paths) {
        std::wcout << L"从" << entrance << L"开始到" << node << L"的距离为" << path.distance << L'\n';
        std::wcout << L"路径为：" << entrance;
        for (auto& conn : path.conns) {
            std::wcout << L" -(" << reinterpret_cast<ssize_t>(conn.second) << L")-> " << conn.first;
        }
        std::cout << '\n';
    }
}

int main(void) {
    setlocale(LC_ALL, ""); // 确保Windows下正常输出中文
    
    size_t n, entrance;
    std::cin >> n >> entrance;
    network net;
    bool use_dijkstra = true;
    for (size_t i = 0; i < n; i++) {
        size_t src, dst; ssize_t power;
        std::cin >> src >> dst >> power;
        if (power < 0) use_dijkstra = false;
        net.insert_conn(src, dst, reinterpret_cast<void*>(power));
    }

    {
        std::cout << "bellman-ford: " << '\n';
        auto paths = shortest_path::bellman_ford(net, entrance).first;
        print_path_result(entrance, paths);
        std::cout << std::endl;
    }
    
    if (use_dijkstra) {
        std::cout << "dijkstra: " << '\n';
        auto paths = shortest_path::dijkstra(net, entrance);
        print_path_result(entrance, paths);
        std::cout << std::endl;
    } else {
        std::wcout << L"存在负权边，不适用dijkstra" << std::endl;
    }
}