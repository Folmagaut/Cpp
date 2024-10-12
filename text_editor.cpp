#include <iostream>
#include <list>
#include <string>

using namespace std;

//template<typename Iterator>
class Editor {
public:
    Editor() = default;
    // сдвинуть курсор влево
    void Left() {
        if (text_.size() > 0 && it_pos_ != text_.begin()) {
            --it_pos_;
        }
    }
    // сдвинуть курсор вправо 
    void Right() {
        if (text_.size() > 0 && it_pos_ != text_.end()) {
            ++it_pos_;
        }
    }
    // вставить символ token
    void Insert(char token) {
        text_.insert(it_pos_, token);
    }
    // вырезать не более tokens символов, начиная с текущей позиции курсора
    void Cut(size_t tokens = 1) {
        ClearBuffer();
        size_t dst = min(static_cast<size_t>(distance(it_pos_, text_.end())), tokens);
        auto it_end = it_pos_;
        for (size_t it = 0; it < dst; ++it) {
            ++it_end;
        }
        buffer_.splice(buffer_.begin(), text_, it_pos_, it_end);
        it_pos_ = it_end;
    }
    // cкопировать не более tokens символов, начиная с текущей позиции курсора
    void Copy(size_t tokens = 1) {
        ClearBuffer();
        size_t dst = min(static_cast<size_t>(distance(it_pos_, text_.end())), tokens);
        auto it_end = it_pos_;
        for (size_t it = 0; it < dst; ++it) {
            ++it_end;
        }
        buffer_.assign(it_pos_, it_end);
    }
    // вставить содержимое буфера в текущую позицию курсора
    void Paste() {
        text_.insert(it_pos_, buffer_.begin(), buffer_.end());
    }
    // получить текущее содержимое текстового редактора
    string GetText() const {
        return string(text_.begin(), text_.end());
    }

    void ClearBuffer() {
        buffer_.erase(buffer_.begin(), buffer_.end());
    }

private:
    list<char> text_ = {};
    list<char> buffer_ = {};
    list<char>::iterator it_pos_ = text_.begin();
};

int main() {
    Editor editor;
    const string text = "hello, world"s;
    for (char c : text) {
        editor.Insert(c);
    }
    // Текущее состояние редактора: `hello, world|`
    for (size_t i = 0; i < text.size(); ++i) {
        editor.Left();
    }
    // Текущее состояние редактора: `|hello, world`
    editor.Cut(7);
    // Текущее состояние редактора: `|world`
    // в буфере обмена находится текст `hello, `
    for (size_t i = 0; i < 5; ++i) {
        editor.Right();
    }
    // Текущее состояние редактора: `world|`
    editor.Insert(',');
    editor.Insert(' ');
    // Текущее состояние редактора: `world, |`
    editor.Paste();
    // Текущее состояние редактора: `world, hello, |`
    editor.Left();
    editor.Left();
    //Текущее состояние редактора: `world, hello|, `
    editor.Cut(3);  // Будут вырезаны 2 символа
    // Текущее состояние редактора: `world, hello|`
    cout << editor.GetText();
    return 0;
}

/*
//авторское решение
#include <iostream>
#include <list>
#include <string>



using namespace std;

class Editor {
public:
    Editor()
            : pos(text.end()) {
    }

    void Left() {
        pos = Advance(pos, -1);
    }

    void Right() {
        pos = Advance(pos, 1);
    }

    void Insert(char token) {
        text.insert(pos, token);
    }

    void Cut(size_t tokens = 1) {
        auto pos2 = Advance(pos, tokens);
        buffer.assign(pos, pos2);
        pos = text.erase(pos, pos2);
    }

    void Copy(size_t tokens = 1) {
        auto pos2 = Advance(pos, tokens);
        buffer.assign(pos, pos2);
    }

    void Paste() {
        text.insert(pos, buffer.begin(), buffer.end());
    }

    string GetText() const {
        return {text.begin(), text.end()};
    }

private:
    using Iterator = list<char>::iterator;
    list<char> text;
    list<char> buffer;
    Iterator pos;

    Iterator Advance(Iterator it, int steps) const {
        while (steps > 0 && it != text.end()) {
            ++it;
            --steps;
        }
        while (steps < 0 && it != text.begin()) {
            --it;
            ++steps;
        }
        return it;
    }
};
*/