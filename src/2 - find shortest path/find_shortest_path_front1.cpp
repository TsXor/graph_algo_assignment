#include "find_shortest_path.cpp"


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

    run_algo(net, entrance, use_dijkstra);
}
