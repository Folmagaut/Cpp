// без изменений с прошлого спринта
#define _USE_MATH_DEFINES
#include <cmath>
#include "geo.h"

namespace geo {

bool Coordinates::operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
}
bool Coordinates::operator!=(const Coordinates& other) const {
        return !(*this == other);
}

}  // namespace geo