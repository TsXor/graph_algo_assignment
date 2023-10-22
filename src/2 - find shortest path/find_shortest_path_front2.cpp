#include "find_shortest_path.cpp"


int main(void) {
    setlocale(LC_ALL, ""); // 确保Windows下正常输出中文
    
    size_t w, h, entrance;
    std::cin >> w >> h >> entrance;
    network net;
    bool use_dijkstra = true;
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            std::string power_str;
            std::cin >> power_str;
            if (power_str != "inf") {
                ssize_t power = std::stoll(power_str);
                if (power < 0) use_dijkstra = false;
                net.insert_conn(x, y, reinterpret_cast<void*>(power));
            }
        }
    }
    
    run_algo(net, entrance, use_dijkstra);
}
