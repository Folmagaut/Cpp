#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

istream& operator>>(istream& is, Query& q) {
    string str;
    is >> str;
    q = {};

    if (str == "NEW_BUS") {
        q.type = QueryType::NewBus;
        is >> q.bus; // из потока сразу присваивается структуре Query строка bus
        int stop_count;
        is >> stop_count;
        q.stops.resize(stop_count); // задаём размер вектора по количеству остановок, как в исходнике теории
        for (size_t i = 0; i < q.stops.size(); ++i) {
            is >> q.stops[i]; // записываем в вектор названия остановок
        }
    }
    
    if (str == "BUSES_FOR_STOP") {
        q.type = QueryType::BusesForStop;
        is >> q.stop; // считываем и записываем просто остановку
    }

    if (str == "STOPS_FOR_BUS") {
        q.type = QueryType::StopsForBus;
        is >> q.bus; // записываем название маршрута-автобуса
    }

    if (str == "ALL_BUSES") {
        q.type = QueryType::AllBuses;
    } 
    return is;
}

struct BusesForStopResponse {
    //string stop;
    vector<string> buses;
};

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {

    if (r.buses.empty()) {
        os << "No stop";
    } else {
        bool is_first = true;
        for (const string& bus : r.buses) {
            if (!is_first) {
                os << " "s;
            }
            is_first = false;
            os << bus;
        }
    }
    return os;
}

struct StopsForBusResponse {
    string bus;
    vector<string> stops;
    vector<pair<string, vector<string>>> s_buses; // string stop; vector<string> buses;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {

    if (r.stops.empty()) {
        os << "No bus";
    } else {
        size_t counter = 0;
        for (const string& stop : r.stops) {
            os << "Stop " << stop << ":";
            if (r.s_buses[counter].second.size() == 1) {
                os << " no interchange";
                if ((counter + 1) != r.stops.size()) {
                os << endl;
                }
            } else {
                for (const auto& v_bus : r.s_buses[counter].second) {
                    if (v_bus != r.bus) {
                    os << " " << v_bus;
                    }
                }
                if ((counter + 1) != r.stops.size()) {
                    os << endl;
                }
            }
            ++counter;
        }
    }
    return os;
}

struct AllBusesResponse {
    map<string, vector<string>> buses;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    if (r.buses.empty()) {
        os << "No buses";
    } else {
        size_t counter = 1;
        for (const auto& bus : r.buses) {
            os << "Bus " << bus.first << ":";
            for (const auto& stop : bus.second) {
                os << " " << stop;
            }
            if ((counter) != r.buses.size()) {
                os << endl;
            }
            ++counter;
        }
    }
    return os;
}

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {

        stops_for_bus_[bus] = stops;
        for (const auto& stop : stops) {
            if (!count(buses_for_stop_[stop].begin(), buses_for_stop_[stop].end(), bus)) {
                buses_for_stop_[stop].push_back(bus);
            }
        }
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {

        BusesForStopResponse b_f_s; // string stop; vector<string> buses;
        //b_f_s.stop = stop;
        if (buses_for_stop_.count(stop)) {
            for (const auto& v_bus : buses_for_stop_.at(stop)) { // map<string, vector<string>> buses_for_stop_;
            b_f_s.buses.push_back(v_bus);
            }
        }
        return b_f_s;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {

        StopsForBusResponse s_f_b;
        if (!stops_for_bus_.count(bus)) {
            s_f_b.stops.resize(0);
            return s_f_b;
        }
        s_f_b.bus = bus;
        for (const auto& stop : stops_for_bus_.at(bus)) {
            s_f_b.stops.push_back(stop);
            pair <string, vector<string>> stop_buses;
            stop_buses.first = stop;
            stop_buses.second = buses_for_stop_.at(stop);
            s_f_b.s_buses.push_back(stop_buses);          // map<string, vector<string>> stop bus1, bus2, bus3
        }
        return s_f_b;        
    }

    AllBusesResponse GetAllBuses() const {

        AllBusesResponse a_b_r;
        a_b_r.buses = stops_for_bus_;
        return a_b_r;
    }
    
private:
    map<string, vector<string>> stops_for_bus_;
    map<string, vector<string>> buses_for_stop_;
};

int main() {
    int query_count;
    Query q;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
            case QueryType::NewBus:
                bm.AddBus(q.bus, q.stops);
                break;
            case QueryType::BusesForStop:
                cout << bm.GetBusesForStop(q.stop) << endl;
                break;
            case QueryType::StopsForBus:
                cout << bm.GetStopsForBus(q.bus) << endl;
                break;
            case QueryType::AllBuses:
                cout << bm.GetAllBuses() << endl;
                break;
        }
    }
}