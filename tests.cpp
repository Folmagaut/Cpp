#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

void PrintTree(ostream& dst, const path& p, int offset) {
    error_code err;
    auto status = filesystem::status(p, err);
    offset += 2;
    vector<string> elements;
    if (status.type() == filesystem::file_type::directory) {
        for (const auto& dir_entry: filesystem::directory_iterator(p)) {
        elements.push_back(dir_entry.path().filename().string());
        }
    }
    sort(elements.begin(), elements.end(), [] (string lhs, string rhs) {
        return lhs > rhs;
    });

    for (const auto& s : elements) {
        path pth(p / s);
        auto status = filesystem::status(pth, err);
        if (status.type() == filesystem::file_type::directory) {
            dst << string(offset, ' ') << s << endl;
            PrintTree(dst, pth, offset);
        }
        else {
            dst << string(offset, ' ') << s << endl;
        }
    }
}

// напишите эту функцию
void PrintTree(ostream& dst, const path& p) {
    dst << p.string() << endl;
    int offset = 0;
    PrintTree(dst, p, offset);
}

int main() {
    error_code err;
    filesystem::remove_all("test_dir", err);
    filesystem::create_directories("test_dir"_p / "a"_p, err);
    filesystem::create_directories("test_dir"_p / "b"_p, err);

    ofstream("test_dir"_p / "b"_p / "f1.txt"_p);
    ofstream("test_dir"_p / "b"_p / "f2.txt"_p);
    ofstream("test_dir"_p / "a"_p / "f3.txt"_p);

    ostringstream out;
    PrintTree(out, "test_dir"_p);
    cout << out.str();
    /* assert(out.str() ==
        "test_dir\n"
        "  b\n"
        "    f2.txt\n"
        "    f1.txt\n"
        "  a\n"
        "    f3.txt\n"s
    ); */
}