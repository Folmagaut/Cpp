// без изменений с прошлого спринта
#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

const int EARTH_RADIUS = 6371000;

namespace geo {

struct Coordinates {
    double lat;
    double lng;
    bool operator==(const Coordinates& other) const;
    bool operator!=(const Coordinates& other) const;
};

inline double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    if (from == to) {
        return 0;
    }
    static const double dr = 3.14159265358979323846 / 180.; // M_PI == 3.1415926535 - почему-то не компилируется с M_PI
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * EARTH_RADIUS;
}

} // geo