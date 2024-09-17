#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

void CreateFile(path p) {
    ofstream file(p);
    if (file) {
        cout << "Файл создан: "s << p.string() << endl;
    } else {
        cout << "Ошибка создания файла: "s << p.string() << endl;
    }
}

void PrintFileOrFolder(filesystem::file_status status) {
    if (status.type() == filesystem::file_type::regular) {
        cout << "Путь указывает на файл"s << endl;
    } else if (status.type() == filesystem::file_type::directory) {
        cout << "Путь указывает на папку"s << endl;
    } else {
        cout << "Путь указывает другой объект"s << endl;
    }
}
void PrintFileOrFolder(filesystem::path p) {
    error_code err;
    auto status = filesystem::status(p, err);
    
    if (err) {
        return;
    }

    PrintFileOrFolder(status);
}

int main() {
    path p = "a"_p / "folder"_p;
    filesystem::create_directories(p);
    filesystem::create_directory(p / "subfolder"_p);
    std::ofstream(p / "file.txt"_p) << "File content"s;

    for (const auto& dir_entry: filesystem::directory_iterator(p)) {
        PrintFileOrFolder(dir_entry.status());
    }
}

/*
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

 struct PathInfo {
    bool isDirectory;
    std::string name;
};

void PrintTree(ostream& dst, const path& p, const filesystem::file_status& status, int offset) {
    //error_code err;
    offset += 2;
    //auto status = filesystem::status(p, err);
    for (const auto& dir_entry: filesystem::directory_iterator(p)) {
        if (status.type() == filesystem::file_type::directory) {
            PrintTree(dst, dir_entry.path(), status, offset);
        }
    }
}

// напишите эту функцию
void PrintTree(ostream& dst, const path& p) {
    
    vector<string> out_to_dst;
    out_to_dst.push_back(p.string());
    dst << p.string() << endl;

    for (const auto& dir_entry: filesystem::directory_iterator(p)) {
        //error_code err;
        //auto status = filesystem::status(dir_entry.path(), err);
        if (dir_entry.status().type() == filesystem::file_type::directory) {
            out_to_dst.push_back(dir_entry.path().filename().string());

            for (const auto& dir_entry : filesystem::directory_iterator(dir_entry.path())) {
                out_to_dst.push_back(dir_entry.path().filename().string());
            }

        } else if (status.type() == filesystem::file_type::regular) {
            out_to_dst.push_back(dir_entry.path().filename().string());
        }
    }
    for (auto s : out_to_dst) {
        cout << s << endl;
    }
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
    /* assert(out.str() ==
        "test_dir\n"
        "  b\n"
        "    f2.txt\n"
        "    f1.txt\n"
        "  a\n"
        "    f3.txt\n"s
    );
}
*/