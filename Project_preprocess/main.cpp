#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

bool Preprocess(ifstream& in_file, ofstream& out_file, const string& relative_file_name, const path& parent_pth,
                            const vector<path>& include_directories, int& line_number, const string& in_file_name) {

    path new_relative_in_file = parent_pth / relative_file_name;
    if (exists(new_relative_in_file)) {

        ifstream in_file_stream(new_relative_in_file.c_str());
        string line;
        while (getline(in_file_stream, line)) {
            // Обрабатываем директиву #include
            smatch match;
            static regex rgxQuote(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
            static regex rgxAngle(R"/(\s*#\s*include\s*<([^>]*)>\s*)/");

            if (regex_search(line, match, rgxQuote) || regex_search(line, match, rgxAngle)) {
                path p(relative_file_name);
                path new_new_relative_in_file = parent_pth / (p.parent_path()).lexically_normal();
                Preprocess(in_file, out_file, match[1].str(), new_new_relative_in_file, include_directories, line_number, in_file_name);
            } else {
                out_file << line << endl;
            }
        }
    } else {
            bool file_found = false;
            for (const auto& incl_pth : include_directories) {
                path p(incl_pth / relative_file_name);
                if (exists(p)) {
                    file_found = true;
                    Preprocess(in_file, out_file, relative_file_name, incl_pth, include_directories, line_number, in_file_name);
                    break;
                }
            }
            if (file_found == false) {
                cout << "unknown include file "s << relative_file_name << " at file "s << in_file_name << " at line " << line_number << endl;
                return false;
            }
        }
    return true;
}

// напишите эту функцию
bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) {
    // Отрываем входной файл для чтения
    ifstream in_file_stream(in_file.c_str());
    if (!in_file_stream) {
        return false;
    }
    
    path p = in_file.parent_path();

    // Создаем выходной файл для записи
    ofstream out_file_stream(out_file.c_str(), ios::app);
    if (!out_file_stream) {
        return false;
    }

    string line;
    int line_number = 0;
    while (getline(in_file_stream, line)) {
        ++line_number;
        // Обрабатываем директиву #include
        smatch match;
        static regex rgxQuote(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
        static regex rgxAngle(R"/(\s*#\s*include\s*<([^>]*)>\s*)/");
        if (regex_search(line, match, rgxQuote) || regex_search(line, match, rgxAngle)) {
            if (!Preprocess(in_file_stream, out_file_stream, match[1].str(), p, include_directories, line_number, in_file.string())) {
                return false;
            }
        } else {
            out_file_stream << line << endl;
        }
    }
    return true;
}

string GetFileContents(string file) {
    ifstream stream(file);

    // конструируем string по двум итераторам
    return {(istreambuf_iterator<char>(stream)), istreambuf_iterator<char>()};
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
                "#include \"dir1/b.h\"\n"
                "// text between b.h and c.h\n"
                "#include \"dir1/d.h\"\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"
                "#   include<dummy.txt>\n"
                "}\n"s;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
                "#include \"subdir/c.h\"\n"
                "// text from b.h after include"s;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
                "#include <std1.h>\n"
                "// text from c.h after include\n"s;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
                "#include \"lib/std2.h\"\n"
                "// text from d.h after include\n"s;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"s;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"s;
    }

    /* Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
                                  {"sources"_p / "include1"_p,"sources"_p / "include2"_p}); */
    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
                                  {"sources"_p / "include1"_p,"sources"_p / "include2"_p})));

    ostringstream test_out;
    test_out << "// this comment before include\n"
                "// text from b.h before include\n"
                "// text from c.h before include\n"
                "// std1\n"
                "// text from c.h after include\n"
                "// text from b.h after include\n"
                "// text between b.h and c.h\n"
                "// text from d.h before include\n"
                "// std2\n"
                "// text from d.h after include\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}