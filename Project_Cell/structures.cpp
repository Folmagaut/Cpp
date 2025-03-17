#include "common.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <string>

const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;
const int CHAR_A = static_cast<int>('A');

const Position Position::NONE = {-1, -1};

// Реализуйте методы:
bool Position::operator==(const Position rhs) const {
    return (row == rhs.row && col == rhs.col);
}

bool Position::operator<(const Position rhs) const {
    if (row != rhs.row) {
        return row < rhs.row;
    }
    return col < rhs.col;
}


bool Position::IsValid() const {
    return row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLS;
}

std::string Position::ToString() const {
    if (!IsValid()) {
        return "";
    }
    
    std::string str_pos;

    int decimal_col = col;
    while (decimal_col >= 0) {
        int remainder = decimal_col % LETTERS;
        str_pos += static_cast<char>(CHAR_A + remainder);
        if (decimal_col < LETTERS) {
            break;
        }
        decimal_col = (decimal_col / LETTERS) - 1;
    }
    std::reverse(str_pos.begin(), str_pos.end());
    str_pos += std::to_string(row + 1);    

    return str_pos;
}

// символы латинского алфавита от A до Z имеют следующие номера индексов: 'A'...'Z' - 65...90
Position Position::FromString(std::string_view str) {
    /* if (str.size() > 8) {
        return Position::NONE;
    }

    std::string s_row;
    std::string s_col;

    size_t cnt = 0;
    for (const char c : str) {
        if (isalpha(c) && isupper(c)) {
            s_col.push_back(c);
        } else if (isdigit(c)) {
            s_row = str.substr(cnt);
            break;
        } else {
            return Position::NONE;
        }
        ++cnt;
    }

    if (s_col.empty() || s_row.empty() || s_col.size() > MAX_POS_LETTER_COUNT) {
        return Position::NONE;
    }

    int col = 0;
    size_t s_col_size = s_col.size();
    for (size_t i = 0; i < s_col_size; ++i) {
        col += (static_cast<int>(s_col.back()) - CHAR_A + 1) * pow(LETTERS, i);
        s_col.pop_back();
    } */
    size_t col_end = 0;
    while (col_end < str.size() && isalpha(str[col_end]) /* && isupper(str[col_end]) */) {
        col_end++;
    }

    if (col_end == 0 || col_end == str.size() || col_end > MAX_POS_LETTER_COUNT) {
        return Position::NONE;
    }

    std::string_view col_str = str.substr(0, col_end);
    std::string_view row_str = str.substr(col_end);
    for (const char c : row_str) {
        if (!isdigit(c)) {
            return Position::NONE;
        }
    }

    int col = 0;
    int multiplier = 1;
    for (auto it = col_str.rbegin(); it != col_str.rend(); ++it) {
        if (!isupper(*it)) {
            return Position::NONE;
        }
        col += (*it - CHAR_A + 1) * multiplier;
        multiplier *= LETTERS;
    }
    
    int row = 0;
    size_t first_nonconv_char_pos;
    try {
        row = std::stoi(std::string(row_str), &first_nonconv_char_pos);
    } catch (...) {
        return Position::NONE;
    }
    if (row_str.size() != first_nonconv_char_pos) {
        return Position::NONE;
    }

    Position pos = {row - 1, col - 1};
    if (!pos.IsValid()) {
        return Position::NONE;
    }
    return pos;
}

/*
decimal = d_n * n^n + d_n-1 * n^(n-1) + ... + d_1 * n^1 + d_0 * n^0
Где:
decimal - десятичное представление числа.
n - основание n-ричной системы счисления.
d_i - цифра в i-й позиции n-ричного числа (справа налево, начиная с 0). */