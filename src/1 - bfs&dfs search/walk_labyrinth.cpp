#include <iostream>
#include <vector>


struct Point {
    size_t x;
    size_t y;
};

std::ostream& operator<<(std::ostream& out, Point p) {
    out << '(' << p.x << ',' << p.y << ')';
    return out;
}

bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

template <>
struct std::hash<Point> : std::hash<size_t> {
    size_t operator()(const Point& val) const noexcept {
        using th = std::hash<size_t>;
        return (th::operator()(val.x) << 16) ^ (th::operator()(val.y) >> 16);
    }
};

enum {
    RIGHT=0, DOWN, LEFT, UP
};

class MyMatrix {
    std::vector<char> data;
    size_t w, h;
public:
    MyMatrix(size_t w, size_t h) {
        this->w = w; this->h = h;
        data.reserve(w * h);
    }
    MyMatrix(const MyMatrix& x) {
        this->data = x.data;
        this->w = x.w;
        this->h = x.h;
    }
    char get(size_t x, size_t y, char default_ = '1') {
        return (x < w && y < h) ? data[x + y * w] : default_;
    }
    void set(size_t x, size_t y, char val) {
        if (x < w && y < h) data[x + y * w] = val;
    }
    void read_stdio() {
        size_t i = 0;
        while (i < w * h) {
            char c; std::cin.get(c);
            if (isspace(c)) continue;
            data.push_back(c);
            i++;
        }
    }
};

Point move_point(MyMatrix& labyrinth, Point& p, uint8_t direction) {
    Point np = p;
    switch (direction) {
        case RIGHT: {
            if (labyrinth.get(p.x + 1, p.y) != '1') np.x++;
        } break;
        case DOWN: {
            if (labyrinth.get(p.x, p.y + 1) != '1') np.y++;
        } break;
        case LEFT: {
            if (labyrinth.get(p.x - 1, p.y) != '1') np.x--;
        } break;
        case UP: {
            if (labyrinth.get(p.x, p.y - 1) != '1') np.y--;
        } break;
    }
    return np;
}

std::vector<Point> dfs(MyMatrix& labyrinth, Point entrance, Point exit) {
    std::vector<Point> path{entrance};
    std::vector<uint8_t> directions{0};
    MyMatrix visited(labyrinth); visited.set(entrance.x, entrance.y, '1');
    while (!path.empty()) {
        if (directions.back() >= 4) {
            path.pop_back();
            directions.pop_back();
        } else {
            Point next = move_point(labyrinth, path.back(), directions.back());
            directions.back()++;
            if (visited.get(next.x, next.y) == '0') {
                path.push_back(next);
                directions.push_back(0);
                visited.set(next.x, next.y, '1');
            }
            if (next == exit) break;
        }
    }
    return path;
}

size_t bfs(MyMatrix& labyrinth, Point entrance, Point exit) {
    std::vector<Point> layer{entrance};
    size_t depth = 0;
    MyMatrix visited(labyrinth); visited.set(entrance.x, entrance.y, '1');
    while (!layer.empty()) {
        depth++;
        std::vector<Point> new_layer;
        for (auto& p : layer) {
            for (uint8_t direction = 0; direction < 4; direction++) {
                Point next = move_point(labyrinth, p, direction);
                if (visited.get(next.x, next.y) == '0') {
                    new_layer.push_back(next);
                    visited.set(next.x, next.y, '1');
                }
                if (next == exit) return depth;
            }
        }
        layer = std::move(new_layer);
    }
    return 0;
}


int main(void) {
    size_t w, h;
    std::cin >> w >> h;
    Point entrance, exit;
    std::cin >> entrance.x >> entrance.y
             >> exit.x >> exit.y;
    MyMatrix labyrinth(w, h);
    labyrinth.read_stdio();

    std::cout << "dfs: ";
    auto path = dfs(labyrinth, entrance, exit);
    if (path.empty()) {
        std::cout << "failed";
    } else {
        std::cout << "*";
        for (auto& p : path) {
            std::cout << " -> " << p;
        }
    }
    std::cout << std::endl;

    std::cout << "bfs: ";
    auto depth = bfs(labyrinth, entrance, exit);
    if (!depth) {
        std::cout << "failed";
    } else {
        std::cout << "in " << depth << " steps";
    }
    std::cout << std::endl;
}
