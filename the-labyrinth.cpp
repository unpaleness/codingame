#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

const int32_t DIST_NONE{0};

struct Point {
    int32_t x{0};
    int32_t y{0};

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    Point operator-(const Point& other) const { return {x - other.x, y - other.y}; }

    friend std::ostream& operator<<(std::ostream& out, const Point& p) { return out << p.x << ' ' << p.y; }
};

struct PointHash {
    size_t operator()(const Point& p) const noexcept {
        return static_cast<size_t>(p.x) + (static_cast<size_t>(p.y) << 32);
    }
};

template <class T>
inline bool IsValidPoint(const T& field, const Point& p) {
    return p.y >= 0 && p.y < field.size() && p.x >= 0 && p.x < field[0].size();
}

inline bool IsSpace(const std::vector<std::string>& field, const Point& p) {
    return IsValidPoint(field, p) && field[p.y][p.x] != '?' && field[p.y][p.x] != '#';
}

inline std::optional<Point> TryGetRoomLocation(const std::vector<std::string>& field, const char room) {
    for (size_t i = 0; i < field.size(); ++i) {
        const auto& row = field[i];
        if (const auto roomIndex = row.find(room); roomIndex != row.npos) {
            return {{static_cast<int32_t>(roomIndex), static_cast<int32_t>(i)}};
        }
    }
    return std::nullopt;
}

inline bool HasSpecificNeighbour(const std::vector<std::string>& field, const Point& pos, const char targetRoom) {
    for (const auto p :
         {Point{pos.x - 1, pos.y}, Point{pos.x + 1, pos.y}, Point{pos.x, pos.y - 1}, Point{pos.x, pos.y + 1}}) {
        if (IsValidPoint(field, p) && field[p.y][p.x] == targetRoom) {
            return true;
        }
    }
    return false;
}

inline std::string DirToString(const Point& dir) {
    if (dir.x > 0) {
        return "RIGHT";
    } else if (dir.x < 0) {
        return "LEFT";
    } else if (dir.y > 0) {
        return "DOWN";
    } else if (dir.y < 0) {
        return "UP";
    }
    return "NONE";
}

class Node {
public:
    Node(const Point& aPos, const int32_t aDistance, const std::shared_ptr<Node>& aParent = nullptr)
        : pos(aPos), distance(aDistance), parent(aParent) {}

    Point getPos() const { return pos; }
    int32_t getDistance() const { return distance; }
    std::shared_ptr<Node> getParent() const { return parent; }

    friend std::ostream& operator<<(std::ostream& out, const Node& node) {
        return out << node.pos << ' ' << node.distance;
    }

private:
    std::shared_ptr<Node> parent;
    Point pos;
    int32_t distance{DIST_NONE};
};

class PathTree {
public:
    std::optional<Point> findPathToNearestUnvisited(const std::vector<std::string>& field,
                                                    const std::vector<std::vector<bool>>& fieldVisited,
                                                    const Point& start) {
        findPath(field, start, true, [&field, &fieldVisited](const std::shared_ptr<Node>& node) {
            if (!node) {
                return false;
            }
            const auto pos = node->getPos();
            return IsValidPoint(fieldVisited, pos) && !fieldVisited[pos.y][pos.x] &&
                   HasSpecificNeighbour(field, pos, '?');
        });
        printPathTree(field);
        return getFirstStep();
    }

    std::optional<Point> findPathToTargetPoint(const std::vector<std::string>& field,
                                               const Point& start,
                                               const Point& target) {
        findPath(field, start, true,
                 [&target](const std::shared_ptr<Node>& node) { return !!node && node->getPos() == target; });
        printPathTree(field);
        return getFirstStep();
    }

