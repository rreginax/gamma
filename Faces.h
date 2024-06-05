#ifndef FACES_H
#define FACES_H

#include <vector>
#include <string>
#include <sstream>

class Faces {
private:
    std::vector<std::vector<int>> interior;
    std::vector<int> external;
    int size;

public:
    Faces(const std::vector<std::vector<int>>& interior, const std::vector<int>& external) {
        if (!interior.empty() && !external.empty()) {
            this->interior = interior;
            this->external = external;
            size = interior.size() + external.size();
        }
        else {
            size = 0;
        }
    }

    std::vector<std::vector<int>> getInterior() const {
        return interior;
    }

    std::vector<int> getExternal() const {
        return external;
    }

    std::string toString() const {
        std::ostringstream result;
        result << "Размер граней = " << size << "\nВнешняя грань\n";
        for (const int& e : external) {
            result << e << " ";
        }
        result << "\nВнутренние грани:\n";
        for (const auto& face : interior) {
            for (const int& f : face) {
                result << f << " ";
            }
            result << "\n";
        }
        return result.str();
    }
};

#endif // FACES_H
