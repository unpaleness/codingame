#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

const uint32_t DIST_NONE{-1};

class Node;

struct Point {
    uint32_t x{0};
    uint32_t y{0};

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

template <>
struct std::hash<Point> {
    size_t operator()(cosnt Point& p) const noexcept {
        return static_cast<size_t>(p.x) + (static_cast<size_t>(p.y) << 32);
    }
};

class PathTree {
public:
    PathTree(const std::vector<std::string>& field, const Point& start, const Point& end) {
        std::unordered_set<size_t> visitedNodeHashes;
        std::queue<std::shared_ptr<Node>> nodesToVisit;
    }

private:
    std::shared_ptr<Node> root{nullptr};
};

class Node {
public:
    Node() { daughters.reserve(4); }

private:
    char type{'?'};
    std::vector<std::shared_ptr<Node>> daughters;
    std::weak_ptr<Node> parent{nullptr};
    Point pos;
    uint32_t distance{DIST_NONE};
};

}  // namespace

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main() {
    uint32_t r;  // number of rows.
    uint32_t c;  // number of columns.
    uint32_t a;  // number of rounds between the time the alarm countdown is activated and the time the alarm goes off.
    std::cin >> r >> c >> a;
    std::cin.ignore();
    std::vector<std::string> field(r, string(c, '?'));

    // game loop
    while (1) {
        uint32_t kr;  // row where Rick is located.
        uint32_t kc;  // column where Rick is located.
        std::cin >> kr >> kc;
        std::cin.ignore();
        for (uint32_t i = 0; i < r; i++) {
            std::string row;  // C of the characters in '#.TC?' (i.e. one line of the ASCII maze).
            std::cin >> row;
            std::cin.ignore();
            field[i] = row;
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        std::cout << "RIGHT" << std::endl;  // Rick's next move (UP DOWN LEFT or RIGHT).
    }
}
