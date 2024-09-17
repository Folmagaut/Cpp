#pragma once

#include <fstream>
#include <iostream>
#include <string>

inline bool DecodeRLE(const std::string& src_name, const std::string& dst_name) {
    using namespace std;

    ifstream in(src_name, ios::binary);
    if (!in) {
        return false;
    }
    ofstream out(dst_name, ios::binary);

    do {
        char header_c;
        if (!in.get(header_c)) {
            return true;
        }

        unsigned char header = static_cast<unsigned char>(header_c);

        size_t size = static_cast<size_t>(header >> 1) + 1;

        static const int max_block_size = 128;
        char block[max_block_size];

        if ((header & 1) == 0) {
            if (!in.read(block, size)) {
                return false;
            }
            out.write(block, size);
        } else {
            char c;
            if (!in.get(c)) {
                return false;
            }

            string s(size, c);
            out.write(s.data(), size);
        }
    } while (in);

    return true;
}