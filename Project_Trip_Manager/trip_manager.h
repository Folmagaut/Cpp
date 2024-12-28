#pragma once
#include "flight_provider.h"
#include "hotel_provider.h"

#include <string>
#include <vector>

class Trip {
public:

    Trip(HotelProvider& hp, FlightProvider& fp) : hp_(hp) , fp_(fp) {
    }
    
    Trip(const Trip& other): hp_(other.hp_), fp_(other.fp_), hotels(other.hotels), flights(other.flights) {
    }
    
    Trip(Trip&& other) : hp_(other.hp_), fp_(other.fp_), hotels(std::move(other.hotels)), flights(std::move(other.flights)) {
    }
    
    Trip& operator=(const Trip& other) {
        hp_ = other.hp_;
        fp_ = other.fp_;
        hotels = other.hotels;
        flights = other.flights;
        return *this;
    }
    
    Trip& operator=(Trip&& other) {
        hp_ = other.hp_;
        fp_ = other.fp_;
        hotels = std::move(other.hotels);
        flights = std::move(other.flights);
        return *this;
    }
    
    void Cancel() {
        for (const auto& hotel : hotels) {
            hp_.Cancel(hotel);
        }
        hotels.clear();

        for (const auto& flight : flights) {
            fp_.Cancel(flight);
        }
        flights.clear();
    }

    ~Trip() {
        Cancel();
    }

private:
    HotelProvider& hp_;
    FlightProvider& fp_;
public:
    std::vector<HotelProvider::BookingId> hotels;
    std::vector<FlightProvider::BookingId> flights;
};

class TripManager {
public:
    using BookingId = std::string;
    struct BookingData {
        std::string city_from;
        std::string city_to;
        std::string date_from;
        std::string date_to;
    };

    Trip Book(const BookingData& data) {
        Trip trip(hotel_provider_, flight_provider_);
        {
            FlightProvider::BookingData flight_booking_data{data.city_from, data.city_to, data.date_from};
            trip.flights.push_back(flight_provider_.Book(flight_booking_data));
        }
        {
            HotelProvider::BookingData hotel_booking_data{data.city_to, data.date_from, data.date_to};
            trip.hotels.push_back(hotel_provider_.Book(hotel_booking_data));
        }
        {
            FlightProvider::BookingData flight_booking_data{data.city_to, data.city_from, data.date_to};
            trip.flights.push_back(flight_provider_.Book(flight_booking_data));
        }
        return trip;
    }

    void Cancel(Trip& trip) {
        trip.Cancel();
    }

private:
    HotelProvider hotel_provider_;
    FlightProvider flight_provider_;
};