    void printPathTree(const std::vector<std::string>& field) const {
        auto fieldTmp = field;
        auto node = goal;
        while (node) {
            const auto pos = node->getPos();
            if (!IsValidPoint(fieldTmp, pos)) {
                continue;
            }
            fieldTmp[pos.y][pos.x] = 'o';
            node = node->getParent();
        }
        for (const auto& row : fieldTmp) {
            std::cerr << row << std::endl;
        }
    }

private:
    void findPath(const std::vector<std::string>& field,
                  const Point& start,
                  const bool stopThenFound,
                  const std::function<bool(const std::shared_ptr<Node>& node)>& foundCondition) {
        goal = nullptr;
        nodes.clear();
        if (!IsValidPoint(field, start)) {
            return;
        }
        nodes.reserve(field.size() * field[0].size());
        std::unordered_set<Point, PointHash> visitedPoints;
        std::queue<std::shared_ptr<Node>> nodesToVisit;
        const auto root = std::make_shared<Node>(start, 0);
        nodes.push_back(root);
        if (IsSpace(field, start)) {
            nodesToVisit.push(root);
        }
        visitedPoints.insert(root->getPos());
        while (!nodesToVisit.empty()) {
            auto node = nodesToVisit.front();
            nodesToVisit.pop();
            if (!node) {
                continue;
            }
            const auto pos = node->getPos();
            if (foundCondition(node)) {
                goal = node;
                if (stopThenFound) {
                    return;
                }
            }
            // std::cerr << __FUNCTION__ << " node:" << *node << ", queue: " << nodesToVisit.size()
            //           << " visitedPoints: " << visitedPoints.size() << std::endl;
            for (const auto p :
                 {Point{pos.x - 1, pos.y}, Point{pos.x + 1, pos.y}, Point{pos.x, pos.y - 1}, Point{pos.x, pos.y + 1}}) {
                if (!IsSpace(field, p) || visitedPoints.count(p)) {
                    continue;
                }
                visitedPoints.insert(p);
                const auto newNode = std::make_shared<Node>(p, node->getDistance() + 1, node);
                nodesToVisit.push(newNode);
                nodes.push_back(newNode);
                // std::cerr << ' ' << __FUNCTION__ << " added: " << *newNode << std::endl;
            }
        }
    }

    std::optional<Point> getFirstStep() const {
        auto firstNode = goal;
        auto rootNode = firstNode ? firstNode->getParent() : nullptr;
        while (firstNode && rootNode && rootNode->getParent()) {
            firstNode = rootNode;
            rootNode = rootNode->getParent();
        }
        if (firstNode && rootNode) {
            return firstNode->getPos() - rootNode->getPos();
        }
        return std::nullopt;
    }

    std::shared_ptr<Node> goal{nullptr};
    std::vector<std::shared_ptr<Node>> nodes;
};

enum class Stage : uint8_t {
    SearchControlRoom = 0,
    GoToControlRoom = 1,
    GoBack = 2,
};

}  // namespace

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main() {
    int32_t r;  // number of rows.
    int32_t c;  // number of columns.
    int32_t a;  // number of rounds between the time the alarm countdown is activated and the time the alarm goes off.
    std::cin >> r >> c >> a;
    std::cin.ignore();
    std::vector<std::string> field(r, std::string(c, '?'));
    std::vector<std::vector<bool>> fieldVisited(r, std::vector<bool>(c, false));
    bool isContolRoomReached{false};
    PathTree pathTree;

    int32_t iteration{0};
    // game loop
    while (iteration < 10000) {
        Point pos;  // Rick's current position
        std::cin >> pos.y >> pos.x;
        std::cin.ignore();

        std::cerr << "Rick's pos: " << pos << std::endl;

        fieldVisited[pos.y][pos.x] = true;

        for (int32_t i = 0; i < r; i++) {
            std::string row;  // C of the characters in '#.TC?' (i.e. one line of the ASCII maze).
            std::cin >> row;
            std::cin.ignore();
            field[i] = row;
        }

        const auto controlRoomPos = TryGetRoomLocation(field, 'C');

        std::cerr << "controlRoomPos: " << controlRoomPos.value_or(Point{-1, -1}) << std::endl;

        if (controlRoomPos && *controlRoomPos == pos) {
            isContolRoomReached = true;
        }

        std::cerr << "isControlRoomReached: " << isContolRoomReached << std::endl;

        std::optional<Point> dir;
        if (isContolRoomReached) {
            if (const auto startRoomPos = TryGetRoomLocation(field, 'T')) {
                dir = pathTree.findPathToTargetPoint(field, pos, *startRoomPos);
            }
        } else {
            if (controlRoomPos) {
                dir = pathTree.findPathToTargetPoint(field, pos, *controlRoomPos);
            } else {
                dir = pathTree.findPathToNearestUnvisited(field, fieldVisited, pos);
            }
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        std::cout << DirToString(dir.value_or(Point{})) << std::endl;  // Rick's next move (UP DOWN LEFT or RIGHT).

        ++iteration;
    }
}
