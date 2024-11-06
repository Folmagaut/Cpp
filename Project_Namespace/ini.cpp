#include "ini.h"

using namespace std;

// место для реализаций методов и функций библиотеки ini
// не забудьте, что они должны быть помещены в namespace ini
namespace ini {

Section& Document::AddSection(std::string name) {
    return sections_[name];
}

const Section& Document::GetSection(const std::string& name) const {
    if (sections_.count(name) && sections_.at(name).size() != 0) {
        return sections_.at(name);
    }
    static Section tmp = {};
    return tmp;
}

std::size_t Document::GetSectionCount() const {
    return sections_.size();
}

void DeleteSpaces(string& line) {
	if (!line.empty()) {
        size_t length = line.find_last_not_of(' ') - line.find_first_not_of(' ') + 1;
        line = line.substr(line.find_first_not_of(' '), length);
    }
}

Document Load(istream& input) {
	Document doc;
	string section_name;
	string line;

	while (getline(input, line)) {
		Section section;
		DeleteSpaces(line);
		if (line.empty()) {
			continue;
		}
		if (line[0] == '[') {
			section_name = line.substr(1, line.find(']') - 1);
			section = doc.AddSection(section_name);
		}
		else if (line.find('=') && !line.empty()) {
			string section_key = line.substr(0, line.find_first_of('='));
			string section_value = line.substr(line.find_first_of('=') + 1, line.size() - line.find_first_of('='));
			DeleteSpaces(section_key);
			DeleteSpaces(section_value);
			doc.AddSection(section_name).insert({section_key, section_value});
		}
	}
	return doc;
}
}