// buildings.h

#include <vector>

namespace buildings {
struct Materials {};

Materials GetMaterials();

class Skyscraper {
    struct Floor {
        /* 1 */ /* buildings:: */Materials materials;
        int number;
    };

    Floor BuildFloor(/* 2 */ /* buildings:: */Materials materials);
    void BuildRoof(buildings::Materials materials);
    int GetMaterials(buildings::Materials source);

    bool IsComplete() const {
        return static_cast<int>(floors_.size()) < total_floors_;
    }

public:
    void Build() {
        while (!IsComplete()) {
            auto materials = /* 3 */ buildings::GetMaterials();
            floors_.push_back(BuildFloor(std::move(materials)));
        }
    }

private:
    int total_floors_ = 100;
    std::vector<Floor> floors_;
    /* 4 */ /* buildings:: */Materials roof_;
};
}  // namespace buildings