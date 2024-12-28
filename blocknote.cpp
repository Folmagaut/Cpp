#include <iostream>
#include <cstdint>
#include <deque>
#include <set>
#include <string>
#include <vector>

class HotelProvider {
public:
    using BookingId = std::string;

    struct BookingData {
        std::string hotel_id;
        std::string date_from;
        std::string date_to;
        int persons;
    };

    BookingId Book(const BookingData& data) {
        using namespace std;
        cerr << "Hotel::Book: "s << data.hotel_id << endl;
        return "B1"s;
    }

    void Cancel(const BookingId& id) {
        using namespace std;
        cerr << "Cancel hotel booking "s << id << endl;
    }
};

class FlightProvider {
public:
    using BookingId = std::string;

    struct BookingData {
        std::string flight_id;
        std::string person;
        std::string date;
    };

    BookingId Book(const BookingData& data) {
        using namespace std;
        counter_++;
        if (1 < counter_) {
            throw runtime_error("Overbooking"s);
        }
        cerr << "Flight::Book: "s << data.flight_id << endl;
        return "F1"s;
    }
    void Cancel(const BookingId& id) {
        using namespace std;
        counter_--;
        cerr << "Cancel flight "s << id << endl;
    }
private:
    int counter_ = 0;
};

struct Trip {
    std::vector<HotelProvider::BookingId> hotels;
    std::vector<FlightProvider::BookingId> flights;
};

class TripManager {
public:
    using BookingId = std::string;

    struct BookingData {
        std::string person;
        std::string city;
        std::string date_from;
        std::string date_to;
    };

    Trip Book(const BookingData& data) {
        Trip trip;
        using namespace std::literals;
        // предположим, что это простейшая командировка,
        // включающая в себя перелёт туда, проживание в гостинице 
        // и перелёт обратно. Не будем писать обработку входящей информации,
        // пока нам это не нужно.
        // добавим блоки, чтобы имена переменных не конфликтовали
        {
            FlightProvider::BookingData flight_booking_data;
            flight_booking_data.flight_id = "1"s;
            trip.flights.push_back(flight_provider_.Book(flight_booking_data));
        }
        {
            HotelProvider::BookingData hotel_booking_data;
            hotel_booking_data.hotel_id = "2"s;
            trip.hotels.push_back(hotel_provider_.Book(hotel_booking_data));
        }
        {
            FlightProvider::BookingData flight_booking_data;
            flight_booking_data.flight_id = "3"s;
            trip.flights.push_back(flight_provider_.Book(flight_booking_data));
        }
        return trip;
    }

    void Cancel(Trip& trip) {
        // отменяем бронирования у провайдеров
        for (auto& id : trip.hotels) {
            hotel_provider_.Cancel(id);
        }
        // чистим вектора в структуре trip
        trip.hotels.clear();
        for (auto& id : trip.flights) {
            flight_provider_.Cancel(id);
        }
        trip.flights.clear();
    }
private:
    HotelProvider hotel_provider_;
    FlightProvider flight_provider_;
};

using namespace std;

int main() {
    TripManager tm;
    // для нашего примера воспользуемся просто пустой структурой
    auto trip = tm.Book({});
    tm.Cancel(trip);
}