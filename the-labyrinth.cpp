#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

const uint32_t DIST_NONE{0};

struct Point {
    uint32_t x{0};
    uint32_t y{0};

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }

    std::string toString() const { return std::to_string(x) + " " + std::to_string(y); }
};

struct PointHash {
    size_t operator()(const Point& p) const noexcept {
        return static_cast<size_t>(p.x) + (static_cast<size_t>(p.y) << 32);
    }
};

inline bool IsValidPoint(const std::vector<std::string>& field, const Point& p) {
    return p.y < field.size() && p.x < field[0].size();
}

inline bool IsSpace(const std::vector<std::string>& field, const Point& p) {
    return IsValidPoint(field, p) && field[p.y][p.x] != '?' && field[p.y][p.x] != '#';
}

class Node {
public:
    Node(const Point& aPos, const uint32_t aDistance, const std::shared_ptr<Node>& aParent = nullptr)
        : pos(aPos), distance(aDistance), parent(aParent) {}

    Point getPos() const { return pos; }
    uint32_t getDistance() const { return distance; }

    std::string toString() const { return pos.toString() + " " + std::to_string(distance); }

private:
    std::shared_ptr<Node> parent;
    Point pos;
    uint32_t distance{DIST_NONE};
};

class PathTree {
public:
    PathTree(const std::vector<std::string>& field, const Point& start, const Point& end) {
        tryFindPath(field, start, end);
        printPathTree(field);
    }

    void printPathTree(const std::vector<std::string>& field) const {
        auto fieldTmp = field;
        for (const auto& node : nodes) {
            if (!node) {
                continue;
            }
            const auto pos = node->getPos();
            if (!IsValidPoint(fieldTmp, pos)) {
                continue;
            }
            fieldTmp[pos.y][pos.x] = 0x30 + node->getDistance() % 10;
        }
        for (const auto& row : fieldTmp) {
            std::cout << row << std::endl;
        }
    }

private:
    bool tryFindPath(const std::vector<std::string>& field, const Point& start, const Point& end) {
        root = nullptr;
        goal = nullptr;
        nodes.clear();
        if (!IsValidPoint(field, start)) {
            return false;
        }
        std::unordered_set<Point, PointHash> visitedPoints;
        std::queue<std::shared_ptr<Node>> nodesToVisit;
        root = std::make_shared<Node>(start, 0);
        nodes.push_back(root);
        if (IsSpace(field, start)) {
            nodesToVisit.push(root);
        }
        while (!nodesToVisit.empty()) {
            auto node = nodesToVisit.front();
            nodesToVisit.pop();
            if (!node) {
                continue;
            }
            // std::cout << "node: " << node->toString() << ", queue: " << nodesToVisit.size() << std::endl;
            const auto pos = node->getPos();
            if (pos == end) {
                goal = node;
            }
            visitedPoints.insert(pos);
            for (const auto p :
                 {Point{pos.x - 1, pos.y}, Point{pos.x + 1, pos.y}, Point{pos.x, pos.y - 1}, Point{pos.x, pos.y + 1}}) {
                if (!IsSpace(field, p) || visitedPoints.count(p)) {
                    continue;
                }
                const auto newNode = std::make_shared<Node>(p, node->getDistance() + 1, node);
                nodesToVisit.push(newNode);
                nodes.push_back(newNode);
            }
        }
        return !!goal;
    }
    std::shared_ptr<Node> root{nullptr};
    std::shared_ptr<Node> goal{nullptr};
    std::vector<std::shared_ptr<Node>> nodes;
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
    std::vector<std::string> field(r, std::string(c, '?'));

    uint32_t iteration{0};
    // game loop
    while (iteration < 2) {
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

        PathTree pathTree(field, Point{kc, kr}, Point{});

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        std::cout << "RIGHT" << std::endl;  // Rick's next move (UP DOWN LEFT or RIGHT).

        ++iteration;
    }
}
