
/*



// отели /////////////////////////////////////////////////////
#include <iostream>
#include <cstdint>
#include <deque>
#include <set>
#include <string>

using namespace std;

struct BookingInfo {
    int64_t time_;
    string hotel_name_;
    int client_id_;
    int room_count_;
};

class HotelManager {
public:
    void Book(int64_t time, const string& hotel_name, int client_id, int room_count) {
        BookingInfo bif = {time, hotel_name, client_id, room_count};
        dobi_.push_back(move(bif));
        current_time_ = time;
    }

    int ComputeClientCount(const string& hotel_name) {
        BIFCounter();

        set<int> clients_cnt;

        for (const auto& bif : dobi_) {
            if (bif.hotel_name_ == hotel_name) {
                clients_cnt.insert(bif.client_id_);
            }
        }
        return static_cast<int>(clients_cnt.size());
    }

    int ComputeRoomCount(const string& hotel_name) {
        BIFCounter();
        int room_cnt = 0;

        for (const auto& bif : dobi_) {
            if (bif.hotel_name_ == hotel_name) {
                room_cnt += bif.room_count_;
            }
        }
        return room_cnt;
    }

private:
    int64_t current_time_ = 0;
    deque<BookingInfo> dobi_;

    void BIFCounter() {
        for (auto& bif : dobi_) {
            if (bif.time_ < current_time_ - 86399) {
                dobi_.pop_front();
            }
        }
    }
};

int main() {
    HotelManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;

        if (query_type == "BOOK") {
            int64_t time;
            cin >> time;
            string hotel_name;
            cin >> hotel_name;
            int client_id, room_count;
            cin >> client_id >> room_count;
            manager.Book(time, hotel_name, client_id, room_count);
        } else {
            string hotel_name;
            cin >> hotel_name;
            if (query_type == "CLIENTS") {
                cout << manager.ComputeClientCount(hotel_name) << "\n";
            } else if (query_type == "ROOMS") {
                cout << manager.ComputeRoomCount(hotel_name) << "\n";
            }
        }
    }

    return 0;
}

// unique_ptr//////////////////////////////////////////////////
#include <cassert>
#include <cstddef>  // нужно для nullptr_t
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr {
private:
    T* ptr_ = nullptr;
public:
    UniquePtr() = default;

    explicit UniquePtr(T* ptr) : ptr_(ptr) {
    }

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) { 
        other.ptr_ = nullptr; 
    }

    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr& operator=(nullptr_t) {
        delete ptr_;
        ptr_ = nullptr;
        return *this;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept { 
        if (this != &other) { 
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr; 
        } 
        return *this; 
    }

    ~UniquePtr() {
        delete ptr_;
    }

    T& operator*() const {
        if (!ptr_) {
            throw logic_error("Error: ptr_ is null"s);
        }
        return *ptr_;
    }

    T* operator->() const {
        if (!ptr_) {
            throw logic_error("Error: ptr_ is null"s);
        }
        return ptr_;
    }

    T* Release() {
        T* ret_ptr = ptr_;
        ptr_ = nullptr;
        return ret_ptr;
    }

    void Reset(T* ptr) {
        delete ptr_;
        ptr_ = ptr;
    }

    void Swap(UniquePtr& other) noexcept {
        std::swap(other.ptr_, ptr_);
    }

    T* Get() const {
        return ptr_;
    }
};

struct Item {
    static int counter;
    int value;
    Item(int v = 0)
        : value(v) 
    {
        ++counter;
    }
    Item(const Item& other)
        : value(other.value) 
    {
        ++counter;
    }
    ~Item() {
        --counter;
    }
};

int Item::counter = 0;

void TestLifetime() {
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        ptr.Reset(new Item);
        assert(Item::counter == 1);
    }
    assert(Item::counter == 0);

    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        auto rawPtr = ptr.Release();
        assert(Item::counter == 1);

        delete rawPtr;
        assert(Item::counter == 0);
    }
    assert(Item::counter == 0);
}

void TestGetters() {
    UniquePtr<Item> ptr(new Item(42));
    assert(ptr.Get()->value == 42);
    assert((*ptr).value == 42);
    assert(ptr->value == 42);
}

int main() {
    TestLifetime();
    TestGetters();
    std::cout << "Ok"s << std::endl;
}

//////////////////////////////////////////////
// тема с мьютексами
#include <cassert>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>

using namespace std;

template <typename T>
class LazyValue {
public:
    explicit LazyValue(function<T()> init) : init_func_(move(init)) {
    }

    bool HasValue() const {
        //lock_guard<mutex> lock(mutex_);
        return value_ != nullopt; 
    }

    const T& Get() const {
        lock_guard<mutex> lock(mutex_);
        if (!value_) {
            value_ = init_func_();
        }
        return value_.value();
    }

private:
    function<T()> init_func_;
    mutable optional<T> value_;
    mutable mutex mutex_;
};

void UseExample() {
    const string big_string = "Giant amounts of memory"s;

    LazyValue<string> lazy_string([&big_string] {
        return big_string;
    });

    assert(!lazy_string.HasValue());
    assert(lazy_string.Get() == big_string);
    assert(lazy_string.Get() == big_string);
}

void TestInitializerIsntCalled() {
    bool called = false;

    {
        LazyValue<int> lazy_int([&called] {
            called = true;
            return 0;
        });
    }
    assert(!called);
}

int main() {
    UseExample();
    TestInitializerIsntCalled();
    //cout << "Ok"s << endl;
}

///////////////////////////////////////////////////
// подбор паролей с mutable кэш-словарём
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

template <typename Checker>
class PasswordCracker {
public:
    PasswordCracker(Checker check, string key)
        : check_(check)
        , curr_check_key_(move(key))
        , cached_passwords_()
    {
    }
    void SetCheck(Checker check, string key) {
        check_ = check;
        curr_check_key_ = move(key);
    }
    string BruteForce() const {
        // если готовый пароль есть в словаре, вернём его
        auto result = cached_passwords_.find(curr_check_key_);
        if (cached_passwords_.end() != result) {
            return result->second;
        }
        // если готового пароля нет, перейдём к подбору
        auto pass = BruteForceInternal(""s, 0);
        // не забудем добавить новый пароль в словарь готовых паролей
        cached_passwords_[move(curr_check_key_)] = pass;
        return pass;
    }
private:
    string BruteForceInternal(const string& begin, int n) const {
        if (n == 5) {
            return check_(begin) ? begin : ""s;
        }
        for (char c = 'A'; c != 'Z' + 1; ++c) {
            string res = BruteForceInternal(begin + string({c}), n + 1);
            if (!res.empty()) {
                return res;
            }
        }
        return {};
    }
    Checker check_;
    string curr_check_key_;
    //словарь для кеширования найденных паролей
    mutable unordered_map<string, string> cached_passwords_;
};

class PasswordChecker {
public:
    PasswordChecker(string match)
        : match_(match)
    {
    }
    bool operator()(const string& s) const {
        return s == match_;
    }
    void operator=(const PasswordChecker& other) {
        match_ = other.match_;
    }

private:
    string match_;
};

string GetShmandexPass(const PasswordCracker<PasswordChecker>& pass_cracker) {
    // если раскомментировать код ниже, программа не скомпилируется
    // PasswordChecker.check("BUY"s);
    // pass_cracker.SetCheck(check);
    return pass_cracker.BruteForce();
}

int main() {
    PasswordChecker check("HELLO"s);
    string key = "1"s;
    PasswordCracker password_cracker(check, key);
    cout << GetShmandexPass(password_cracker) << endl;
    cout << GetShmandexPass(password_cracker) << endl;
}

//////////////////////////////////////////////////////
// моё Константность объектов
#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#include "log_duration.h"

using namespace std;

struct Person {
    string name;
    int age, income;
    bool is_male;
};

vector<Person> ReadPeople(istream& input) {
    int count;
    input >> count;

    vector<Person> result(count);
    for (Person& p : result) {
        char gender;
        input >> p.name >> p.age >> p.income >> gender;
        p.is_male = gender == 'M';
    }
    return result;
}

string MostPopularName(const map<string, size_t>& names) {
    pair<string, size_t> most_popular_name = {""s, 0};
    for (const auto& name : names) {
        if (name.second > most_popular_name.second) {
            most_popular_name = name;
        }
    }
    return most_popular_name.first;
}

int main() {
    vector<Person> people = ReadPeople(cin);

{
    LOG_DURATION("Time = "s);
    map<string, size_t> male_names;
    map<string, size_t> fem_names;
    for (const auto& person : people) {
        if (person.is_male) {
            male_names[person.name] += 1;
        } else {
            fem_names[person.name] += 1;
        }
    }

    vector<size_t> wealth(people.size());
    size_t wealth_counter = 0;
    size_t income = 0;
    sort(people.begin(), people.end(), 
                [](const Person& lhs, const Person& rhs) {
                return lhs.income > rhs.income;
                });
    for (const auto& person : people) {
        income += person.income;
        wealth[wealth_counter] = income;
        ++wealth_counter;
    }

    sort(people.begin(), people.end(), 
                [](const Person& lhs, const Person& rhs) {
                return lhs.age < rhs.age;
                });

    for (string command; cin >> command;) {
        
        if (command == "AGE"s) {
            int adult_age;
            cin >> adult_age;

            auto adult_begin = lower_bound(people.begin(), people.end(), adult_age, 
            	[](const Person& lhs, int age) {
                return lhs.age < age;
            });

            cout << "There are "s << distance(adult_begin, people.end()) << " adult people for maturity age "s
                 << adult_age << '\n';
        } else if (command == "WEALTHY"s) {
            int count;
            cin >> count;
            int total_income = 0;
            if (count > 0) {
                total_income = wealth[min(count - 1, static_cast<int>(people.size() - 1))];
            }
            cout << "Top-"s << count << " people have total income "s << total_income << '\n';
        } else if (command == "POPULAR_NAME"s) {
            char gender;
            cin >> gender;
            string most_popular_name;
            if (gender == 'M') {
                most_popular_name = MostPopularName(male_names);
            } else {
                most_popular_name = MostPopularName(fem_names);
            }
            cout << "Most popular name among people of gender "s << gender << " is "s << most_popular_name << '\n';
        }
    }
}
}

///////////////////////////////////////////////////////
// авторское
#include <algorithm>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>
#include "log_duration.h"
using namespace std;

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
            : first(begin)
            , last(end) {
    }

    Iterator begin() const {
        return first;
    }

    Iterator end() const {
        return last;
    }

private:
    Iterator first, last;
};

struct Person {
    string name;
    int age, income;
    bool is_male;
};

vector<Person> ReadPeople(istream& input) {
    int count;
    input >> count;

    vector<Person> result(count);
    for (Person& p : result) {
        char gender;
        input >> p.name >> p.age >> p.income >> gender;
        p.is_male = gender == 'M';
    }

    return result;
}

template <typename Iter>
optional<string> FindMostPopularName(IteratorRange<Iter> range) {
    if (range.begin() == range.end()) {
        return nullopt;
    } else {
        sort(range.begin(), range.end(), [](const Person& lhs, const Person& rhs) {
            return lhs.name < rhs.name;
        });
        const string* most_popular_name = &range.begin()->name;
        int count = 1;
        for (auto i = range.begin(); i != range.end();) {
            auto same_name_end = find_if_not(i, range.end(), [i](const Person& p) {
                return p.name == i->name;
            });
            const auto cur_name_count = distance(i, same_name_end);
            if (cur_name_count > count || (cur_name_count == count && i->name < *most_popular_name)) {
                count = cur_name_count;
                most_popular_name = &i->name;
            }
            i = same_name_end;
        }
        return *most_popular_name;
    }
}

struct StatsData {
    optional<string> most_popular_male_name;
    optional<string> most_popular_female_name;
    vector<int> cumulative_wealth;
    vector<Person> sorted_by_age;
};

void InitMostPopularNames(StatsData& stat_data, vector<Person>& people) {
    IteratorRange males{people.begin(), partition(people.begin(), people.end(),
                                                  [](const Person& p) {
                                                      return p.is_male;
                                                  })};
    IteratorRange females{males.end(), people.end()};

    // По мере обработки запросов список людей не меняется, так что мы можем
    // один раз найти самые популярные женское и мужское имена
    stat_data.most_popular_male_name = FindMostPopularName(males);
    stat_data.most_popular_female_name = FindMostPopularName(females);
}

void InitWealth(StatsData& stat_data, vector<Person>& people) {
    // Запросы WEALTHY можно тоже обрабатывать за О(1), один раз отсортировав всех
    // людей по достатку и посчитав массив префиксных сумм
    sort(people.begin(), people.end(),
         [](const Person& lhs, const Person& rhs) {
             return lhs.income > rhs.income;
         });

    auto& wealth = stat_data.cumulative_wealth;
    wealth.resize(people.size());

    if (!people.empty()) {
        wealth[0] = people[0].income;
        for (size_t i = 1; i < people.size(); ++i) {
            wealth[i] = wealth[i - 1] + people[i].income;
        }
    }
}

void InitSortedByAge(StatsData& stat_data, vector<Person> people) {
    sort(people.begin(), people.end(),
         [](const Person& lhs, const Person& rhs) {
             return lhs.age < rhs.age;
         });
    stat_data.sorted_by_age = move(people);
}

StatsData BuildStatsData(vector<Person> people) {
    StatsData result;

    InitMostPopularNames(result, people);
    InitWealth(result, people);
    InitSortedByAge(result, people);

    return result;
}

void ProceedAgeCommand(const StatsData& stat_data) {
    int adult_age;
    cin >> adult_age;

    auto adult_begin = lower_bound(stat_data.sorted_by_age.begin(), stat_data.sorted_by_age.end(), adult_age,
                                   [](const Person& lhs, int age) {
                                       return lhs.age < age;
                                   });

    cout << "There are "s << distance(adult_begin, stat_data.sorted_by_age.end())
         << " adult people for maturity age "s << adult_age << '\n';
}

void ProceedWealthyCommand(const StatsData& stat_data) {
    int count;
    cin >> count;
    cout << "Top-"s << count << " people have total income "s << stat_data.cumulative_wealth[count - 1] << '\n';
}

void ProceedPopularNameCommand(const StatsData& stat_data) {
    char gender;
    cin >> gender;
    const auto& most_popular_name
            = gender == 'M' ? stat_data.most_popular_male_name : stat_data.most_popular_female_name;
    if (most_popular_name) {
        cout << "Most popular name among people of gender "s << gender << " is "s << *most_popular_name << '\n';
    } else {
        cout << "No people of gender "s << gender << '\n';
    }
}

int main() {
    // Основной проблемой исходного решения было то, что в нём случайно изменялись
    // входные данные. Чтобы гарантировать, что этого не произойдёт, мы организовываем код
    // так, чтобы в месте обработки запросов были видны только константные данные.
    //
    // Для этого всю их предобработку мы вынесли в отдельную функцию, результат которой
    // сохраняем в константной переменной.
    {
    LOG_DURATION("Time = "s);
    const StatsData stats = BuildStatsData(ReadPeople(cin));

    for (string command; cin >> command;) {
        if (command == "AGE"s) {
            ProceedAgeCommand(stats);
        } else if (command == "WEALTHY"s) {
            ProceedWealthyCommand(stats);
        } else if (command == "POPULAR_NAME"s) {
            ProceedPopularNameCommand(stats);
        }
    }
    }
}

/////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

class Speakable {
public:
    virtual ~Speakable() = default;
    virtual void Speak(ostream& out) const = 0;
};

class Drawable {
public:
    virtual ~Drawable() = default;
    virtual void Draw(ostream& out) const = 0;
};

class Animal {
public:
    virtual ~Animal() = default;
    void Eat(string_view food) {
        cout << GetType() << " is eating "sv << food << endl;
        ++energy_;
    }
    virtual string GetType() const = 0;

private:
    int energy_ = 100;
};

class Fish : public Animal, public Drawable {
public:
    string GetType() const override {
        return "fish"s;
    }
    void Draw(ostream& out) const override {
        out << "><(((*>"sv << endl;
    }
};

class Cat : public Animal, public Speakable, public Drawable {
public:
    void Speak(ostream& out) const override {
        out << "Meow-meow"sv << endl;
    }
    void Draw(ostream& out) const override {
        out << "(^w^)"sv << endl;
    }
    string GetType() const override {
        return "cat"s;
    }
};

// Рисует животных, которых можно нарисовать
void DrawAnimals(const std::vector<const Animal*>& animals, ostream& out) {
    for (const auto& animal : animals) {
        const Drawable* ptr = dynamic_cast<const Drawable*>(animal);
        if (ptr != nullptr) {
            ptr->Draw(out);
        }
    }
}

// Побеседовать с животными, которые умеют разговаривать
void TalkToAnimals(const std::vector<const Animal*> animals, ostream& out) {
    for (const auto& animal : animals) {
        const Speakable* ptr = dynamic_cast<const Speakable*>(animal);
        if (ptr != nullptr) {
            ptr->Speak(out);
        }
    }
}

void PlayWithAnimals(const std::vector<const Animal*> animals, ostream& out) {
    TalkToAnimals(animals, out);
    DrawAnimals(animals, out);
}

int main() {
    Cat cat;
    Fish fish;
    vector<const Animal*> animals{&cat, &fish};
    PlayWithAnimals(animals, cerr);
}

//////////////////////////////////////////////////
// Макрос _USE_MATH_DEFINES необходим, чтобы при подключении <cmath> была объявлена константа M_PI
#define _USE_MATH_DEFINES
// Макрос _USE_MATH_DEFINES следует объявить ДО подключения других заголовочных файлов,
// которые могут подключить <cmath> неявно
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>

using namespace std;

enum class Color { RED, GREEN, BLUE };

ostream& operator<<(ostream& out, Color color) {
    switch (color) {
        case Color::RED:
            out << "red"s;
            break;
        case Color::GREEN:
            out << "green"s;
            break;
        case Color::BLUE:
            out << "blue"s;
            break;
    }
    return out;
}

class Shape {
public:
    Shape(Color color) : color_(color) {
    }
    Color GetColor() const {
        return color_;
    }
    void SetColor(Color color) {
        color_ = color;
    }
    virtual double GetArea() const {
        return area_;
    }
    virtual string GetType() const {
        return "Shape"s;
    }
private:
    Color color_;
    double area_ = 0.0;
};

class Rectangle : public Shape {
public:
    Rectangle(double width, double height, Color color)
        : Shape(color)
        , width_(width)
        , height_(height) {
    }

    double GetWidth() const {
        return width_;
    }
    double GetHeight() const {
        return height_;
    }
    void SetSize(double width, double height) { 
        width_ = width;
        height_ = height;
    }
    double GetArea() const override {
        return width_ * height_;
    }
    string GetType() const override {
        return "Rectangle"s;
    }

private:
    double width_;
    double height_;
};

class Circle : public Shape {
public:
    Circle(double radius, Color color) 
        : Shape(color)
        , radius_(radius) {
    }

    double GetRadius() const {
        return radius_;
    }
    void SetRadius(double radius) {
        radius_ = radius;
    }
    double GetArea() const override {
        return (M_PI * pow(radius_, 2));
    }
    string GetType() const override {
        return "Circle"s;
    }

private:
    double radius_;
};

// Возвращает суммарную площадь фигур, указатели на которые находятся в переданной коллекции collection
template <typename ShapeCollection>
double CalcSumArea(const ShapeCollection& collection) {
    double sum = 0.0;
    for (const Shape* shape : collection) {
        sum += shape->GetArea();
    }
    return sum;
}

void PrintShapeInfo(const Shape& shape) {
    cout << shape.GetType() << ": color: "s << shape.GetColor() << ", area:"s << shape.GetArea() << endl;
}

int main() {
    Circle c{10.0, Color::RED};
    Rectangle r{10, 20, Color::BLUE};
    Shape sh{Color::GREEN};

    const Shape* shapes[] = {&c, &r, &sh};

    assert(sh.GetType() == "Shape"s);
    assert(c.GetType() == "Circle"s);
    assert(r.GetType() == "Rectangle"s);

    assert(sh.GetColor() == Color::GREEN);
    assert(c.GetColor() == Color::RED);
    assert(r.GetColor() == Color::BLUE);
    sh.SetColor(Color::BLUE);
    c.SetColor(Color::GREEN);
    r.SetColor(Color::RED);
    assert(sh.GetColor() == Color::BLUE);
    assert(c.GetColor() == Color::GREEN);
    assert(r.GetColor() == Color::RED);

    assert(std::abs(r.GetArea() - 200.0) < 1e-5);
    assert(std::abs(c.GetArea() - 314.15) < 1e-2);
    c.SetRadius(1.0);
    assert(std::abs(c.GetArea() - 3.1415) < 1e-4);
    r.SetSize(5, 7);
    assert(r.GetWidth() == 5);
    assert(r.GetHeight() == 7);
    assert(std::abs(r.GetArea() - 35.0) < 1e-5);

    assert(abs(CalcSumArea(shapes) - 38.1416) < 1e-4);

    for (const Shape* shape : shapes) {
        PrintShapeInfo(*shape);
    }
    cout << "Total area: " << CalcSumArea(shapes) << endl;
}

/////////////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <string>

using namespace std;

class House {
public:
    House(int length, int width, int height)
    : length_(length), width_(width), height_(height) {
    }

    int GetLength() const {
        return length_;
    }
    int GetWidth() const {
        return width_;
    }
    int GetHeight() const {
        return height_;
    }

private:
    int length_ = 0;
    int width_ = 0;
    int height_ = 0;
};

class Resources {
public:
    //Resources() = default;

    Resources(int brick_count) : brick_count_(brick_count) {
    }

    void TakeBricks(int count) {
        if (count < 0) {
            throw out_of_range("Out of range"s);
        }
        if (count <= brick_count_) {
            brick_count_ -= count;
        } else {
            throw out_of_range("Out of range"s);
        }
    }

    int GetBrickCount() const {
        return brick_count_;
    }

private:
    int brick_count_ = 0;
};

struct HouseSpecification {
    int length = 0;
    int width = 0;
    int height = 0;
};

class Builder {
public:
    Builder(Resources& resources) : resources_(resources) {
    }

    House BuildHouse(const HouseSpecification& spec) {
        int all_res = (spec.length * 2 * spec.height + spec.width * 2 * spec.height) * 32;
        if (resources_.GetBrickCount() < all_res) {
            throw runtime_error("Runtime error");
        }
        resources_.TakeBricks(all_res);
        House house(spec.length, spec.width, spec.height);
        return house;
    }

private:
    Resources& resources_;
    //HouseSpecification spec_;
};

int main() {
    Resources resources{10000};
    Builder builder1{resources};
    Builder builder2{resources};

    House house1 = builder1.BuildHouse(HouseSpecification{12, 9, 3});
    assert(house1.GetLength() == 12);
    assert(house1.GetWidth() == 9);
    assert(house1.GetHeight() == 3);
    cout << resources.GetBrickCount() << " bricks left"s << endl;

    House house2 = builder2.BuildHouse(HouseSpecification{8, 6, 3});
    assert(house2.GetLength() == 8);
    cout << resources.GetBrickCount() << " bricks left"s << endl;

    House banya = builder1.BuildHouse(HouseSpecification{4, 3, 2});
    assert(banya.GetHeight() == 2);
    cout << resources.GetBrickCount() << " bricks left"s << endl;
}

///////////////////////////////////////
#include <cstdint>
#include <climits>
#include <iostream>

using namespace std;

int main() {
    int64_t a;
    int64_t b;
    cin >> a >> b;
    if ((a > 0 && b > 0 && a > LLONG_MAX - b)) {
        cout << "Overflow!"s << endl;
    } else if (a < 0 && b < 0 && a < LLONG_MIN - b) {
        cout << "Overflow!"s << endl;
    } else {
        cout << a + b << endl;
    }  
}

//////////////////////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std::literals;

template <typename Key, typename Value, typename ValueFactoryFn>
class Cache {
public:
    // Создаёт кеш, инициализированный фабрикой значений, созданной по умолчанию
    Cache() = default;

    // Создаёт кеш, инициализированный объетом, играющим роль фабрики,
    // создающей (при помощи operator() значение по его ключу)
    // Фабрика должна вернуть shared_ptr<Value> либо unique_ptr<Value>.
    // Пример использования:
    // shared_ptr<Value> value = value_factory(key);
    explicit Cache(ValueFactoryFn value_factory) : value_factory_(std::move(value_factory)) {
        // Реализуйте конструктор самостоятельно
    }

    // Возвращает закешированное значение по ключу. Если значение отсутствует или уже удалено,
    // оно должно быть создано с помощью фабрики и сохранено в кеше.
    // Если на объект нет внешних ссылок, он должен быть удалён из кеша
    std::shared_ptr<Value> GetValue(const Key& key) {
        //(void) key;
        // Заглушка. Реализуйте метод самостоятельно
        auto it = cache_.find(key);
        if (it != cache_.end() && !it->second.expired()) {
            return it->second.lock();
        }

        auto value = value_factory_(key);
        cache_[key] = value;
        return value;
    }

    private:
    std::unordered_map<Key, std::weak_ptr<Value>> cache_;
    ValueFactoryFn value_factory_;
};

// Пример объекта, находящегося в кеше
class Object {
public:
    explicit Object(std::string id)
        : id_(std::move(id))  //
    {
        using namespace std;
        cout << "Object '"sv << id_ << "' has been created"sv << endl;
    }

    const std::string& GetId() const {
        return id_;
    }

    ~Object() {
        using namespace std;
        cout << "Object '"sv << id_ << "' has been destroyed"sv << endl;
    }

private:
    std::string id_;
};

using ObjectPtr = std::shared_ptr<Object>;

struct ObjectFactory {
    ObjectPtr operator()(std::string id) const {
        return std::make_shared<Object>(std::move(id));
    }
};

void Test1() {
    using namespace std;
    ObjectPtr alice1;
    // Кеш, объектов Object, создаваемых при помощи ObjectFactory,
    // доступ к которым предоставляется по ключу типа string
    Cache<string, Object, ObjectFactory> cache;

    // Извлекаем объекты Alice и Bob
    alice1 = cache.GetValue("Alice"s);
    auto bob = cache.GetValue("Bob"s);
    // Должны вернуться два разных объекта с правильными id
    assert(alice1 != bob);
    assert(alice1->GetId() == "Alice"s);
    assert(bob->GetId() == "Bob"s);

    // Повторный запрос объекта Alice должен вернуть существующий объект
    auto alice2 = cache.GetValue("Alice"s);
    assert(alice1 == alice2);

    // Указатель alice_wp следит за жизнью объекта Alice
    weak_ptr alice_wp{alice1};
    alice1.reset();
    assert(!alice_wp.expired());
    cout << "---"sv << endl;
    alice2.reset();
    // Объект Alice будет удалён, так как на него больше не ссылаются shared_ptr
    assert(alice_wp.expired());
    cout << "---"sv << endl;
    // Объект Bob будет удалён после разрушения указателя bob

    alice1 = cache.GetValue("Alice"s);  // объект 'Alice' будет создан заново
    cout << "---"sv << endl;
}

struct Book {
    Book(std::string title, std::string content)
        : title(std::move(title))
        , content(std::move(content)) {
    }

    std::string title;
    std::string content;
};

// Функциональный объект, загружающий книги из unordered_map
class BookLoader {
public:
    using BookStore = std::unordered_map<std::string, std::string>;

    // Принимает константную ссылку на хранилище книг и ссылку на переменную-счётчик загрузок
    explicit BookLoader(const BookStore& store, size_t& load_count) : store_(store), load_count_(load_count) {
        // Реализуйте конструктор самостоятельно
    }

    // Загружает книгу из хранилища по её названию и возвращает указатель
    // В случае успешной загрузки (книга есть в хранилище)
    // нужно увеличить значения счётчика загрузок load_count, переданного в конструктор, на 1.
    // Если книга в хранилище отсутствует, нужно выбросить исключение std::out_of_range,
    // а счётчик не увеличивать
    std::shared_ptr<Book> operator()(const std::string& title) const {
        // Заглушка, реализуйте метод самостоятельно
        //(void) title;
        auto it = store_.find(title);
        if (it == store_.end()) {
            throw std::out_of_range("Not implemented"s);
        }
        ++load_count_;
        return std::make_shared<Book>(title, it->second);
    }

private:
    // Добавьте необходимые данные и/или методы
    const BookStore& store_;
    size_t& load_count_;
};

void Test2() {
    using namespace std;
    // Хранилище книг.
    BookLoader::BookStore books{
        {"Sherlock Holmes"s,
         "To Sherlock Holmes she is always the woman. I have seldom heard him mention her under any other name."s},
        {"Harry Potter"s, "Chapter 1. The boy who lived. ...."s},
    };
    using BookCache = Cache<string, Book, BookLoader>;

    size_t load_count = 0;
    // Создаём кеш, который будет использщовать BookLoader для загрузки книг из хранилища books
    BookCache book_cache{BookLoader{books, load_count}};

    // Загруженная книга должна содержать данные из хранилища
    auto book1 = book_cache.GetValue("Sherlock Holmes"s);
    assert(book1);
    assert(book1->title == "Sherlock Holmes"s);
    assert(book1->content == books.at(book1->title));
    assert(load_count == 1);

    // Повторный запрос книги должен возвращать закешированное значение
    auto book2 = book_cache.GetValue("Sherlock Holmes"s);
    assert(book2);
    assert(book1 == book2);
    assert(load_count == 1);

    weak_ptr<Book> weak_book{book1};
    assert(!weak_book.expired());
    book1.reset();
    book2.reset();
    // Книга удаляется, как только на неё перестают ссылаться указатели вне кеша
    assert(weak_book.expired());

    book1 = book_cache.GetValue("Sherlock Holmes"s);
    assert(load_count == 2);
    assert(book1);

    try {
        book_cache.GetValue("Fifty Shades of Grey"s);
        // BookLoader выбросит исключение при попытке загрузить несуществующую книгу
        assert(false);
    } catch (const std::out_of_range&) {
        // Всё нормально. Такой книги нет в книгохранилище
    } catch (...) {
        cout << "Unexpected exception"sv << endl;
    }
    // Счётчик загрузок не должен обновиться, так как книги нет в хранилище
    assert(load_count == 2);

    // Добавляем книгу в хранилище
    books["Fifty Shades of Grey"s] = "I scowl with frustration at myself in the mirror..."s;

    try {
        auto book = book_cache.GetValue("Fifty Shades of Grey"s);
        // Теперь книга должна быть успешно найдена
        assert(book->content == books.at("Fifty Shades of Grey"s));
    } catch (...) {
        assert(false);
    }
    // Счётчик загрузок должен обновиться, так как книга была загружена из хранилища
    assert(load_count == 3);
}

int main() {
    Test1();
    Test2();
}

////////////////////////////////////////////
#include <iostream>
#include <memory>
#include <string_view>

using namespace std;

struct Apartment;

struct Person {
    explicit Person(string name)
        : name(move(name))  //
    {
        cout << "Person "sv << this->name << " has been created"sv << endl;
    }
    ~Person() {
        cout << "Person "sv << name << " has died"sv << endl;
    }

    string name;
    shared_ptr<Apartment> apartment;
};

struct Apartment {
    Apartment() {
        cout << "The apartment has been created"sv << endl;
    }
    ~Apartment() {
        cout << "The apartment has been destroyed"sv << endl;
    }

    // Ссылка на Person автоматически обнулится, когда Person будет удалён.
    weak_ptr<Person> person;
};

void PrintApartmentInfo(const Apartment& apartment) {
    if (const auto person = apartment.person.lock()) {
        cout << person->name << " is living in the apartment" << endl;
    } else {
        cout << "The apartment is empty"sv << endl;
    }
}

int main() {
    auto apartment = make_shared<Apartment>();
    {
        auto person = make_shared<Person>("Ivan"s);
        person->apartment = apartment;
        apartment->person = person;
        cout << "----"sv << endl;
    }
    PrintApartmentInfo(*apartment);
}

//////////////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

template <typename Value>
class CoW {
    // Прокси-объект объявлен в приватной области. Поэтому его нельзя создать снаружи класса.
    struct WriteProxy {
        explicit WriteProxy(Value* value) noexcept
            : value_ptr_{value} {
        }

        // Прокси-объект нельзя копировать и присваивать.
        WriteProxy(const WriteProxy&) = delete;
        WriteProxy& operator=(const WriteProxy&) = delete;

        // У lvalue-ссылок операции разыменования нет.
        Value& operator*() const& = delete;
        // А у rvalue-ссылок разыменование есть.
        [[nodiscard]] Value& operator*() const&& noexcept {
            return *value_ptr_;
        }

        // Операции -> у lvalue-ссылок нет.
        Value* operator->() const& = delete;
        // У rvalue-ссылок операция -> есть.
        Value* operator->() const&& noexcept {
            return value_ptr_;
        }

    private:
        Value* value_ptr_;
    };
public:

    [[nodiscard]] WriteProxy Write() {
        EnsureUnique();

        // Возвращаем прокси-объект для модификации данных.
        return WriteProxy(value_.get());
    }
    
    // Конструируем значение по умолчанию.
    CoW()
        : value_(std::make_shared<Value>()) {
    }

    // Создаём значение за счёт перемещения его из value.
    CoW(Value&& value)
        : value_(std::make_shared<Value>(std::move(value))) {
    }

    // Создаём значение из value.
    CoW(const Value& value)
        : value_(std::make_shared<Value>(value)) {
    }

    // Оператор разыменования служит для чтения значения.
    const Value& operator*() const noexcept {
        assert(value_);
        return *value_;
    }

    // Оператор -> служит для чтения полей и вызова константных методов.
    const Value* operator->() const noexcept {
        assert(value_);
        return value_.get();
    }

    // Write принимает функцию, в которую CoW передаст неконстантную ссылку на хранящееся значение.
    //template <typename ModifierFn>
    //void Write(ModifierFn&& modify) {
    //    EnsureUnique();
    //    // Теперь value_ — единственный владелец данных.

    //    std::forward<ModifierFn>(modify)(*value_);
    //}
    // Value& Write() {
    //    EnsureUnique();

    //    return *value_;
    //} //- так не надо

private:
    std::shared_ptr<Value> value_;

    // Удостоверяемся, что текущий объект единолично владеет данными.
    // Если это не так, создаём копию и будем ссылаться на неё.
    void EnsureUnique() {
        assert(value_);

        if (value_.use_count() > 1) {
            // Кроме нас на value_ ссылается кто-то ещё, копируем содержимое value_.
            value_ = std::make_shared<Value>(*value_);
        }
    }
};

int main() {
    using namespace std::literals;

    CoW<std::string> s1("Hello");
    CoW<std::string> s2{s1};

    // Для доступа к значению используем операцию разыменования.
    std::cout << *s1 << ", "sv << *s2 << std::endl;

    // Для вызова константных методов служит стрелочка.
    std::cout << s1->size() << std::endl;

    // Оба указателя ссылаются на одну и ту же строку в памяти.
    assert(&*s1 == &*s2);
    std::cout << &*s1 << ", "sv << &*s2 << std::endl;

    //s2.Write([](auto& value) {
        // Внутри этой функции можно изменить значение, содержащееся в s2.
    //    value = "World"s;
    //    value += '!';
    //});

    // Теперь s2 содержит строку "World!".
    std::cout << *s1 << " "sv << *s2 << std::endl;
    std::cout << &*s1 << ", "sv << &*s2 << std::endl;

    // Чтобы изменить значение, нужно разыменовать результат вызова Write().
    *s2.Write() = "Wor";
    *s2.Write() += "ld";
    // Можно вызывать неконстантные методы, используя ->.
    s2.Write()->append("!");

    std::cout << *s1 << " "sv << *s2 << std::endl;
    std::cout << &*s1 << ", "sv << &*s2 << std::endl;
}

//////////////////////////////////////////////////////////
#include <string>
#include <iostream>
#include <memory>

using namespace std;

// Зарядное устройство
class Charger {
public:
    explicit Charger(string name)
        : name_(move(name)) {
        cout << "Charger "sv << name_ << " has been created"sv << endl;
    }
    ~Charger() {
        cout << "Charger "sv << name_ << " has been destroyed"sv << endl;
    }
private:
    string name_;
};

class Robot {
public:
    explicit Robot(string name)
        : name_(move(name)) {
        cout << "Robot "sv << name_ << " has been created"sv << endl;
    }
    ~Robot() {
        cout << name_ << " has been destroyed"sv << endl;
    }

    // Начать использовать зарядное устройство.
    void UseCharger(shared_ptr<Charger> charger) {
        charger_ = move(charger);
    }
    // Перестать владеть зарядным устройством.
    void ReleaseCharger() {
        charger_.reset();
    }

    const shared_ptr<Charger>& GetCharger() const {
        return charger_;
    }
private:
    string name_;
    shared_ptr<Charger> charger_;
};

int main() {
    Robot r2d2{"R2D2"s};
    Robot c3po{"C3PO"s};
    {
        auto charger1 = make_shared<Charger>("Charger1"s);
        auto charger2 = make_shared<Charger>("Charger2"s);
        auto charger3 = make_shared<Charger>("Charger3"s);

        r2d2.UseCharger(charger1);
        c3po.UseCharger(charger2);
        cout << "----"sv << endl;
    }
    cout << "----"sv << endl;
    cout << "C3PO uses R2D2's charger"sv << endl;
    c3po.UseCharger(r2d2.GetCharger());
    cout << "R2D2 releases its charger"sv << endl;
    r2d2.ReleaseCharger();
    cout << "----"sv << endl;
}

////////////////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <memory>

template <typename T>
struct TreeNode;

template <typename T>
using TreeNodePtr = std::unique_ptr<TreeNode<T>>;

template <typename T>
struct TreeNode {
    // Используйте TreeNodePtr<T> вместо сырых указателей.
    // Примите умные указатели по rvalue-ссылке.
    TreeNode(T val, TreeNodePtr<T>&& left, TreeNodePtr<T>&& right)
        : value(std::move(val))
        , left(std::move(left)) 
        , right(std::move(right)) {
    }

    T value;
    TreeNodePtr<T> left;  // Замените TreeNode* на TreeNodePtr<T>
    TreeNodePtr<T> right; // Замените TreeNode* на TreeNodePtr<T>

    // parent оставьте обычным указателем, иначе возникнет
    // кольцевая зависимость.
    TreeNode* parent = nullptr;
};

template <typename T>
bool CheckTreeProperty(const TreeNode<T>* node, const T* min, const T* max) noexcept {
    if (!node) {
        return true;
    }
    if ((min && node->value <= *min) || (max && node->value >= *max)) {
        return false;
    }
    return CheckTreeProperty(node->left.get(), min, &node->value)
           && CheckTreeProperty(node->right.get(), &node->value, max);
}

template <class T>
bool CheckTreeProperty(const TreeNode<T>* node) noexcept {
    return CheckTreeProperty<T>(node, nullptr, nullptr);
}

template <typename T>
TreeNode<T>* begin(TreeNode<T>* node) noexcept {
    while (node->left) {
        node = node->left.get();
    }

    return node;
}

template <typename T>
TreeNode<T>* next(TreeNode<T>* node) noexcept {
    if (node->right) {
        return begin(node->right.get());
    }
    while (node->parent) {
        bool is_right = (node == node->parent->right.get());
        if (!is_right) {
            return node->parent;
        }
        node = node->parent;
    }

    return nullptr;
}

// Замените указатели на умные. Сигнатура функции должна стать такой:
// TreeNodePtr<int> N(int val, TreeNodePtr<int>&& left = {}, TreeNodePtr<int>&& right = {})
TreeNodePtr<int> N(int val, TreeNodePtr<int>&& left = {}, TreeNodePtr<int>&& right = {}) {
    auto node = std::make_unique<TreeNode<int>>(val, std::move(left), std::move(right));
    if (node->left) {
        node->left->parent = node.get();
    }
    if (node->right) {
        node->right->parent = node.get();
    }

    return node;
}

int main() {
    using namespace std;
    using T = TreeNode<int>;
    auto root1 = N(6, N(4, N(3), N(5)), N(7));
    assert(CheckTreeProperty(root1.get()));

    T* iter = begin(root1.get());
    while (iter) {
        cout << iter->value << " "s;
        iter = next(iter);
    }
    cout << endl;

    auto root2 = N(6, N(4, N(3), N(5)), N(7, N(8)));
    assert(!CheckTreeProperty(root2.get()));

}

/////////////////////////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

struct Cat {
    Cat(const string& name, int age)
        : name_(name)
        , age_(age)  //
    {
    }

    //Cat(const Cat& other) {
    //    name_ = other.name_; //GetName();
    //    age_ = other.age_; // GetAge();
    //}

    const string& GetName() const noexcept {
        return name_;
    }
    int GetAge() const noexcept {
        return age_;
    }
    ~Cat() {
    }
    void Speak() const {
        cout << "Meow!"s << endl;
    }

private:
    string name_;
    int age_;
};

// Функция создаёт двухлетних котов
unique_ptr<Cat> CreateCat(const string& name) {
    return make_unique<Cat>(name, 2);
}

class Witch {
public:
    explicit Witch(const string& name)
        : name_(name) {
    }

    Witch(Witch&&) = default;

    Witch(const Witch& other) {
        name_ = other.name_; // GetName();
        if (other.cat_ != nullptr) { // ReleaseCat();
            cat_ = make_unique<Cat>(other.cat_.get()->GetName(), other.cat_.get()->GetAge());
        } else {
            cat_ = nullptr;
        }
    }

    Witch& operator=(Witch&&) = default;

    Witch& operator=(const Witch& other) {
        if (this != &other) {
            name_ = other.name_;
            if (other.cat_ != nullptr) {
                cat_ = make_unique<Cat>(other.cat_.get()->GetName(), other.cat_.get()->GetAge());
            }
            else {
                cat_ = nullptr;
            }
        }
        return *this;
    }

    const string& GetName() const noexcept {
        return name_;
    }
    void SetCat(unique_ptr<Cat>&& cat) noexcept {
        cat_ = std::move(cat);
    }
    unique_ptr<Cat> ReleaseCat() noexcept {
        return std::move(cat_);
    }

private:
    string name_;
    unique_ptr<Cat> cat_;
};

void Test() {
    // Объекты Witch можно перемещать
    {
        Witch witch("Hermione"s);
        auto cat = CreateCat("Crookshanks"s);
        Cat* raw_cat = cat.get();
        assert(raw_cat);
        witch.SetCat(move(cat));

        Witch moved_witch(std::move(witch));
        auto released_cat = moved_witch.ReleaseCat();
        assert(released_cat.get() == raw_cat);  // Кот переместился от witch к moved_witch
    }

    // Можно использовать перемещающий оператор присваивания
    {
        Witch witch("Hermione"s);
        auto cat = CreateCat("Crookshanks"s);
        Cat* raw_cat = cat.get();
        witch.SetCat(move(cat));

        Witch witch2("Minerva McGonagall");
        witch2 = move(witch);
        auto released_cat = witch.ReleaseCat();
        assert(!released_cat);
        released_cat = witch2.ReleaseCat();
        assert(released_cat.get() == raw_cat);
    }

    // Можно копировать волшебниц
    {
        Witch witch("Hermione");
        auto cat = CreateCat("Crookshanks"s);
        witch.SetCat(move(cat));

        Witch witch_copy(witch);
        assert(!cat);
        cat = witch.ReleaseCat();
        assert(cat);  // У первой волшебницы кот никуда не делся

        auto cat_copy = witch_copy.ReleaseCat();
        assert(cat_copy != nullptr && cat_copy != cat);
        assert(cat_copy->GetName() == cat->GetName());  // Копия волшебницы содержит копию кота
    }

    // Работает копирующее присваивание волшебниц
    {
        Witch witch("Hermione"s);
        auto cat = CreateCat("Crookshanks"s);
        witch.SetCat(move(cat));

        Witch witch2("Minerva McGonagall"s);
        witch2 = witch;

        assert(!cat);
        cat = witch.ReleaseCat();
        assert(cat);  // У первой волшебницы кот никуда не делся

        auto cat_copy = witch2.ReleaseCat();
        assert(cat_copy != nullptr && cat_copy != cat);
        assert(cat_copy->GetName() == cat->GetName());  // При присваивании скопировался кот
    }
}

int main() {
    Test();
}

///////////////////////////////////////////////////
#include <iostream>
#include <string>

using namespace std;

struct Circle {
    double x;
    double y;
    double r;
};

struct Dumbbell {
    Circle circle1;
    Circle circle2;
    string text;
};

struct DumbbellHash {
// реализуйте хешер для Dumbbell
    size_t operator()(const Dumbbell& dumbbell) const {
        // Используем комбинацию из всех полей, чтобы получить более равномерное распределение
        // Применяем простые арифметические операции и битовые сдвиги для эффективного смешивания
        size_t x1 = double_hasher_(dumbbell.circle1.x);
        size_t y1 = double_hasher_(dumbbell.circle1.y);
        size_t r1 = double_hasher_(dumbbell.circle1.r);
        size_t hash_circle1 = x1 + y1 * 31 + r1 * 31 * 31;
        
        size_t x2 = double_hasher_(dumbbell.circle2.x);
        size_t y2 = double_hasher_(dumbbell.circle2.y);
        size_t r2 = double_hasher_(dumbbell.circle2.r);
        size_t hash_circle2 = x2 + y2 * 31 + r2 * 31 * 31;
        
        size_t hash_text = string_hasher_(dumbbell.text);
        
        return (hash_circle1 * 31 * 31 * 31 * 31 + hash_circle2 * 31 + hash_text);
    }
private:
    hash<double> double_hasher_;
    hash<string> string_hasher_;
};

int main() {
    DumbbellHash hash;
    Dumbbell dumbbell{{10, 11.5, 2.3}, {3.14, 15, -8}, "abc"s};
    cout << "Dumbbell hash "s << hash(dumbbell);
}

//////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template <typename Hash>
int FindCollisions(const Hash& hasher, istream& text) {
    // место для вашей реализации
    size_t counter = 0;
    string word;
    unordered_map<size_t, unordered_set<string>> hash_word_collision_counter;
    while (text >> word) {
        size_t hs = hasher(word);
        bool is_collide = hash_word_collision_counter[hs].insert(move(word)).second;
        if (is_collide && hash_word_collision_counter.at(hs).size() > 1) {
            ++counter;
        }
    }
    return counter;
}

// Это плохой хешер. Его можно использовать для тестирования.
// Подумайте, в чём его недостаток
struct HasherDummy {
    size_t operator() (const string& str) const {
        size_t res = 0;
        for (char c : str) {
            res += static_cast<size_t>(c);
        }
        return res;
    }
};

struct DummyHash {
    size_t operator()(const string&) const {
        return 42;
    }
};

int main() {
    DummyHash dummy_hash;
    hash<string> good_hash;
    //HasherDummy h_d;

    {
        istringstream stream("I love C++"s);
        cout << FindCollisions(dummy_hash, stream) << endl;
    }
    {
        istringstream stream("I love C++"s);
        cout << FindCollisions(good_hash, stream) << endl;
    }
} 

///////////////////////////////////////////////////////////////////
#include "log_duration.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <set>
#include <string>
#include <sstream>
#include <unordered_set>

using namespace std;

class VehiclePlate {
private:
    auto AsTuple() const {
        return tie(letters_, digits_, region_);
    }

public:
    bool operator==(const VehiclePlate& other) const {
        return AsTuple() == other.AsTuple();
    }

    bool operator<(const VehiclePlate& other) const {
        return AsTuple() < other.AsTuple();
    }

    VehiclePlate(char l0, char l1, int digits, char l2, int region)
        : letters_{l0, l1, l2}
        , digits_(digits)
        , region_(region) {
    }

    string ToString() const {
        ostringstream out;
        out << letters_[0] << letters_[1];
        out << setfill('0') << right << setw(3) << digits_;
        out << letters_[2] << setw(2) << region_;

        return out.str();
    }

    const array<char, 3>& GetLetters() const {
        return letters_;
    }

    int GetDigits() const {
        return digits_;
    }

    int GetRegion() const {
        return region_;
    }

private:
    array<char, 3> letters_;
    int digits_;
    int region_;
};

struct PlateHasherTrivial {
    size_t operator()(const VehiclePlate& plate) const {
        return static_cast<size_t>(plate.GetDigits());
    }
};

struct PlateHasherRegion {
    size_t operator()(const VehiclePlate& plate) const {
        return static_cast<size_t>(plate.GetDigits() + plate.GetRegion() * 1000);
    }
};

struct PlateHasherString {
    size_t operator()(const VehiclePlate& plate) const {
        return hasher_(plate.ToString());
    }
private:
    hash<string> hasher_;
};

struct PlateHasherAll {
    // реализуйте собственный хешер, который будет
    // эффективнее и лучше всех остальных
    size_t operator()(const VehiclePlate& plate) const {
        // Используем комбинацию из всех полей, чтобы получить более равномерное распределение
        // Применяем простые арифметические операции и битовые сдвиги для эффективного смешивания
        size_t hash = 0;
        hash = hash * 31 + std::hash<char>{}(plate.GetLetters()[0]);
        hash = hash * 31 + std::hash<char>{}(plate.GetLetters()[1]);
        hash = hash * 31 + std::hash<char>{}(plate.GetLetters()[2]);
        hash = hash * 31 + plate.GetDigits();
        hash = hash * 31 + plate.GetRegion();
        return hash;
    }
};

ostream& operator<<(ostream& out, VehiclePlate plate) {
    out << plate.ToString();
    return out;
}

class PlateGenerator {
    char GenerateChar() {
        uniform_int_distribution<short> char_gen{0, static_cast<short>(possible_chars_.size() - 1)};
        return possible_chars_[char_gen(engine_)];
    }

    int GenerateNumber() {
        uniform_int_distribution<short> num_gen{0, 999};
        return num_gen(engine_);
    }

    int GenerateRegion() {
        uniform_int_distribution<short> region_gen{0, static_cast<short>(possible_regions_.size() - 1)};
        return possible_regions_[region_gen(engine_)];
    }

public:
    VehiclePlate Generate() {
        return VehiclePlate(GenerateChar(), GenerateChar(), GenerateNumber(), GenerateChar(), GenerateRegion());
    }

private:
    mt19937 engine_;

    // допустимые значения сохраним в static переменных
    // они объявлены inline, чтобы их определение не надо было выносить вне класса
    inline static const array possible_regions_
        = {1,  2,  102, 3,   4,   5,   6,   7,   8,  9,   10,  11,  12, 13,  113, 14,  15, 16,  116, 17, 18,
           19, 20, 21,  121, 22,  23,  93,  123, 24, 84,  88,  124, 25, 125, 26,  27,  28, 29,  30,  31, 32,
           33, 34, 35,  36,  136, 37,  38,  85,  39, 91,  40,  41,  82, 42,  142, 43,  44, 45,  46,  47, 48,
           49, 50, 90,  150, 190, 51,  52,  152, 53, 54,  154, 55,  56, 57,  58,  59,  81, 159, 60,  61, 161,
           62, 63, 163, 64,  164, 65,  66,  96,  67, 68,  69,  70,  71, 72,  73,  173, 74, 174, 75,  80, 76,
           77, 97, 99,  177, 199, 197, 777, 78,  98, 178, 79,  83,  86, 87,  89,  94,  95};

    // постфикс s у литерала тут недопустим, он приведёт к неопределённому поведению
    inline static const string_view possible_chars_ = "ABCEHKMNOPTXY"sv;
};

int main() {
    static const int N = 1'000'000;

    PlateGenerator generator;
    vector<VehiclePlate> fill_vector;
    vector<VehiclePlate> find_vector;

    generate_n(back_inserter(fill_vector), N, [&]() {
        return generator.Generate();
    });
    generate_n(back_inserter(find_vector), N, [&]() {
        return generator.Generate();
    });

    int found;
    {
        LOG_DURATION("unordered_set");
        unordered_set<VehiclePlate, PlateHasherAll> container;
        for (auto& p : fill_vector) {
            container.insert(p);
        }
        found = count_if(find_vector.begin(), find_vector.end(), [&](const VehiclePlate& plate) {
            return container.count(plate) > 0;
        });
    }
    cout << "Found matches (1): "s << found << endl;

    {
        LOG_DURATION("set");
        set<VehiclePlate> container;
        for (auto& p : fill_vector) {
            container.insert(p);
        }
        found = count_if(find_vector.begin(), find_vector.end(), [&](const VehiclePlate& plate) {
            return container.count(plate) > 0;
        });
    }
    cout << "Found matches (2): "s << found << endl;
}

//////////////////////////////////////////////////
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

class VehiclePlate {
private:
    auto AsTuple() const {
        return tie(letters_, digits_, region_);
    }

public:
    bool operator==(const VehiclePlate& other) const {
        return AsTuple() == other.AsTuple();
    }

    VehiclePlate(char l0, char l1, int digits, char l2, int region)
        : letters_{l0, l1, l2}
        , digits_(digits)
        , region_(region) {
    }

    string ToString() const {
        ostringstream out;
        out << letters_[0] << letters_[1];

        // чтобы дополнить цифровую часть номера слева нулями
        // до трёх цифр, используем подобные манипуляторы:
        // setfill задаёт символ для заполнения,
        // right задаёт выравнивание по правому краю,
        // setw задаёт минимальное желаемое количество знаков
        out << setfill('0') << right << setw(3) << digits_;
        out << letters_[2] << setw(2) << region_;

        return out.str();
    }

    int Hash() const {
        return digits_;
    }

private:
    array<char, 3> letters_;
    int digits_;
    int region_;
};

ostream& operator<<(ostream& out, VehiclePlate plate) {
    out << plate.ToString();
    return out;
}

class ParkingCounter {
public:
    // зарегистрировать парковку автомобиля
    void Park(VehiclePlate car) {
        // место для вашей реализации
        ++car_to_parks_[car];
    }

    // метод возвращает количество зарегистрированных 
    // парковок автомобиля
    int GetCount(const VehiclePlate& car) const {
        // место для вашей реализации
        auto car_park_counter = car_to_parks_.find(car);
        if (car_park_counter == car_to_parks_.end()) {
            return 0;
        }
        return car_to_parks_.at(car);
    }

    auto& GetAllData() const {
        return car_to_parks_;
    }

private:
    // для хранения данных используйте контейнер unordered_map
    // назовите поле класса car_to_parks_
    unordered_map<VehiclePlate, int, VehiclePlateHasher> car_to_parks_;
};

class VehiclePlateHasher {
public:
    size_t operator()(const VehiclePlate& plate) const {
        // измените эту функцию, чтобы она учитывала все данные номера
        // рекомендуется использовать метод ToString() и существующий 
        // класс hash<string>
        string plate_string = plate.ToString();
        return static_cast<size_t>(plate_hash_(plate_string));
    }
private:
    hash<string> plate_hash_; 
};
// выбросьте это исключение в случае ошибки парковки
struct ParkingException {};

template <typename Clock>
class Parking {
    // при обращении к типу внутри шаблонного параметра мы обязаны использовать 
    // typename; чтобы этого избежать, объявим псевдонимы для нужных типов
    using Duration = typename Clock::duration;
    using TimePoint = typename Clock::time_point;

public:
    Parking(int cost_per_second) : cost_per_second_(cost_per_second) {}

    // запарковать машину с указанным номером
    void Park(VehiclePlate car) {
        // место для вашей реализации
        if (now_parked_.find(car) != now_parked_.end()) {
            throw ParkingException();
        }
        TimePoint start_time = Clock::now();
        now_parked_[car] = start_time;
    }

    // забрать машину с указанным номером
    void Withdraw(const VehiclePlate& car) {
        // место для вашей реализации
        if (now_parked_.find(car) == now_parked_.end()) {
            throw ParkingException();
        }
        TimePoint stop_time = Clock::now();
        Duration duration = stop_time - now_parked_.at(car); // start_time;
        complete_parks_[car] += duration;
        now_parked_.erase(car);
    }

    // получить счёт за конкретный автомобиль
    int64_t GetCurrentBill(const VehiclePlate& car) const {
        // место для вашей реализации
        int64_t cost = 0;
        if (complete_parks_.count(car)) {
            cost += chrono::duration_cast<chrono::seconds>(complete_parks_.at(car)).count() * cost_per_second_;
        }
        if (now_parked_.count(car)) {
            TimePoint current_time = Clock::now();
            cost += chrono::duration_cast<chrono::seconds>(current_time - now_parked_.at(car)).count() * cost_per_second_;
        }
        return cost;
    }

    // завершить расчётный период
    // те машины, которые находятся на парковке на данный момент, должны 
    // остаться на парковке, но отсчёт времени для них начинается с нуля
    unordered_map<VehiclePlate, int64_t, VehiclePlateHasher> EndPeriodAndGetBills() {
        // место для вашей реализации
        unordered_map<VehiclePlate, int64_t, VehiclePlateHasher> complete_car_bills;
        for (const auto& [car, time_n] : complete_parks_) {
            complete_car_bills[car] = 0;
        }
        for (const auto& [car, time_n] : now_parked_) {
            complete_car_bills[car] = 0;
        }
        for (auto& [car, time_n] : complete_car_bills) {
            complete_car_bills[car] += GetCurrentBill(car);
        }
        for (auto [car, time_n] : now_parked_) {
            now_parked_[car] = Clock::now();
        }
        complete_parks_.clear();
        return complete_car_bills;
    }

    // не меняйте этот метод
    auto& GetNowParked() const {
        return now_parked_;
    }

    // не меняйте этот метод
    auto& GetCompleteParks() const {
        return complete_parks_;
    }

private:
    int cost_per_second_;
    unordered_map<VehiclePlate, TimePoint, VehiclePlateHasher> now_parked_;
    unordered_map<VehiclePlate, Duration, VehiclePlateHasher> complete_parks_;
};

// эти часы удобно использовать для тестирования
// они покажут столько времени, сколько вы задали явно
class TestClock {
public:
    using time_point = chrono::system_clock::time_point;
    using duration = chrono::system_clock::duration;

    static void SetNow(int seconds) {
        current_time_ = seconds;
    }

    static time_point now() {
        return start_point_ + chrono::seconds(current_time_);
    }

private:
    inline static time_point start_point_ = chrono::system_clock::now();
    inline static int current_time_ = 0;
};

int main() {
    Parking<TestClock> parking(10);

    TestClock::SetNow(10);
    parking.Park({'A', 'A', 111, 'A', 99});

    TestClock::SetNow(20);
    parking.Withdraw({'A', 'A', 111, 'A', 99});
    parking.Park({'B', 'B', 222, 'B', 99});

    TestClock::SetNow(40);
    assert(parking.GetCurrentBill({'A', 'A', 111, 'A', 99}) == 100);
    assert(parking.GetCurrentBill({'B', 'B', 222, 'B', 99}) == 200);
    parking.Park({'A', 'A', 111, 'A', 99});

    TestClock::SetNow(50);
    assert(parking.GetCurrentBill({'A', 'A', 111, 'A', 99}) == 200);
    assert(parking.GetCurrentBill({'B', 'B', 222, 'B', 99}) == 300);
    assert(parking.GetCurrentBill({'C', 'C', 333, 'C', 99}) == 0);
    parking.Withdraw({'B', 'B', 222, 'B', 99});

    TestClock::SetNow(70);
    {
        // проверим счёт
        auto bill = parking.EndPeriodAndGetBills();

        // так как внутри макроса используется запятая,
        // нужно заключить его аргумент в дополнительные скобки
        assert((bill
                == unordered_map<VehiclePlate, int64_t, VehiclePlateHasher>{
                   {{'A', 'A', 111, 'A', 99}, 400},
                   {{'B', 'B', 222, 'B', 99}, 300},
               }));
    }

    TestClock::SetNow(80);
    {
        // проверим счёт
        auto bill = parking.EndPeriodAndGetBills();

        // так как внутри макроса используется запятая,
        // нужно заключить его аргумент в дополнительные скобки
        assert((bill
                == unordered_map<VehiclePlate, int64_t, VehiclePlateHasher>{
                   {{'A', 'A', 111, 'A', 99}, 100},
               }));
    }

    try {
        parking.Park({'A', 'A', 111, 'A', 99});
        assert(false);
    }
    catch (ParkingException) {
    }

    try {
        parking.Withdraw({'B', 'B', 222, 'B', 99});
        assert(false);
    }
    catch (ParkingException) {
    }

    cout << "Success!"s << endl;
}

////////////////////////////////////////////////
#include <array>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <tuple>

using namespace std;

class VehiclePlate {
private:
    auto AsTuple() const {
        return tie(letters_, digits_, region_);
    }
public:
    VehiclePlate(char l0, char l1, int digits, char l2, int region)
        : letters_{l0, l1, l2}
        , digits_(digits)
        , region_(region) {
    }

    string ToString() const {
        ostringstream out;
        out << letters_[0] << letters_[1];
        // чтобы дополнить цифровую часть номера слева нулями
        // до трёх цифр, используем подобные манипуляторы:
        // setfill задаёт символ для заполнения,
        // right задаёт выравнивание по правому краю,
        // setw задаёт минимальное желаемое количество знаков
        out << setfill('0') << right << setw(3) << digits_;
        out << letters_[2] << setw(2) << region_;

        return out.str();
    }

    int Hash() const {
        return digits_;
    }

    //[[nodiscard]] bool operator==(const VehiclePlate& rhs) const noexcept {
    //    return letters_ == rhs.letters_ && digits_ == rhs.digits_ && region_ == rhs.region_;
    //}
    bool operator==(const VehiclePlate& other) const {
        return AsTuple() == other.AsTuple();
    }

private:
    array<char, 3> letters_;
    int digits_;
    int region_;
};

ostream& operator<<(ostream& out, VehiclePlate plate) {
    out << plate.ToString();
    return out;
}

template <typename T>
class HashableContainer {
public:
    void Insert(T elem) {
        int index = elem.Hash();

        // если вектор недостаточно велик для этого индекса,
        // то увеличим его, выделив место с запасом
        if (index >= int(elements_.size())) {
            elements_.resize(index * 2 + 1);
        }
    
        for (auto& e : elements_[index]) {
            if (e == elem) {
                return;
            }
        }
        elements_[index].push_back(move(elem));
    }

    void PrintAll(ostream& out) const {
        for (auto& e : elements_) {
            if (e.empty()) {
                continue;
            }
            for (auto& e_in : e) {
                out << e_in << endl;
            }
        }
    }

    const auto& GetVector() const {
        return elements_;
    }

private:
    vector<vector<T>> elements_;
};

class VehiclePlateHasher {
public:
    size_t operator()(const VehiclePlate& plate) const {
        return static_cast<size_t>(plate.Hash());
    }
};

int main() {
    // явно указываем хешер шаблонным параметром
    unordered_set<VehiclePlate, VehiclePlateHasher> plate_base;

    plate_base.insert({'B', 'H', 840, 'E', 99});
    plate_base.insert({'O', 'K', 942, 'K', 78});
    plate_base.insert({'O', 'K', 942, 'K', 78});
    plate_base.insert({'O', 'K', 942, 'K', 78});
    plate_base.insert({'O', 'K', 942, 'K', 78});
    plate_base.insert({'H', 'E', 968, 'C', 79});
    plate_base.insert({'T', 'A', 326, 'X', 83});
    plate_base.insert({'H', 'H', 831, 'P', 116});
    plate_base.insert({'A', 'P', 831, 'Y', 99});
    plate_base.insert({'P', 'M', 884, 'K', 23});
    plate_base.insert({'O', 'C', 34, 'P', 24});
    plate_base.insert({'M', 'Y', 831, 'M', 43});
    plate_base.insert({'B', 'P', 831, 'M', 79});
    plate_base.insert({'K', 'T', 478, 'P', 49});
    plate_base.insert({'X', 'P', 850, 'A', 50});

    for (auto& plate : plate_base) {
        cout << plate << endl;
    }

}

//////////////////////////////////////////////
//#include <algorithm>
//#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

class MoneyBox {
public:
    explicit MoneyBox(vector<int64_t> nominals)
        : nominals_(move(nominals))
        , counts_(nominals_.size()) {
    }

    const vector<int>& GetCounts() const {
        return counts_;
    }

    void PushCoin(int64_t value) {
        // реализуйте метод добавления купюры или монеты
        //int dist = GetIndex(value);
        //assert(dist < int(nominals_.size()));
        int64_t dist = 0;
        while (nominals_[dist] != value) {
            ++dist;
        }
        ++counts_[dist];
    }

    void PrintCoins(ostream& out) const {
        // реализуйте метод печати доступных средств
        for (size_t i = 0; i < nominals_.size(); ++i) {
            if (counts_[i] > 0) {
                out << nominals_[i] << ": "s << counts_[i] << endl;
            }
        }
    }

private:
    const vector<int64_t> nominals_;
    vector<int> counts_;

    //int GetIndex(int64_t value) const {
    //    return find(nominals_.begin(), nominals_.end(), value) - nominals_.begin();
    //}
};

ostream& operator<<(ostream& out, const MoneyBox& cash) {
    cash.PrintCoins(out);
    return out;
}

int main() {
    MoneyBox cash({10, 50, 100, 200, 500, 1000, 2000, 5000});

    int times;
    cout << "Enter number of coins you have:"s << endl;
    cin >> times;

    cout << "Enter all nominals:"s << endl;
    for (int i = 0; i < times; ++i) {
        int64_t value;
        cin >> value;
        cash.PushCoin(value);
    }

    cout << cash << endl;
}

///////////////////////////////
#include <cassert>
#include <cstdint>
#include <iostream>
#include <deque>
#include <numeric>
#include <vector>
#include <utility>

using namespace std;

class Rabbit {
public:
    enum class Color { WHITE, BLACK, INVISIBLE };

    Rabbit(Color color)
        : color_(color) 
    {
    }
    // копирование кроликов, как и слонов, в природе не предусмотрено
    Rabbit(const Rabbit& other) = delete;
    Rabbit& operator=(const Rabbit& other) = delete;

    //Rabbit(Rabbit&& other) = default;
    //Rabbit& operator=(Rabbit&& other) = default;
    Rabbit(Rabbit&& other) {
    color_ = exchange(other.color_, Color::INVISIBLE);
}

Rabbit& operator=(Rabbit&& other) {
    color_ = exchange(other.color_, Color::INVISIBLE);
    return *this;
}

    Color GetColor() const {
        return color_;
    }

private:
    Color color_;
};

class Hat {
public:
    Hat(Rabbit&& rabbit)
        : rabbit_(move(rabbit)) 
    {
    }
    // не стоит копировать шляпу
    Hat(const Hat& other) = delete;
    Hat& operator=(const Hat& other) = delete;
    // но вот переместить шляпу можно
    Hat(Hat&& other) = default;
    Hat& operator=(Hat&&) = default;
    // в любой момент можно посмотреть, есть ли кролик в шляпе
    const Rabbit& GetRabbit() const {
        return rabbit_;
    }
private:
    Rabbit rabbit_;
};

int main() {
    Hat magic_hat(Rabbit(Rabbit::Color::WHITE));
    Hat other_magic_hat(move(magic_hat));
    if (magic_hat.GetRabbit().GetColor() == Rabbit::Color::WHITE) {
        cout << "Кролик перемещён, но его цвет белый"s << endl;
    }
    else if (magic_hat.GetRabbit().GetColor() == Rabbit::Color::BLACK) {
        cout << "Кролик перемещён, но его цвет чёрный"s << endl;
    }
    else {
        cout << "Кажется, мы не уверены на счёт цвета кролика"s << endl;
    }
}

///////////////////////////////////////////////////////
#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <string_view>
#include <vector>

using namespace std;

class Translator {
public:
    Translator() = default;
    void Add(string_view source, string_view target) {
        words_.push_front(string{source});
        words_.push_back(string{target});
        target_server_[words_.back()] = words_.front();
        source_server_[words_.front()] = words_.back();
    }
    string_view TranslateForward(string_view source) const {
        auto it = source_server_.find(source);
        return it != source_server_.end() ? it->second : "";
    }
    string_view TranslateBackward(string_view target) const {
        auto it = target_server_.find(target);
        return it != target_server_.end() ? it->second : "";
    }

private:
    unordered_map<string_view, string_view> target_server_;
    unordered_map<string_view, string_view> source_server_;
    deque<string> words_;
};

void TestSimple() {
    Translator translator;
    translator.Add(string("okno"s), string("window"s));
    translator.Add(string("stol"s), string("table"s));

    assert(translator.TranslateForward("okno"s) == "window"s);
    assert(translator.TranslateBackward("table"s) == "stol"s);
    assert(translator.TranslateForward("table"s) == ""s);
}

int main() {
    TestSimple();
}

//////////////////////////////////////
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string_view>
#include <vector>


using namespace std;

vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result;
    str.remove_prefix(min(str.find_first_not_of(" "), str.size()));
    
    while (!str.empty()) {
        // тут выполнен инвариант: str не начинается с пробела
        int64_t space = str.find(' ');

        // здесь можно избавиться от проверки на равенство npos
        // если space == npos, метод substr ограничит возвращаемый string_view концом строки
        result.push_back(str.substr(0, space));
        str.remove_prefix(min(str.find_first_not_of(" ", space), str.size()));
    }

    return result;
}

int main() {
    assert((SplitIntoWordsView("") == vector<string_view>{}));
    assert((SplitIntoWordsView("     ") == vector<string_view>{}));
    assert((SplitIntoWordsView("aaaaaaa") == vector{"aaaaaaa"sv}));
    assert((SplitIntoWordsView("a") == vector{"a"sv}));
    assert((SplitIntoWordsView("a b c") == vector{"a"sv, "b"sv, "c"sv}));
    assert((SplitIntoWordsView("a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    cout << "All OK" << endl;
}

////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <cstdint>
#include <string_view>
#include <vector>

using namespace std;

vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result{};

    // Используем цикл для поиска пробельных символов
    while (!str.empty()) {
        // Находим первый непробельный символ и удаляем префикс до него
        auto firstNonWhitespacePos = str.find_first_not_of(" ");
        if (firstNonWhitespacePos != str.npos) {
            str.remove_prefix(firstNonWhitespacePos);
        } else {
            return result;
        }
        // Находим первую позицию пробела
        size_t spacePos = str.find(' ');
        if (spacePos == str.npos) {
            // Если нет пробела, добавляем остаток строки в результат
            result.emplace_back(str);
            break;
        } else {
            // Добавляем текущий отрезок строки в результат
            result.emplace_back(str.substr(0, spacePos));
            // Перемещаем начало строки после пробела
            str.remove_prefix(spacePos + 1);
        }
    }

    return result;
}

int main() {
    assert((SplitIntoWordsView("") == vector<string_view>{}));
    assert((SplitIntoWordsView("     ") == vector<string_view>{}));
    assert((SplitIntoWordsView("aaaaaaa") == vector{"aaaaaaa"sv}));
    assert((SplitIntoWordsView("a") == vector{"a"sv}));
    assert((SplitIntoWordsView("a b c") == vector{"a"sv, "b"sv, "c"sv}));
    assert((SplitIntoWordsView("a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    cout << "All OK" << endl;
}

///////////////////////////////////////////
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <chrono>
#include <random>

using namespace std;

// Функция для генерации случайного вектора или дека
template <typename Container>
void generateRandomData(Container& container, int size) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 1000000);

    for (int i = 0; i < size; ++i) {
        container.push_back(dis(gen));
    }
}

// Функция для измерения времени сортировки
template <typename Container>
double measureSortTime(Container& container) {
    auto start = chrono::high_resolution_clock::now();
    sort(container.begin(), container.end());
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    return duration.count() / 1000.0; // Преобразование в миллисекунды
}

int main() {
    const int size = 1000000;

    // Генерация данных
    vector<int> v;
    deque<int> d;
    generateRandomData(v, size);
    generateRandomData(d, size);

    // Сортировка вектора
    double vectorTime = measureSortTime(v);
    cout << "Время сортировки вектора: " << vectorTime << " мс" << endl;

    // Сортировка дека
    double dequeTime = measureSortTime(d);
    cout << "Время сортировки дека: " << dequeTime << " мс" << endl;

    return 0;
}

////////////////////////////////////////
#include <cassert>
#include <vector>

template <typename T>
void ReverseArray(T* start, size_t size) {
    // Напишите тело функции самостоятельно
    if (size == 0 || size == 1) {
        return;
    }

    size_t left = 0;
    size_t right = size - 1;

    while (left < right) {
        std::swap(start[left], start[right]);
        ++left;
        --right;
    }
}

int main() {
    using namespace std;
    
    vector<int> v = {1, 2};
    ReverseArray(v.data(), v.size());
    assert(v == (vector<int>{2, 1}));
}

/////////////////////////////////////////////////////////
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>

using namespace std;

// Используйте эту заготовку PtrVector или замените её на свою реализацию
template <typename T>
class PtrVector {
public:
    PtrVector() = default;

    // Создаёт вектор указателей на копии объектов из other
    PtrVector(const PtrVector& other) {
        // Резервируем место в vector-е для хранения нужного количества элементов
        // Благодаря этому при push_back не будет выбрасываться исключение
        items_.reserve(other.items_.size());

        try {
            for (auto p : other.items_) {
                // Копируем объект, если указатель на него ненулевой
                auto p_copy = p ? new T(*p) : nullptr;  // new может выбросить исключение

                // Не выбросит исключение, т. к. в vector память уже зарезервирована
                items_.push_back(p_copy);
            }
        } catch (...) {
            // удаляем элементы в векторе и перевыбрасываем пойманное исключение
            DeleteItems();
            throw;
        }
    }

    PtrVector& operator=(const PtrVector& rhs) {
        if (this != &rhs) {
            // Реализация операции присваивания с помощью идиомы Copy-and-swap.
            // Если исключение будет выброшено, то на текущий объект оно не повлияет.
            PtrVector rhs_copy(rhs);

            // rhs_copy содержит копию правого аргумента.
            // Обмениваемся с ним данными.
            swap(rhs_copy); // или просто items_.swap(rhs_copy.items_); - тогда метод swap в классе не нужен

            // Теперь текущий объект содержит копию правого аргумента,
            // а rhs_copy - прежнее состояние текущего объекта, которое при выходе
            // из блока будет разрушено.
        }

        return *this;
    }

    // обменивает состояние текущего объекта с other без выбрасывания исключений
    void swap(PtrVector& other) noexcept {
        std::swap(GetItems(), other.GetItems());
    };

    // Деструктор удаляет объекты в куче, на которые ссылаются указатели,
    // в векторе items_
    ~PtrVector() {
        DeleteItems();
    }

    // Возвращает ссылку на вектор указателей
    vector<T*>& GetItems() noexcept {
        return items_;
    }

    // Возвращает константную ссылку на вектор указателей
    vector<T*> const& GetItems() const noexcept {
        return items_;
    }

private:
    void DeleteItems() noexcept {
        for (auto p : items_) {
            delete p;
        }
    }

    vector<T*> items_;
};

// Эта функция main тестирует шаблон класса PtrVector
int main() {
    struct CopyingSpy {
        CopyingSpy(int& copy_count, int& deletion_count)
            : copy_count_(copy_count)
            , deletion_count_(deletion_count) {
        }
        CopyingSpy(const CopyingSpy& rhs)
            : copy_count_(rhs.copy_count_)          // счётчик копирований
            , deletion_count_(rhs.deletion_count_)  // счётчик удалений
        {
            if (rhs.throw_on_copy_) {
                throw runtime_error("copy construction failed"s);
            }
            ++copy_count_;
        }
        ~CopyingSpy() {
            ++deletion_count_;
        }
        void ThrowOnCopy() {
            throw_on_copy_ = true;
        }

    private:
        int& copy_count_;
        int& deletion_count_;
        bool throw_on_copy_ = false;
    };

    // Проверка присваивания
    {
        int item0_copy_count = 0;
        int item0_deletion_count = 0;
        {
            PtrVector<CopyingSpy> v;

            v.GetItems().push_back(new CopyingSpy(item0_copy_count, item0_deletion_count));
            v.GetItems().push_back(nullptr);
            {
                PtrVector<CopyingSpy> v_copy;
                v_copy = v;
                assert(v_copy.GetItems().size() == v.GetItems().size());
                assert(v_copy.GetItems().at(0) != v.GetItems().at(0));
                assert(v_copy.GetItems().at(1) == nullptr);
                assert(item0_copy_count == 1);
                assert(item0_deletion_count == 0);
            }
            assert(item0_deletion_count == 1);
        }
        assert(item0_deletion_count == 2);
    }

    // Проверка корректности самоприсваивания
    {
        int item0_copy_count = 0;
        int item0_deletion_count = 0;

        PtrVector<CopyingSpy> v;
        v.GetItems().push_back(new CopyingSpy(item0_copy_count, item0_deletion_count));
        CopyingSpy* first_item = v.GetItems().front();

        v = v;
        assert(v.GetItems().size() == 1);
        // При самоприсваивании объекты должны быть расположены по тем же адресам
        assert(v.GetItems().front() == first_item);
        assert(item0_copy_count == 0);
        assert(item0_deletion_count == 0);
    }

    // Проверка обеспечения строгой гарантии безопасности исключений при присваивании
    {
        int item0_copy_count = 0;
        int item0_deletion_count = 0;

        int item1_copy_count = 0;
        int item1_deletion_count = 0;

        // v хранит 2 элемента
        PtrVector<CopyingSpy> v;
        v.GetItems().push_back(new CopyingSpy(item0_copy_count, item0_deletion_count));
        v.GetItems().push_back(new CopyingSpy(item1_copy_count, item1_deletion_count));

        int other_item0_copy_count = 0;
        int other_item0_deletion_count = 0;
        // other_vector хранит 1 элемент, при копировании которого будет выброшено исключение
        PtrVector<CopyingSpy> other_vector;
        other_vector.GetItems().push_back(new CopyingSpy(other_item0_copy_count, other_item0_deletion_count));
        other_vector.GetItems().front()->ThrowOnCopy();

        // Сохраняем массив указателей
        auto v_items(v.GetItems());

        try {
            v = other_vector;
            // Операция должна выбросить исключение
            assert(false);
        } catch (const runtime_error&) {
        }

        // Элементы массива должны остаться прежними
        assert(v.GetItems() == v_items);
        assert(item0_copy_count == 0);
        assert(item1_copy_count == 0);
        assert(other_item0_copy_count == 0);
    }
}

/////////////////////////////////////////////
#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

template <typename T>
class PtrVector {
public:
    PtrVector() = default;

    // Создаёт вектор указателей на копии объектов из other
    PtrVector(const PtrVector& other) {
        // Реализуйте копирующий конструктор самостоятельно
        items_.reserve(other.items_.size());
        
        try {
            for (T* item : other.items_) {
                if (item != nullptr) {
                    T* new_item = new T(*item);
                    items_.push_back(new_item);
                } else {
                    items_.push_back(nullptr);
                }
            }
        } catch (...) {
            Cleanup();
            throw;
        }
    }
    // Деструктор удаляет объекты в куче, на которые ссылаются указатели,
    // в векторе items_
    ~PtrVector() {
        // Реализуйте тело деструктора самостоятельно
        for (T* item : items_) {
            delete item;
        }
    }

    // Возвращает ссылку на вектор указателей
    vector<T*>& GetItems() noexcept {
        // Реализуйте метод самостоятельно
        return items_;
    }

    // Возвращает константную ссылку на вектор указателей
    vector<T*> const& GetItems() const noexcept {
        // Реализуйте метод самостоятельно
        return items_;
    }

    void Cleanup() {
        for (T* item : items_) {
            delete item;
        }
    }

private:
    vector<T*> items_;

};

// Эта функция main тестирует шаблон класса PtrVector
int main() {
    // Вспомогательный "шпион", позволяющий узнать о своём удалении
    struct DeletionSpy {
        explicit DeletionSpy(bool& is_deleted)
            : is_deleted_(is_deleted) {
        }
        ~DeletionSpy() {
            is_deleted_ = true;
        }
        bool& is_deleted_;
    };

    // Проверяем удаление элементов
    {
        bool spy1_is_deleted = false;
        DeletionSpy* ptr1 = new DeletionSpy(spy1_is_deleted);
        {
            PtrVector<DeletionSpy> ptr_vector;
            ptr_vector.GetItems().push_back(ptr1);
            assert(!spy1_is_deleted);

            // Константная ссылка на ptr_vector
            const auto& const_ptr_vector_ref(ptr_vector);
            // И константная, и неконстантная версия GetItems
            // должны вернуть ссылку на один и тот же вектор
            assert(&const_ptr_vector_ref.GetItems() == &ptr_vector.GetItems());
        }
        // При разрушении ptr_vector должен удалить все объекты, на которые
        // ссылаются находящиеся внутри него указателели
        assert(spy1_is_deleted);
    }

    // Вспомогательный «шпион», позволяющий узнать о своём копировании
    struct CopyingSpy {
        explicit CopyingSpy(int& copy_count)
            : copy_count_(copy_count) {
        }
        CopyingSpy(const CopyingSpy& rhs)
            : copy_count_(rhs.copy_count_)  //
        {
            ++copy_count_;
        }
        int& copy_count_;
    };

    // Проверяем копирование элементов при копировании массива указателей
    {
        // 10 элементов
        vector<int> copy_counters(10);

        PtrVector<CopyingSpy> ptr_vector;
        // Подготавливаем оригинальный массив указателей
        for (auto& counter : copy_counters) {
            ptr_vector.GetItems().push_back(new CopyingSpy(counter));
        }
        // Последний элемент содержит нулевой указатель
        ptr_vector.GetItems().push_back(nullptr);

        auto ptr_vector_copy(ptr_vector);
        // Количество элементов в копии равно количеству элементов оригинального вектора
        assert(ptr_vector_copy.GetItems().size() == ptr_vector.GetItems().size());

        // копия должна хранить указатели на новые объекты
        assert(ptr_vector_copy.GetItems() != ptr_vector.GetItems());
        // Последний элемент исходного массива и его копии - нулевой указатель
        assert(ptr_vector_copy.GetItems().back() == nullptr);
        // Проверяем, что элементы были скопированы (копирующие шпионы увеличивают счётчики копий).
        assert(all_of(copy_counters.begin(), copy_counters.end(), [](int counter) {
            return counter == 1;
        }));
    }
}

///////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

// Умный указатель, удаляющий связанный объект при своём разрушении.
// Параметр шаблона T задаёт тип объекта, на который ссылается указатель
template <typename T>
class ScopedPtr {
public:
    // Конструктор по умолчанию создаёт нулевой указатель,
    // так как поле ptr_ имеет значение по умолчанию nullptr
    ScopedPtr() = default;

    // Создаёт указатель, ссылающийся на переданный raw_ptr.
    // raw_ptr ссылается либо на объект, созданный в куче при помощи new,
    // либо является нулевым указателем
    // Спецификатор noexcept обозначает, что метод не бросает исключений
    explicit ScopedPtr(T* raw_ptr) noexcept : ptr_(raw_ptr) {
    }

    // Удаляем у класса конструктор копирования
    ScopedPtr(const ScopedPtr&) = delete;

    // Деструктор. Удаляет объект, на который ссылается умный указатель.
    ~ScopedPtr() {
        // Реализуйте тело деструктора самостоятельно
        delete ptr_;
    }

    // Возвращает указатель, хранящийся внутри ScopedPtr
    T* GetRawPtr() const noexcept {
        return ptr_;
        // Напишите код метода самостоятельно
    }

    // Прекращает владение объектом, на который ссылается умный указатель.
    // Возвращает прежнее значение "сырого" указателя и устанавливает поле ptr_ в null
    T* Release() noexcept {
        // Реализуйте самостоятельно
        T* ret_ptr = ptr_;
        ptr_ = nullptr;
        return ret_ptr;
    }

    // Оператор приведения к типу bool позволяет узнать, ссылается ли умный указатель
    // на какой-либо объект
    explicit operator bool() const noexcept {
        // Реализуйте самостоятельно
        return ptr_ != nullptr;
    }

    // Оператор разыменования возвращает ссылку на объект
    // Выбрасывает исключение std::logic_error, если указатель нулевой
    T& operator*() const {
        // Реализуйте самостоятельно
        if (!ptr_) {
            throw logic_error("Error: ptr_ is null"s);
        }
        return *ptr_;
    }

    // Оператор -> должен возвращать указатель на объект
    // Выбрасывает исключение std::logic_error, если указатель нулевой
    T* operator->() const {
        // Реализуйте самостоятельно
        if (!ptr_) {
            throw logic_error("Error: ptr_ is null"s);
        }
        return ptr_;
    }

private:
    T* ptr_ = nullptr;
};

// Этот main тестирует класс ScopedPtr
int main() {
    // Проверка работы оператора приведения к типу bool
    {
        // Для нулевого указателя приведение к типу bool возвращает false
        const ScopedPtr<int> empty_ptr;
        assert(!empty_ptr);

        // Для ненулевого указателя приведение к типу bool возвращает true
        const ScopedPtr<int> ptr_to_existing_object(new int(0));
        assert(ptr_to_existing_object);

        static_assert(noexcept(static_cast<bool>(ptr_to_existing_object)));
    }

    // Проверка работы оператора разыменования *
    {
        string* raw_ptr = new string("hello");
        ScopedPtr<string> smart_ptr(raw_ptr);
        // Ссылка, возвращаемая оператором разыменования, должна ссылаться на объект,
        // на который указывает умный указатель
        assert(&*smart_ptr == raw_ptr);

        try {
            ScopedPtr<int> empty_ptr;
            // При попытке разыменовать пустой указатель должно быть выброшено
            // исключение logic_error
            *empty_ptr;
            // Сюда попасть мы не должны
            assert(false);
        } catch (const logic_error&) {
            // мы там, где нужно
        } catch (...) {
            // Других исключений выбрасываться не должно
            assert(false);
        }
    }
    
    // Проверка работы оператора ->
    {
        string* raw_ptr = new string("hello");
        ScopedPtr<string> smart_ptr(raw_ptr);
        // Доступ к членам класса через умный указатель должен быть аналогичен
        // доступу через "сырой" указатель
        assert(smart_ptr->data() == raw_ptr->data());

        try {
            ScopedPtr<string> empty_ptr;
            // При попытке разыменовать пустой указатель должно быть выброшено
            // исключение logic_error
            empty_ptr->clear();
            // Сюда попасть мы не должны
            assert(false);
        } catch (const logic_error&) {
            // мы там, где нужно
        } catch (...) {
            // Других исключений выбрасываться не должно
            assert(false);
        }
    }

    // Пример использования
    {
        // На этой структуре будет проверяться работа умного указателя
        struct Object {
            Object() {
                cout << "Object is default constructed"s << endl;
            }
            void DoSomething() {
                cout << "Doing something"s << endl;
            }
            ~Object() {
                cout << "Object is destroyed"s << endl;
            }
        };

        // Сконструированный по умолчанию указатель ссылается на nullptr
        ScopedPtr<Object> empty_smart_ptr;
        // Перегруженный оператор приведения к типу bool вернёт false для пустого указателя
        assert(!empty_smart_ptr);

        ScopedPtr<Object> smart_ptr(new Object());
        // Перегруженный оператор bool вернёт true для указателя, ссылающегося на объект
        assert(smart_ptr);

        // Проверка оператора разыменования
        (*smart_ptr).DoSomething();
        // Проверка оператора доступа к членам класса
        smart_ptr->DoSomething();
    }
}

////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Щупальце
class Tentacle {
public:
    explicit Tentacle(int id)
        : id_(id) {
    }

    int GetId() const {
        return id_;
    }

private:
    int id_ = 0;
};

// Осьминог
class Octopus {
public:
    Octopus() {
        Tentacle* t = nullptr;
        try {
            for (int i = 1; i <= 8; ++i) {
                t = new Tentacle(i);      // Может выбросить исключение bad_alloc
                tentacles_.push_back(t);  // Может выбросить исключение bad_alloc

                // Обнуляем указатель на щупальце, которое уже добавили в tentacles_,
                // чтобы не удалить его в обработчике catch повторно
                t = nullptr;
            }
        } catch (const bad_alloc&) {
            // Удаляем щупальца, которые успели попасть в контейнер tentacles_
            Cleanup();
            // Удаляем щупальце, которое создали, но не добавили в tentacles_
            delete t;
            // Конструктор не смог создать осьминога с восемью щупальцами,
            // поэтому выбрасываем исключение, чтобы сообщить вызывающему коду об ошибке
            // throw без параметров внутри catch выполняет ПЕРЕВЫБРОС пойманного исключения
            throw;
        }
    }

    const Tentacle& GetTentacle(int index) const {
        if (index < 0 || static_cast<size_t>(index) >= tentacles_.size()) {
            throw out_of_range("Invalid tentacle index"s);
        }
        // Чтобы превратить указатель в ссылку, разыменовываем его
        return *tentacles_[index];
    }

    ~Octopus() {
        // Осьминог владеет объектами в динамической памяти (щупальца),
        // которые должны быть удалены при его разрушении.
        // Деструктор - лучшее место, чтобы прибраться за собой.
        Cleanup();
    }

private:
    void Cleanup() {
        // Удаляем щупальца осьминога из динамической памяти
        for (Tentacle* t : tentacles_) {
            delete t;
        }
        // Очищаем массив указателей на щупальца
        tentacles_.clear();
    }

    // Вектор хранит указатели на щупальца. Сами объекты щупалец находятся в куче
    vector<Tentacle*> tentacles_;
};

int main() {
    {
        Octopus octopus;
        // Мы просто хотели ещё одного осьминога
        Octopus octopus1 = octopus;
        // Всё было хорошо и не предвещало беды...
        // ... до этого момента
    }
    cout << "Congratulations. Everything is OK!"s << endl;
}

////////////////////////////////
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Щупальце
class Tentacle {
public:
    explicit Tentacle(int id)
        : id_(id) {
    }

    int GetId() const {
        return id_;
    }

private:
    int id_ = 0;
};

// Осьминог
class Octopus {
public:
    Octopus() {
        // <--- Тело конструктора обновлено
        Tentacle* t = nullptr;
        try {
            for (int i = 1; i <= 8; ++i) {
                t = new Tentacle(i);
                tentacles_.push_back(t);
                t = nullptr;
            }
        } catch (const bad_alloc&) {
            Cleanup();
            delete t;
            throw bad_alloc();
        }
        // --->
    }

    // <--- Добавлен деструктор
    ~Octopus() {
        Cleanup();
    }
    // --->

private:
    // <--- Добавлен метод Cleanup
    void Cleanup() {
        for (Tentacle* t : tentacles_) {
            delete t;
        }
        tentacles_.clear();
    }
    // --->

    vector<Tentacle*> tentacles_;
};

int main() {
    Octopus octopus;
}

/////////////////////////////////////////////
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Породы кошек
enum class CatBreed {
    Bengal,
    Balinese,
    Persian,
    Siamese,
    Siberian,
    Sphynx,
};

// Пол
enum class Gender {
    Male,
    Female,
};

struct Cat {
    string name;
    Gender gender;
    CatBreed breed;
    int age;
};

string CatBreedToString(CatBreed breed) {
    switch (breed) {
        case CatBreed::Bengal:
            return "Bengal"s;
        case CatBreed::Balinese:
            return "Balinese"s;
        case CatBreed::Persian:
            return "Persian"s;
        case CatBreed::Siamese:
            return "Siamese"s;
        case CatBreed::Siberian:
            return "Siberian";
        case CatBreed::Sphynx:
            return "Sphynx"s;
        default:
            throw invalid_argument("Invalid cat breed"s);
    }
}

ostream& operator<<(ostream& out, CatBreed breed) {
    out << CatBreedToString(breed);
    return out;
}

ostream& operator<<(ostream& out, Gender gender) {
    out << (gender == Gender::Male ? "male"s : "female"s);
    return out;
}

ostream& operator<<(ostream& out, const Cat& cat) {
    out << '{' << cat.name << ", "s << cat.gender;
    out << ", breed: "s << cat.breed << ", age:"s << cat.age << '}';
    return out;
}

// Возвращает массив указателей на элементы вектора cats, отсортированные с использованием
// компаратора comp. Компаратор comp - функция, принимающая два аргумента типа const Cat&
// и возвращающая true, если значения упорядочены, и false в ином случае
template <typename Comparator>
vector<const Cat*> GetSortedCats(const vector<Cat>& cats, const Comparator& comp) {
    vector<const Cat*> sorted_cat_pointers;

    if (cats.size() == 0) {
        return {};
    }
    sorted_cat_pointers.reserve(cats.size());

    for (const auto& cat : cats) {
        sorted_cat_pointers.push_back(&cat);
    }
    
    //Напишите тело функции самостоятельно. Подсказка:
    //1) Поместите в массив sorted_cat_pointers адреса объектов из массива cats.
    //2) Отсортируйте массив sorted_cat_pointers с помощью переданного компаратора comp.
    //   Так как comp сравнивает ссылки на объекты, а отсортировать нужно указатели,
    //   передайте в sort лямбда функцию, принимающую указатели и сравнивающую объекты
    //   при помощи компаратора comp:
    //   [comp](const Cat* lhs, const Cat* rhs) {
    //       return comp(*lhs, *rhs);
    //   }
    
    sort(sorted_cat_pointers.begin(), sorted_cat_pointers.end(), [comp](const Cat* lhs, const Cat* rhs) {
        return comp(*lhs, *rhs);
    });
    return sorted_cat_pointers;
}

// Выводит в поток out значения объектов, на который ссылаются указатели вектора cat_pointers.
// Пример вывода элементов vector<const Cat*>:
// {{Tom, male, breed: Bengal, age:2}, {Charlie, male, breed: Balinese, age:7}}
void PrintCatPointerValues(const vector<const Cat*>& cat_pointers, ostream& out) {
    out << "{";
    bool is_first = true;
    for (const auto& cat: cat_pointers) {
        if (is_first) {
            out << *cat;
            is_first = false;
        } else {
            out << ", " << *cat;
        }
    }
    out << "}";
}

int main() {
    const vector<Cat> cats = {
        {"Tom"s, Gender::Male, CatBreed::Bengal, 2},
        {"Leo"s, Gender::Male, CatBreed::Siberian, 3},
        {"Luna"s, Gender::Female, CatBreed::Siamese, 1},
        {"Charlie"s, Gender::Male, CatBreed::Balinese, 7},
        {"Ginger"s, Gender::Female, CatBreed::Sphynx, 5},
        {"Tom"s, Gender::Male, CatBreed::Siamese, 2},
    };

    {
        auto sorted_cats = GetSortedCats(cats, [](const Cat& lhs, const Cat& rhs) {
            return tie(lhs.breed, lhs.name) < tie(rhs.breed, rhs.name);
        });

        cout << "Cats sorted by breed and name:"s << endl;
        PrintCatPointerValues(sorted_cats, cout);
        cout << endl;
    }

    {
        auto sorted_cats = GetSortedCats(cats, [](const Cat& lhs, const Cat& rhs) {
            return tie(lhs.gender, lhs.breed) < tie(rhs.gender, rhs.breed);
        });

        cout << "Cats sorted by gender and breed:"s << endl;
        PrintCatPointerValues(sorted_cats, cout);
        cout << endl;
    }
    return 0;
}

////////////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <string>

using namespace std;

int main() {
    int value = 1;

    // Сначала value_ptr ссылается на value
    int* value_ptr = &value;

    cout << "&value: "s << &value << endl;
    cout << "value_ptr: "s << value_ptr << endl;
    assert(*value_ptr == 1);

    int another_value = 2;
    // Затем ссылается на another_value
    value_ptr = &another_value;

    cout << "&another_value: "s << &another_value << endl;
    cout << "value_ptr: "s << value_ptr << endl;
    assert(*value_ptr == 2);
}

////////////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <string>

using namespace std;

int main() {
    int value = 1;

    // Сначала value_ptr ссылается на value
    int* value_ptr = &value;

    cout << "&value: "s << &value << endl;
    cout << "value_ptr: "s << value_ptr << endl;
    assert(*value_ptr == 1);

    int another_value = 2;
    // Затем ссылается на another_value
    value_ptr = &another_value;

    cout << "&another_value: "s << &another_value << endl;
    cout << "value_ptr: "s << value_ptr << endl;
    assert(*value_ptr == 2);
}

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

int EffectiveCount(const vector<int>& v, int n, int i) {
    
    double expected_answer = log2(static_cast<double>(v.size()));

    int64_t expected_border = static_cast<int64_t>(v.size())*(i + 1)/(n + 1);
    
    bool use_find_if = false;
    if (expected_border <= expected_answer) {
        use_find_if = true;
    }
    
    auto iter = v.begin();
    if (use_find_if) {
        cout << "Using find_if\n";
        iter = find_if(v.begin(), v.end(), [i](int x) {
            return x > i;
        });
    } else {
        cout << "Using upper_bound\n";
        iter = upper_bound(v.begin(), v.end(), i);
    }

    return iter - v.begin();
}

int main() {
    static const int NUMBERS = 1'000'000;
    static const int MAX = 1'000'000'000;

    mt19937 r;
    uniform_int_distribution<int> uniform_dist(0, MAX);

    vector<int> nums;
    for (int i = 0; i < NUMBERS; ++i) {
        int random_number = uniform_dist(r);
        nums.push_back(random_number);
    }
    sort(nums.begin(), nums.end());

    int i;
    cin >> i;
    int result = EffectiveCount(nums, MAX, i);
    cout << "Total numbers before "s << i << ": "s << result << endl;
}

//////////////////////////////////////////////////////////

#include <algorithm>
#include <deque>
#include <iostream>
#include <string>

using namespace std;

struct Ticket {
    int id;
    string name;
};

class TicketOffice {
public:
    // добавить билет в систему
    void PushTicket(const string& name) {
        Ticket t;
        t.name = name;
        t.id = last_id_;
        // реализуйте метод
        tickets_.push_back(t);
        ++last_id_;
    }

    // получить количество доступных билетов
    int GetAvailable() const {
        return tickets_.size();
        // реализуйте метод
    }

    // получить количество доступных билетов определённого типа
    int GetAvailable(const string& name) const {
        int counter = 0;
        for (auto it : tickets_) {
            if (it.name == name) {
            ++counter;
            }
        }
        return counter;
        // реализуйте метод
    }

    // отозвать старые билеты (до определённого id)
    void Invalidate(int minimum) {
        // реализуйте метод
        //tickets_.pop_front();
        for (auto it : tickets_) {
            if (it.id < minimum) {
                tickets_.pop_front();
            }
        }
    }

private:
    int last_id_ = 0;
    deque<Ticket> tickets_;
};

int main() {
TicketOffice tickets;

tickets.PushTicket("Swan Lake"); // id - 0
tickets.PushTicket("Swan Lake"); // id - 1
tickets.PushTicket("Boris Godunov"); // id - 2
tickets.PushTicket("Boris Godunov"); // id - 3
tickets.PushTicket("Swan Lake"); // id - 4
tickets.PushTicket("Boris Godunov"); // id - 5
tickets.PushTicket("Boris Godunov"); // id - 6

cout << tickets.GetAvailable() << endl; // Вывод: 7
cout << tickets.GetAvailable("Swan Lake") << endl; // Вывод: 3
cout << tickets.GetAvailable("Boris Godunov") << endl; // Вывод: 4

// Invalidate удалит билеты с номерами 0, 1, 2:
tickets.Invalidate(3);

cout << tickets.GetAvailable() << endl; // Вывод: 4
cout << tickets.GetAvailable("Swan Lake") << endl; // Вывод: 1
cout << tickets.GetAvailable("Boris Godunov") << endl; // Вывод: 3

tickets.PushTicket("Swan Lake"); // id - 7
tickets.PushTicket("Swan Lake"); // id - 8

cout << tickets.GetAvailable("Swan Lake") << endl; // Вывод: 3
}

//////////////////////////////////////////////////////
#include <iostream>

using namespace std;

template <typename F>
int FindFloor(int n, F drop) {
    // Переделайте этот алгоритм, имеющий линейную сложность.
    // В итоге должен получится логарифмический алгоритм.
    int a = 1, b = n;
    while (a != b) {
        int m = (a + b) / 2;
        if (drop(m)) {
            b = m;
        } else {
            a = m + 1;
        }
    }
    return a;
}

int main() {
    int n, t;
    cout << "Enter n and target floor number: "s << endl;
    //cin >> n >> t;

    n = 256;
    t = 129;
    int count = 0;
    int found = FindFloor(n, [t, &count](int f) {
        ++count;
        return f >= t;
    });

    cout << "Found floor "s << found << " after "s << count << " drops"s;

    return 0;
}

///////////////////////////////////////////
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

int main() {
    error_code err;

    path p = "tmp"_p / "a"_p / "folder"_p;

    CreateFile(p / "file.txt"_p);

    filesystem::create_directory(p, err);
    if (err) {
        cout << "Ошибка создания папки через create_directory: "s << err.message() << endl;
    } else {
        cout << "Успешно создана папка через create_directory: "s << p.string() << endl;
    }

    filesystem::create_directories(p, err);
    if (err) {
        cout << "Ошибка создания папки через create_directories: "s << err.message() << endl;
    } else {
        cout << "Успешно создана папка через create_directories: "s << p.string() << endl;
    }

    CreateFile(p / "file.txt"_p);

    path p2 = p.parent_path() / "folder2"_p;
    filesystem::create_directory(p2, err);
    if (err) {
        cout << "Ошибка создания папки через create_directory: "s << err.message() << endl;
    } else {
        cout << "Успешно создана папка через create_directory: "s << p2.string() << endl;
    }

    CreateFile(p2 / "file.txt"_p);
}

////////////////////////////////////////////
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

size_t GetFileSize(const string& file) {
    ifstream stream(file, ios::binary | ios::ate);
    if (!stream) {
        return string::npos;
    }
    size_t size = stream.tellg();
    stream.close();
    return size;
}

int main() {
    ofstream("test.txt") << "123456789"s;
    assert(GetFileSize("test.txt"s) == 9);

    ofstream test2("test2.txt"s);
    test2.seekp(1000);
    test2 << "abc"s;
    test2.flush();

    assert(GetFileSize("test2.txt"s) == 1003);
    assert(GetFileSize("missing file name"s) == string::npos);
}


//////////////////////////////////////////
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, const char** argv) {
    // 1
    {
        fstream fout("telefon.txt", ios::out);
        fout << "У меня зазвонил телефон."s << endl;
        fout << "- Кто говорит?"s << endl;
    }

    // 2
    {
        fstream fout("telefon.txt", ios::in | ios::out);
        fout.seekp(-17, ios::end);
        fout << "на линии?"s << endl << "- Слон."s << endl;
        cout << "Writing at pos: "s << fout.tellp() << endl;
    }

    // 3
    {
        fstream fin("telefon.txt", ios::in);
        string str;
        while (getline(fin, str)) {
            cout << str << endl;
        }
    }
}

////////////////////////////////////////////////////////
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

string GetLine(istream& in) {
    string s;
    getline(in, s);
    return s;
}

void CreateFiles(const string& config_file) {
    ifstream input_file(config_file);
    ofstream output_file;
    string line;

    while (getline(input_file, line)) {
        if (line[0] != '>') {
            output_file.close();
            output_file.open(line);
            continue;
        }
        output_file << line.substr(1) << endl;
    }
}

int main() {
    ofstream("test_config.txt"s) << "a.txt\n"
                                    ">10\n"
                                    ">abc\n"
                                    "b.txt\n"
                                    ">123"s;

    CreateFiles("test_config.txt"s);
    ifstream in_a("a.txt"s);
    assert(GetLine(in_a) == "10"s && GetLine(in_a) == "abc"s);

    ifstream in_b("b.txt"s);
    assert(GetLine(in_b) == "123"s);
}

//////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main() {
    {
        ifstream in_file("results.txt"s);
        int x; 
        if (in_file >> x) {
            cout << "Из файла прочитано число "s << x << endl;
        }
    }

    {
        ofstream out_file("results.txt"s);
        out_file << 100 << 500 << endl;
    }
}

/////////////////////////////////////////
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
using namespace chrono;
// хотите немного магии? тогда используйте namespace literals
using namespace literals;

int main() {
     cout << "Ожидание 5s..."s << endl;
    const auto start_time = steady_clock::now();

    // операция - ожидание 5 секунд
    this_thread::sleep_for(5s);
    const auto end_time = steady_clock::now();

    const auto dur = 10h;
    cerr << "Продолжительность"s << chrono::duration_cast<chrono::nanoseconds>(dur).count() << " ns"s << endl;

    //cout << "Ожидание завершено"s << endl;
}
/////////////////////////////////////////////////
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

using namespace std;
using namespace chrono;

void write_to_file(const string &message) {
    ofstream outputFile("results.txt");
    if (outputFile.is_open()) {
        outputFile << message << endl;
        outputFile.close();
    } else {
        cerr << "Не удалось открыть файл результатов." << endl;
    }
}

int main() {
    cout << "Ожидание 5s..." << endl;
    const auto start_time = steady_clock::now();

    // Операция - ожидание 5 секунд
    this_thread::sleep_for(seconds(5));
    const auto end_time = steady_clock::now();

    const auto dur = end_time - start_time;
    write_to_file("Продолжительность сна: " + to_string(duration_cast<milliseconds>(dur).count()) + " ms");

    cout << "Ожидание завершено" << endl;
}

/////////////////////////////////////////
// Заголовок cstdlib понадобится для функции rand,
// выдающей случайные числа.
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

vector<int> ReverseVector(const vector<int>& source_vector) {
    vector<int> res;
    for (int i : source_vector) {
        res.insert(res.begin(), i);
    }

    return res;
}

// Функция считает количество ненулевых чисел в массиве
int CountPops(const vector<int>& source_vector, int begin, int end) {
    int res = 0;

    for (int i = begin; i < end; ++i) {
        if (source_vector[i]) {
            ++res;
        }
    }

    return res;
}

void AppendRandom(vector<int>& v, int n) {
    for (int i = 0; i < n; ++i) {
        // Получаем случайное число с помощью функции rand.
        // Конструкцией (rand() % 2) получим целое число в диапазоне 0..1.
        // В C++ имеются более современные генераторы случайных чисел,
        // но в данном уроке не будем их касаться.
        v.push_back(rand() % 2);
    }
}

int main() {
    vector<int> random_bits;

    // Операция << для целых чисел это сдвиг всех бит в двоичной
    // записи числа. Запишем с её помощью число 2 в степени 17 (131072)
    static const int N = 1 << 17;

    // Заполним вектор случайными числами 0 и 1.
    AppendRandom(random_bits, N);

    // Перевернём вектор задом наперёд.
    vector<int> reversed_bits = ReverseVector(random_bits);

    // Посчитаем процент единиц на начальных отрезках вектора.
    for (int i = 1, step = 1; i <= N; i += step, step *= 2) {
        // Чтобы вычислить проценты мы умножаем на литерал 100. типа double.
        // Целочисленное значение функции CountPops при этом автоматически
        // преобразуется к double, как и i.
        double rate = CountPops(reversed_bits, 0, i) * 100. / i;
        cout << "After "s << i << " bits we found "s << rate << "% pops"s
             << endl;
    }
}

//////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <random>
//#include <set>
#include <stack>
#include <vector>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Queue {
public:

    void SwapStacks(stack<Type>& stack1, stack<Type>& stack2) {
        Type tp = stack1.top();
        stack2.push(tp);
        stack1.pop();
        if (stack1.size() != 0) {
            SwapStacks(stack1, stack2);
        }
    }

    void Push(const Type& element) {
        // напишите реализацию
        stack1_.push(element);
        stack<Type> medium = stack1_;
        stack2_ = {};
        SwapStacks(stack1_, stack2_);
        stack1_ = medium;
    }
    void Pop() {
        // напишите реализацию
        stack2_.pop();
        if (stack2_.empty()) {
            stack1_ = {};
            return;
        }
        stack<Type> medium = stack2_;
        stack1_ = {};
        SwapStacks(stack2_, stack1_);
        stack2_ = medium;
    }
    Type& Front() {
        // напишите реализацию
        return stack2_.top();
    }

    uint64_t Size() const {
        // напишите реализацию
        return stack1_.size();
    }
    bool IsEmpty() const {
        // напишите реализацию
        return stack2_.empty();
    }

private:
    stack<Type> stack1_;
    stack<Type> stack2_;
};

int main() {
    Queue<int> queue;
    vector<int> values(5);
    // заполняем вектор для тестирования очереди
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);
    PrintRange(values.begin(), values.end());
    cout << "Заполняем очередь"s << endl;
    // заполняем очередь и выводим элемент в начале очереди
    for (int i = 0; i < 5; ++i) {
        queue.Push(values[i]);
        cout << "Вставленный элемент "s << values[i] << endl;
        cout << "Первый элемент очереди "s << queue.Front() << endl;
    }
    cout << "Вынимаем элементы из очереди"s << endl;
    // выводим элемент в начале очереди и вытаскиваем элементы по одному
    while (!queue.IsEmpty()) {
        // сначала будем проверять начальный элемент, а потом вытаскивать,
        // так как операция Front на пустой очереди не определена
        cout << "Будем вынимать элемент "s << queue.Front() << endl;
        queue.Pop();
    }
    return 0;
}

//////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <random>
//#include <set>
#include <vector>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Stack {
public:
    void Push(const Type& element) {
        elements_.push_back(element);
    }
    void Pop() {
        elements_.pop_back();
    }
    const Type& Peek() const {
        return elements_.back();
    }
    Type& Peek() {
        return elements_.back();
    }
    void Print() const {
        PrintRange(elements_.begin(), elements_.end());
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        return elements_.empty();
    }

private:
    vector<Type> elements_;
};

template <typename Type>
struct PairWithMin {
    Type element;
    Type minimum;
};

template <typename Type>
ostream& operator<<(ostream& out, const PairWithMin<Type>& pair_to_print) {
    out << pair_to_print.element;
    return out;
}

template <typename Type>
class StackMin {
public:
    void Push(const Type& new_element) {
        PairWithMin<Type> new_pair = {new_element, new_element};
        if (!elements_.IsEmpty() && new_pair.minimum > elements_.Peek().minimum) {
            new_pair.minimum = elements_.Peek().minimum;
        }
        elements_.Push(new_pair);
    }
    void Pop() {
        elements_.Pop();
    }
    const Type& Peek() const {
        return elements_.Peek().element;
    }
    Type& Peek() {
        return elements_.Peek().element;
    }
    void Print() const {
        elements_.Print();
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        return elements_.IsEmpty();
    }
    const Type& PeekMin() const {
        return elements_.Peek().minimum;
    }
    Type& PeekMin() {
        return elements_.Peek().minimum;
    }

private:
    Stack<PairWithMin<Type>> elements_;
};

template <typename Type>
class SortedSack {
public:
    void Push(const Type& element) {
    // напишите реализацию метода
    
        if (elements_.IsEmpty()) {
            elements_.Push(element);
        } else {
            if (element > elements_.Peek()) {
                is_pushed_ = false;
                SwapPeekAndElement(element);
            } else {
                elements_.Push(element);
            }
        }
    }

    void SwapPeekAndElement(const Type& element) {

        Type out_element = elements_.Peek();
        elements_.Pop();
        if (elements_.IsEmpty()) {
            elements_.Push(element);
            is_pushed_ = true;
            elements_.Push(out_element);
            return;
        }
        if (element > elements_.Peek()) {
            SwapPeekAndElement(element);
        }
        if (!is_pushed_) {
            elements_.Push(element);
            is_pushed_ = true;
        }
        elements_.Push(out_element);        
    }

    void Pop() {
    // напишите реализацию метода
        elements_.Pop();
    }
    const Type& Peek() const {
        return elements_.Peek();
    }
    Type& Peek() {
        return elements_.Peek();
    }
    void Print() const {
        elements_.Print();
    }
    uint64_t Size() const {
        return elements_.Size();
    }
    bool IsEmpty() const {
        return elements_.IsEmpty();
    }
private:
    Stack<Type> elements_;
    bool is_pushed_;
};

int main() {
    SortedSack<int> sack;
    vector<int> values(5);
    // заполняем вектор для тестирования нашего класса
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);

    // заполняем класс и проверяем, что сортировка сохраняется после каждой вставки
    for (int i = 0; i < 5; ++i) {
        cout << "Вставляемый элемент = "s << values[i] << endl;
        sack.Push(values[i]);
        sack.Print();
    }
}

/////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <set>
#include <string>

using namespace std;

template <typename RandomIt>
pair<RandomIt, RandomIt> FindStartsWith(RandomIt range_begin, RandomIt range_end, string prefix) {
    // Все строки, начинающиеся с '<prefix>', больше или равны строке "<prefix>"
    auto left = lower_bound(range_begin, range_end, prefix);

    // Составим следующий в алфавите символ.
    // Не страшно, если prefix = 'z':
    //в этом случае мы получим следующий за 'z' символ в таблице символов

    string next_prefix = prefix;
    next_prefix[next_prefix.size() - 1] = next_prefix[next_prefix.size() - 1] + 1;

    // Строка "<next_prefix>" в рамках буквенных строк
    // является точной верхней гранью
    // множества строк, начнающихся с '<prefix>'
    auto right = lower_bound(range_begin, range_end, next_prefix);

    return {left, right};
}

int main() {
    const vector<string> sorted_strings = {"moscow", "motovilikha", "murmansk"};
    const auto mo_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mo");
    for (auto it = mo_result.first; it != mo_result.second; ++it) {
        cout << *it << " ";
    }
    cout << endl;
    const auto mt_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mt");
    cout << (mt_result.first - begin(sorted_strings)) << " " << (mt_result.second - begin(sorted_strings)) << endl;
    const auto na_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "na");
    cout << (na_result.first - begin(sorted_strings)) << " " << (na_result.second - begin(sorted_strings)) << endl;
    return 0;
}

/////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <set>
#include <string>

using namespace std;

template <typename RandomIt>
pair<RandomIt, RandomIt> FindStartsWith(RandomIt range_begin, RandomIt range_end, char prefix) {
    // напишите реализацию
    if (range_begin == range_end) {
        return make_pair(range_begin, range_end);
    }
    
    auto right = range_end;

    string str;
    str.push_back(prefix);

    auto left = lower_bound(range_begin, range_end, str);
    if (left == range_end) {
        return make_pair(right, right);
    }
    string str_found = *left;
    if (str_found[0] == prefix) {
        auto right = left;
        for (auto it = left; it != range_end; ++it) {
            string it_str = *(it + 1);
            if (it_str[0] == prefix) {
                right += 1;
            } else {
                return make_pair(left, right + 1);
            }
        }
    } else {
        if (left != range_end) {
            return make_pair(left, left);
        }
    }
    return make_pair(right, right);
}

int main() {
    const vector<string> sorted_strings = {"moscow", "murmansk", "vologda"};
    const auto m_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'm');
    for (auto it = m_result.first; it != m_result.second; ++it) {
        cout << *it << " ";
    }
    cout << endl;
    const auto p_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'p');
    cout << (p_result.first - begin(sorted_strings)) << " " << (p_result.second - begin(sorted_strings)) << endl;
    const auto z_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'z');
    cout << (z_result.first - begin(sorted_strings)) << " " << (z_result.second - begin(sorted_strings)) << endl;
    return 0;
}

////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <set>
#include <string>

using namespace std;

set<int>::const_iterator FindNearestElement(const set<int>& numbers, int border) {
    auto lower = numbers.lower_bound(border);
    auto before_lower = numbers.lower_bound(border);
    
    if (!numbers.empty() && border > *--numbers.end()) {
        return --numbers.end();
    }
    if (lower != numbers.begin()) {
        if ((border - *--before_lower) <= (*lower - border)) {
            return before_lower;
        } else {
            return lower;
        }
    } else {
        return lower;
    }
}

int main() {
    set<int> numbers = {1, 4, 6};
    cout <<  * FindNearestElement(numbers, 0) << " " <<  * FindNearestElement(numbers, 3) << " "
         <<  * FindNearestElement(numbers, 5) << " " <<  * FindNearestElement(numbers, 6) << " "
         <<  * FindNearestElement(numbers, 100) << endl;
    set<int> empty_set;
    cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << endl;
    return 0;
}

///////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <string>

using namespace std;

void PrintSpacesPositions(string& str) {
    // напишите реализацию
    auto it = str.begin();

    while (true) {
        it = find(it, str.end(), ' ');
        if (it == str.end()) break;
        cout << distance(str.begin(), it) << endl;
        ++it;
    }
}

int main() {
    string str = "He said: one and one and one is three"s;
    PrintSpacesPositions(str);
    return 0;
}

/////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <random>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    // 1. Если диапазон содержит меньше 2 элементов, выходим из функции
    int range_length = range_end - range_begin;
    if (range_length < 2) {
        return;
    }

    // 2. Создаем вектор, содержащий все элементы текущего диапазона
    vector<typename RandomIt::value_type> elements(range_begin, range_end);

    // 3. Разбиваем вектор на две равные части
    auto mid = elements.begin() + range_length / 2;

    // 4. Вызываем функцию MergeSort от каждой половины вектора
    MergeSort(elements.begin(), mid);
    MergeSort(mid, elements.end());

    // 5. С помощью алгоритма merge сливаем отсортированные половины
    // в исходный диапазон
    // merge -> http://ru.cppreference.com/w/cpp/algorithm/merge
    merge(elements.begin(), mid, mid, elements.end(), range_begin);
}

int main() {
    vector<int> test_vector(10);

    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);

    // shuffle   -> https://ru.cppreference.com/w/cpp/algorithm/random_shuffle
    // Перемешивает элементы в случайном порядке
    random_device rd;
    mt19937 g(rd());
    shuffle(test_vector.begin(), test_vector.end(), g);

    // Выводим вектор до сортировки
    PrintRange(test_vector.begin(), test_vector.end());

    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());

    // Выводим результат
    PrintRange(test_vector.begin(), test_vector.end());
}
////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <sstream>
#include <vector>

using namespace std;

// функция, записывающая элементы диапазона в строку
template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    if (range_begin == range_end || range_begin + 1 == range_end) {
        return;
    }
    auto mid_of_container = range_begin + (distance(range_begin, range_end) / 2);
    vector<typename RandomIt::value_type> left_part (range_begin, mid_of_container);
    vector<typename RandomIt::value_type> right_part (mid_of_container, range_end);
    MergeSort(left_part.begin(), left_part.end());
    MergeSort(right_part.begin(), right_part.end());
    merge(left_part.begin(), left_part.end(), right_part.begin(), right_part.end(), range_begin);

}

int main() {
    vector<int> test_vector(10);
    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);

    // shuffle   -> https://ru.cppreference.com/w/cpp/algorithm/random_shuffle
    // Перемешивает элементы в случайном порядке
    random_device rd;
    mt19937 g(rd());
    shuffle(test_vector.begin(), test_vector.end(), g);
    
    // Выводим вектор до сортировки
    PrintRange(test_vector.begin(), test_vector.end());
    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());
    // Выводим результат
    PrintRange(test_vector.begin(), test_vector.end());
    return 0;
}

/////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

using namespace std;

// функция, записывающая элементы диапазона в строку
template <typename It>
string PrintRangeToString(It range_begin, It range_end) {
    // удобный тип ostringstream -> https://ru.cppreference.com/w/cpp/io/basic_ostringstream
    ostringstream out;
    for (auto it = range_begin; it != range_end; ++it) {
        out << *it << " "s;
    }
    out << endl;
    // получаем доступ к строке с помощью метода str для ostringstream
    return out.str();
}

template <typename It>
vector<string> GetPermutations(It range_begin, It range_end) {
    vector<string> result;
    // Сортируем контейнер. Используем компаратор greater с параметрами 
    // по умолчанию для сортировки в обратном порядке.
    sort(range_begin, range_end, greater<>());
    // prev_permutation ->
    //     http://ru.cppreference.com/w/cpp/algorithm/prev_permutation
    // Преобразует диапазон в предыдущую (лексикографически) перестановку,
    // если она существует, и возвращает true,
    // иначе (если не существует) - в последнюю (наибольшую) и возвращает false
    do {
        result.push_back(PrintRangeToString(range_begin, range_end));
    } while (prev_permutation(range_begin, range_end));

    return result;
}

int main() {
    vector<int> permutation(3);
    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(permutation.begin(), permutation.end(), 1);
    auto result = GetPermutations(permutation.begin(), permutation.end());
    for (const auto& s : result) {
        cout << s;
    }
    return 0;
}

///////////////////////////////////////////////
#include <iostream>

using namespace std;

bool IsPowOfTwo(int number) {
    
    if (number == 0) {
        return false;
    }
    
    if (number == 1) {
        return true;
    }

    if (number % 2 == 0) {
        return IsPowOfTwo(number / 2);
    } else {
        return false;
    }
}

int main() {
    int result = IsPowOfTwo(4);
    cout << result << endl;
    return 0;
}

///////////////////////////////////////////////////////
#include <iostream>
#include <cstdint>

using namespace std;

uint64_t Fibonacci(int position) {
    if (position == 0) {
        return 0;
    } else if (position == 1) {
        return 1;
    } else {
        return Fibonacci(position - 1) + Fibonacci(position - 2);
    }
}

int main() {
    cout << "Значение числа Фибоначчи " << Fibonacci(44) << " для позиции 44" << endl;
    return 0;
}

//////////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Container>
void EraseAndPrint(Container& container, int pos, int del_begin, int del_end) {
    container.erase(container.begin() + pos);
    PrintRange(container.begin(), container.end());
    container.erase(container.begin() + del_begin, container.begin() + del_end);
    PrintRange(container.begin(), container.end());
}

int main() {
    vector<string> langs = {"Python"s, "Java"s, "C#"s, "Ruby"s, "C++"s};
    EraseAndPrint(langs, 0, 0, 2);
    return 0;
}

/////////////////////////////////////////////////
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>

using namespace std;

class Rational {
public:
    Rational() = default;

    Rational(int value)
        : numerator_(value)
        , denominator_(1) {
    }

    Rational(int numerator, int denominator)
        : numerator_(numerator)
        , denominator_(denominator) {
        if (denominator == 0) {
            throw domain_error("Denominator is zero");
        }
        Normalize();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

    Rational& operator+=(Rational right) {
        numerator_ = numerator_ * right.denominator_ + right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator-=(Rational right) {
        numerator_ = numerator_ * right.denominator_ - right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator*=(Rational right) {
        numerator_ *= right.numerator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator/=(Rational right) {
        if (right.numerator_ == 0) {
            throw invalid_argument("Division by zero");
        }
        numerator_ *= right.denominator_;
        denominator_ *= right.numerator_;
        Normalize();
        return *this;
    }

private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        int n = gcd(numerator_, denominator_);
        numerator_ /= n;
        denominator_ /= n;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

ostream& operator<<(ostream& output, Rational rational) {
    return output << rational.Numerator() << '/' << rational.Denominator();
}

istream& operator>>(istream& input, Rational& rational) {
    int numerator;
    int denominator;
    char slash;
    if ((input >> numerator) && (input >> slash) && (slash == '/') && (input >> denominator)) {
        rational = Rational{numerator, denominator};
    }
    return input;
}

Rational operator+(Rational value) {
    return value;
}

Rational operator-(Rational value) {
    return {-value.Numerator(), value.Denominator()};
}

Rational operator+(Rational left, Rational right) {
    return left += right;
}

Rational operator-(Rational left, Rational right) {
    return left -= right;
}

Rational operator*(Rational left, Rational right) {
    return left *= right;
}

Rational operator/(Rational left, Rational right) {
    return left /= right;
}

bool operator==(Rational left, Rational right) {
    return left.Numerator() == right.Numerator() && left.Denominator() == right.Denominator();
}

bool operator!=(Rational left, Rational right) {
    return !(left == right);
}

bool operator<(Rational left, Rational right) {
    return left.Numerator() * right.Denominator() < left.Denominator() * right.Numerator();
}

bool operator>(Rational left, Rational right) {
    return left.Numerator() * right.Denominator() > left.Denominator() * right.Numerator();
}

bool operator>=(Rational left, Rational right) {
    return !(left < right);
}

bool operator<=(Rational left, Rational right) {
    return !(left > right);
}

// ========== для примера ========

int main() {
    try {
        // При попытке сконструировать дробь с нулевым знаменателем
        // должно выброситься исключение domain_error
        const Rational invalid_value{1, 0};
        // Следующая строка не должна выполниться
        cout << invalid_value << endl;
    } catch (const domain_error& e) {
        cout << "Ошибка: "s << e.what() << endl;
    }
}

////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdexcept>
#include <numeric>

using namespace std;

class Rational {
public:
    Rational() = default;

    Rational(int value)
        : numerator_(value)
        , denominator_(1) {
    }

    Rational(int numerator, int denominator)
        : numerator_(numerator)
        , denominator_(denominator)
    {
        Normalize();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

    Rational& operator+=(Rational right) {
        numerator_ = numerator_ * right.denominator_ + right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator-=(Rational right) {
        numerator_ = numerator_ * right.denominator_ - right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator*=(Rational right) {
        numerator_ *= right.numerator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator/=(Rational right) {
        if (right.Denominator() == 0) {
        throw invalid_argument("Division by zero");
        }
        if (right.Numerator() == 0) {
        throw invalid_argument("Division by zero");
        }
        numerator_ *= right.denominator_;
        denominator_ *= right.numerator_;
        Normalize();
        return *this;
    }

private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        int n = gcd(numerator_, denominator_);
        numerator_ /= n;
        denominator_ /= n;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

ostream& operator<<(ostream& output, Rational rational) {
    return output << rational.Numerator() << '/' << rational.Denominator();
}

istream& operator>>(istream& input, Rational& rational) {
    int numerator;
    int denominator;
    char slash;
    if ((input >> numerator) && (input >> slash) && (slash == '/') && (input >> denominator)) {
        rational = Rational{numerator, denominator};
    }
    return input;
}

// Unary plus and minus

Rational operator+(Rational value) {
    return value;
}

Rational operator-(Rational value) {
    return {-value.Numerator(), value.Denominator()};
}

// Binary arithmetic operations

Rational operator+(Rational left, Rational right) {
    return left += right;
}

Rational operator-(Rational left, Rational right) {
    return left -= right;
}

Rational operator*(Rational left, Rational right) {
    return left *= right;
}

Rational operator/(Rational left, Rational right) {
    return left /= right;
}

// Comparison operators

bool operator==(Rational left, Rational right) {
    return left.Numerator() == right.Numerator() &&
           left.Denominator() == right.Denominator();
}

bool operator!=(Rational left, Rational right) {
    return !(left == right);
}

bool operator<(Rational left, Rational right) {
    return left.Numerator() * right.Denominator() < 
           left.Denominator() * right.Numerator();
}

bool operator>(Rational left, Rational right) {
    return left.Numerator() * right.Denominator() >
           left.Denominator() * right.Numerator();
}

bool operator>=(Rational left, Rational right) {
    return !(left < right);
}

bool operator<=(Rational left, Rational right) {
    return !(left > right);
}
int main() {
    try {
        const Rational three_fifth{3, 5};
        const Rational zero;
        cout << three_fifth << " / " << zero << " = " << (three_fifth / zero) << endl;
    } catch (const invalid_argument& e) {
        cout << "Ошибка: "s << e.what() << endl;
    }
    try {
        Rational value{3, 5};
        value /= Rational();
        // Следующая строка не должна выполниться
        cout << value << endl;
    } catch (const invalid_argument& e) {
        cout << "Ошибка: "s << e.what() << endl;
    }
}
/////////////////////////////////////////////////////////////
#include <iostream>
#include <numeric>
#include <string>

using namespace std;

class Rational {
public:
    Rational() = default;

    Rational(int value)
        : numerator_(value)
        , denominator_(1)
    {
    }

    Rational(int numerator, int denominator)
        : numerator_(numerator)
        , denominator_(denominator)
    {
        Normalize();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

    Rational& operator+=(Rational right) {
        numerator_ = numerator_ * right.denominator_ + right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator-=(Rational right) {
        numerator_ = numerator_ * right.denominator_ - right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator*=(Rational right) {
        numerator_ *= right.numerator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator/=(Rational right) {
        numerator_ *= right.denominator_;
        denominator_ *= right.numerator_;
        Normalize();
        return *this;
    }

private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        int n = gcd(numerator_, denominator_);
        numerator_ /= n;
        denominator_ /= n;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

ostream& operator<<(ostream& output, Rational rational) {
    return output << rational.Numerator() << '/' << rational.Denominator();
}

istream& operator>>(istream& input, Rational& rational) {
    int numerator;
    int denominator;
    char slash;
    if ((input >> numerator) && (input >> slash) && (slash == '/') && (input >> denominator)) {
        rational = Rational{numerator, denominator};
    }
    return input;
}

Rational operator+(Rational left, Rational right) {
    left += right;
    return left;
    //const int numerator = left.Numerator() * right.Denominator() + right.Numerator() * left.Denominator();
    //const int denominator = left.Denominator() * right.Denominator();
    //return {numerator, denominator};
}

Rational operator-(Rational left, Rational right) {
    left -= right;
    return left;
    //const int numerator = left.Numerator() * right.Denominator() - right.Numerator() * left.Denominator();
    //const int denominator = left.Denominator() * right.Denominator();
    //return {numerator, denominator};
}

Rational operator+(Rational value) {
    return value;
}

Rational operator-(Rational value) {
    return {-value.Numerator(), value.Denominator()};
}

Rational operator*(Rational left, const Rational right) {
    left *= right;
    return left;
    //const int numerator = left.Numerator() * right.Numerator();
    //const int denominator = left.Denominator() * right.Denominator();
    //return {numerator, denominator};
    }

Rational operator/(Rational left, const Rational right) {
    left /= right;
    return left;
    //const int numerator = left.Numerator() * right.Denominator();
    //const int denominator = left.Denominator() * right.Numerator();
    //return {numerator, denominator};
    } 
bool operator==(Rational left, Rational right) {
    return left.Numerator() == right.Numerator() && 
           left.Denominator() == right.Denominator();
}

bool operator!=(Rational left, Rational right) {
    return !(left == right);
}

bool operator<(const Rational left, const Rational right) {
    return left.Numerator() * right.Denominator() < right.Numerator() * left.Denominator();
}

bool operator>(const Rational left, const Rational right) {
    return left.Numerator() * right.Denominator() > right.Numerator() * left.Denominator();
}

bool operator<=(const Rational left, const Rational right) {
    return !(left > right);
}

bool operator>=(const Rational left, const Rational right) {
    return !(left < right);
}
// Rational& operator+=(Rational& left, const Rational& right) {
//    left = left + right;
//    return left;
//}

//Rational& operator-=(Rational& left, const Rational& right) {
//    left = left - right;
//    return left;
//}

//Rational& operator*=(Rational& left, const Rational& right) {
//    left = left * right;
//    return left;
//}

//Rational& operator/=(Rational& left, const Rational& right) {
//    left = left / right;
//    return left;
//}

int main() {
    Rational scale1; // (-6, -24)
    Rational scale2;
    cin >> scale1 >> scale2;
    scale1 += scale2;
    cout << scale1 << endl;
}

///////////////////////////////////////////////////////
#include <iostream>
#include <numeric>

using namespace std;

class Rational {
public:
    Rational() = default;

    Rational(int numerator)
        : numerator_(numerator)
        , denominator_(1) {
    }

    Rational(int numerator, int denominator)
        : numerator_(numerator)
        , denominator_(denominator) {
        Normalize();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        const int divisor = gcd(numerator_, denominator_);
        numerator_ /= divisor;
        denominator_ /= divisor;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

// вывод
ostream& operator<<(ostream& output, Rational rational) {
    output << rational.Numerator() << "/"s << rational.Denominator();
    return output;
}

// ввод
istream& operator>>(istream& input, Rational& rational) {
    int n, d;
    char slash; // переменная для считывания косой черты
    input >> n >> slash >> d;
    rational = Rational(n, d);
    return input;
}

Rational operator+(Rational left, Rational right) {
    const int numerator = left.Numerator() * right.Denominator() 
                  + right.Numerator() * left.Denominator();
    const int denominator = left.Denominator() * right.Denominator();

    return {numerator, denominator};
}

Rational operator-(Rational left, Rational right) {
    const int numerator = left.Numerator() * right.Denominator() 
                  - right.Numerator() * left.Denominator();
    const int denominator = left.Denominator() * right.Denominator();

    return {numerator, denominator};
}

Rational operator+(Rational rational) {
    return rational;
}

Rational operator-(Rational rational) {
    return {-rational.Numerator(), rational.Denominator()};
}

int main() {
    Rational scale1; // (-6, -24)
    Rational scale2;
    cin >> scale1 >> scale2;
    Rational scale3 = scale1 + scale2;
    cout << scale3 << endl;

}

///////////////////////////////////////////////////////////////////
#include <cmath>
#include <iostream>
#include <vector>
#include <set>
#include <string>

using namespace std;

class Greeter {
public:
    Greeter(const string& name)
        : name_(name) 
    {
        cout << "Hello, "s << name_ << "!"s << endl;
    }

    ~Greeter() {
        // Поля класса внутри деструктора еще не уничтожены, и к ним можно обращаться
        cout << "Goodbye, "s << name_ << "!"s << endl;
    }
private:
    string name_;
};

class BigGreeter {
public:
    BigGreeter(const string& name1, const string& name2, const string& name3)
        : greeter3_{name1}, greeter2_{name2}, greeter1_{name3} {}
private:
    Greeter greeter1_;
    Greeter greeter2_;
    Greeter greeter3_;
};



int main() {
    BigGreeter bigGreeter{"Alice", "Bob", "Charlie"};
    return 0;
}

/////////////////////////////////////////////////////////////////////
#include <iostream>
#include <numeric>

using namespace std;

class Cat {
public:
    explicit Cat(const string& name)
        : name_(name)
    {
    }
    const string& GetName() const {
        return name_;
    }
private:
    string name_;
};

void Feed(const Cat& cat) {
    cout << cat.GetName() << ", eat some milk"s << endl;
}

int main() {
    Cat cat1{"Матроскин"s};
    Feed(cat1); // Выглядит нормально
    Feed(Cat{"Леопольд"s}); // И это тоже

    // Следующие 2 строки выглядят странно: мы кормим какой-то объект и строку
    //Feed({"Том"s});
    //Feed("Котёнок по имени Гав"s);
}
////////////////////////////////////////////////////////////////////
#include <iostream>
#include <numeric>

using namespace std;

class Rational {
public:
    Rational() = default;

    Rational(int numerator) : numerator_(numerator), denominator_(1) {
    }

    Rational(int numerator, int denominator) : numerator_(numerator), denominator_(denominator) {
        Normalize();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        const int divisor = gcd(numerator_, denominator_);
        numerator_ /= divisor;
        denominator_ /= divisor;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

int main() {
    const Rational scale(-6, -24);
    //const Rational scale;

    cout << scale.Numerator() << "/"s << scale.Denominator() << endl;
}


/////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>

using namespace std;

class Cat {
public:
    Cat(const string& name)
        : name_(name)
    {
    }
    string GetName() const {
        return name_;
    }
private:
    string name_;
};

class Witch {
public:
    Witch(const string& catName)
        : cat_(catName)
    {
        cout << "Моего кота зовут "s << cat_.GetName() << endl;
    }
private:
    Cat cat_;
};

int main() {
    Witch hermione{"Живоглот"s};
}

////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

class Cat {
public:
    Cat(const string& name)
        : name_(name)
    {
    }
    string GetName() const {
        return name_;
    }
private:
    string name_;
};

class Witch {
public:
    Witch(const string& catName)
        : cat_(catName)
    {
        cout << "Моего кота зовут "s << cat_.GetName() << endl;
    }
private:
    Cat cat_;
};

int main() {
    Witch hermione{"Живоглот"s};
}

///////////////////////////////////////////////////////
#include <numeric>
#include <iostream>

using namespace std;

class Rational {
public:
    // Явно сообщаем компилятору, что в этом классе нужно создать конструктор по умолчанию
    Rational() = default;

    // Параметризованный конструктор

    Rational(int numerator) {
        numerator_ = numerator;
        denominator_ = 1;
    }

    Rational(int numerator, int denominator) {
        if (denominator != 0) {
            bool subz = false;
            //bool den_subz = false;
            if (numerator < 0) {
                numerator *= -1;
                subz = !subz;
            }

            if (denominator < 0) {
                denominator *= -1;
                subz = !subz;
            }
            int g_c_d = gcd(numerator, denominator);
            numerator = numerator / g_c_d;
            denominator = denominator / g_c_d;
            if (subz) {
                numerator_ = -numerator;
            } else {
                numerator_ = numerator;
            }

            denominator_ = denominator;
            
        }

    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

private:
    // Задаём значения по умолчанию для числителя и знаменателя
    int numerator_ = 0;
    int denominator_ = 1;
};

int main() {
    const Rational scale(-6, -24);
    //const Rational scale;

    cout << scale.Numerator() << "/"s << scale.Denominator() << endl;

    //cout << gcd(24, 12) << endl;
}

//////////////////////////////////////////////////////////////////////////
#include <string>

using namespace std;

enum class AnimalType {
    Cat,
    Dog,
    Mouse,
};

class Animal {
public:
    
    Animal (AnimalType animal_type, string name, string owner_name) {
        type_ = animal_type;
        name_ = name;
        owner_name_ = owner_name;
    }
    
    AnimalType GetType() const {
        return type_;
    }

    const string& GetName() const {
        return name_;
    }

    const string& GetOwnerName() const {
        return owner_name_;
    }

private:
    AnimalType type_;
    string name_;
    string owner_name_;
};

//////////////////////////////////////////////////////////////////////////////////////
#include <iostream>

using namespace std;

class Rational {
public:
    // Явно сообщаем компилятору, что в этом классе нужно создать конструктор по умолчанию
    Rational() = default;

    // Параметризованный конструктор
    Rational(int numerator, int denominator) {
        numerator_ = numerator;
        denominator_ = denominator;
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

private:
    // Задаём значения по умолчанию для числителя и знаменателя
    int numerator_ = 0;
    int denominator_ = 1;
};

int main() {
    const Rational scale(1, 3);
    //const Rational scale;

    cout << scale.Numerator() << "/"s << scale.Denominator() << endl;
}

///////////////////////////////////////////////////////////////////////
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

using namespace std;

class Synonyms {
public:
    void Add(const string& first_word, const string& second_word) {
        synonyms_[first_word].insert(second_word);
        synonyms_[second_word].insert(first_word);
    }

    size_t GetSynonymCount(const string& word) const {
        if (synonyms_.count(word) != 0) {
            return synonyms_.at(word).size();
        }
        return 0;
    }

    bool AreSynonyms(const string& first_word, const string& second_word) const {
        if (synonyms_.count(first_word)) {
            if (synonyms_.at(first_word).count(second_word)) {
                return true;
            }
        }
        return false;
    }

private:
    map<string, set<string>> synonyms_;
};

void TestAddingSynonymsIncreasesTheirCount() {
    Synonyms synonyms;
    assert(synonyms.GetSynonymCount("music"s) == 0);
    assert(synonyms.GetSynonymCount("melody"s) == 0);

    synonyms.Add("music"s, "melody"s);
    assert(synonyms.GetSynonymCount("music"s) == 1);
    assert(synonyms.GetSynonymCount("melody"s) == 1);

    synonyms.Add("music"s, "tune"s);
    assert(synonyms.GetSynonymCount("music"s) == 2);
    assert(synonyms.GetSynonymCount("tune"s) == 1);
    assert(synonyms.GetSynonymCount("melody"s) == 1);
}

void TestAreSynonyms() {
    Synonyms synonyms;

    assert(!synonyms.AreSynonyms("winner"s, "champion"s));

    synonyms.Add("winner"s, "champion"s);

    assert(synonyms.AreSynonyms("winner"s, "champion"s));
    assert(synonyms.AreSynonyms("champion"s, "winner"s));
    assert(!synonyms.AreSynonyms("winner"s, "dinner"s));
    assert(!synonyms.AreSynonyms("dinner"s, "winner"s));

    synonyms.Add("good"s, "nice"s);

    assert(!synonyms.AreSynonyms("good"s, "champion"s));
    assert(!synonyms.AreSynonyms("good"s, "winner"s));
    assert(!synonyms.AreSynonyms("nice"s, "winner"s));
    assert(synonyms.AreSynonyms("good"s, "nice"s));

    assert(!synonyms.AreSynonyms("music"s, "tune"s));
}

void TestSynonyms() {
    TestAddingSynonymsIncreasesTheirCount();
    TestAreSynonyms();
}

int main() {
    TestSynonyms();

    Synonyms synonyms;

    string line;
    while (getline(cin, line)) {
        istringstream command(line);
        string action;
        command >> action;

        if (action == "ADD"s) {
            string first_word, second_word;
            command >> first_word >> second_word;
            synonyms.Add(first_word, second_word);
        } else if (action == "COUNT"s) {
            string word;
            command >> word;
            cout << synonyms.GetSynonymCount(word) << endl;
        } else if (action == "CHECK"s) {
            string first_word, second_word;
            command >> first_word >> second_word;
            if (synonyms.AreSynonyms(first_word, second_word)) {
                cout << "YES"s << endl;
            } else {
                cout << "NO"s << endl;
            }
        } else if (action == "EXIT"s) {
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Определяет, будет ли слово палиндромом
// text может быть строкой, содержащей строчные буквы английского алфавита и пробелы
// Пустые строки и строки, состоящие только из пробелов, - это не палиндромы
bool IsPalindrome(const string& text) {
    vector<char> letters, inverse_letters;
    for (const char c : text) {
        if (c != ' ') {
            letters.push_back(c);
        }
    }
    if (!letters.empty()) {
        inverse_letters = letters;
        reverse(inverse_letters.begin(), inverse_letters.end());
        if (letters == inverse_letters) {
        return true;
        }
    }
    return false;    
}

int main() {
    string text;
    getline(cin, text);

    if (IsPalindrome(text)) {
        cout << "palindrome"s << endl;
    } else {
        cout << "not a palindrome"s << endl;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

struct Animal {
    string name;
    int age;
    double weight;
};

enum class AnimalSortKey {
    AGE,     // по полю age
    WEIGHT,  // по полю weight
    RELATIVE_WEIGHT  // по weight / age
};

// template <typename Container, typename KeyMapper>
// void SortBy(Container& container, KeyMapper key_mapper, bool reverse = false) {
//    sort(container.begin(), container.end(), [key_mapper, reverse](const auto& lhs, const auto& rhs) {
//        if (reverse) {
//            return key_mapper(lhs) > key_mapper(rhs);
//        } else {
//            return key_mapper(lhs) < key_mapper(rhs);
//        }
//    });
//}

template <typename Container, typename KeyMapper>
void SortBy(Container& container, KeyMapper key_mapper, bool reverse = false) {
        // если KeyMapper — это AnimalSortKey...
    if constexpr (is_same_v<KeyMapper, AnimalSortKey>) {
        switch (key_mapper) {
            case AnimalSortKey::AGE:
                return SortBy(container, [](const auto& x) { return x.age; }, reverse);
            case AnimalSortKey::WEIGHT:
                return SortBy(container, [](const auto& x) { return x.weight; }, reverse);
            case AnimalSortKey::RELATIVE_WEIGHT:
                return SortBy(container, [](const auto& x) { return x.weight / x.age; }, reverse);
        }
                // вышли из функции, остальное снаружи if
    } else {
        if (reverse) {
            sort(container.begin(), container.end(),
             [key_mapper](const auto& lhs, const auto& rhs) {
                return key_mapper(lhs) > key_mapper(rhs);
             });
        } else {
            sort(container.begin(), container.end(),
             [key_mapper](const auto& lhs, const auto& rhs) {
                return key_mapper(lhs) < key_mapper(rhs);
             });
        }
    }

}

void PrintNames(const vector<Animal>& animals) {
    for (const Animal& animal : animals) {
        cout << animal.name << ' ';
    }
    cout << endl;
}

int main() {
    vector<Animal> animals = {
        {"Мурка"s,   10, 5},
        {"Белка"s,   5,  1.5},
        {"Георгий"s, 2,  4.5},
        {"Рюрик"s,   12, 3.1},
    };
    PrintNames(animals);
    SortBy(animals, [](const Animal& animal) { return animal.name; }, true);
    PrintNames(animals);
    SortBy(animals, [](const Animal& animal) { return animal.weight; });
    PrintNames(animals);
    return 0;
}


#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;
template <typename AnyType>
ostream& operator<<(ostream& out, const set<AnyType>& container);
template <typename AnyType>
ostream& operator<<(ostream& out, const vector<AnyType>& container);
template <typename AnyKey, typename AnyValue>
//ostream& operator<<(ostream& out, const pair<AnyKey, AnyValue>& container);
//template <typename AnyKey, typename AnyValue>
ostream& operator<<(ostream& out, const map<AnyKey, AnyValue>& container);

template <typename AnyKey, typename AnyValue>
ostream& operator<<(ostream& out, const pair<AnyKey, AnyValue>& container) {
    out << "(";
    out << container.first;
    out << ", ";
    out << container.second;    
    out << ")";
    return out;
}

//template <typename AnyKey, typename AnyValue>
//void PrintPair(ostream& out, const pair<AnyKey, AnyValue>& container) {
 //   out << "(";
 //   out << container.first;
  //  out << ", ";
  //  out << container.second;    
  //  out << ")";
//}

template <typename Container>
void Print(ostream& out, const Container& container) {
    bool is_first = true;
    for (const auto& element : container) {
        if (!is_first) {
            out << ", "s;
        }
        is_first = false;
        out << element;
    }
}

template <typename AnyType>
ostream& operator<<(ostream& out, const set<AnyType>& container) {
    out << "{";
    Print(out, container);
    out << "}";
    return out;
}

template <typename AnyType>
ostream& operator<<(ostream& out, const vector<AnyType>& container) {
    out << "[";
    Print(out, container);
    out << "]";
    return out;
}

template <typename AnyKey, typename AnyValue>
ostream& operator<<(ostream& out, const map<AnyKey, AnyValue>& container) {
    out << "<";
    Print(out, container);
    out << ">";
    return out;
}

int main() {
const set<string> cats = {"Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s};
cout << cats << endl;
const vector<int> ages = {10, 5, 2, 12};
cout << ages << endl;
const map<string, int> cat_ages = {
    {"Мурка"s, 10}, 
    {"Белка"s, 5},
    {"Георгий"s, 2}, 
    {"Рюрик"s, 12}
};
cout << cat_ages << endl;
}

//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>

using namespace std;

template <typename AnyType>

ostream& operator<<(ostream& out, const vector<AnyType>& container) {
    size_t i = 0;
    for (const AnyType& element : container) {
        ++i;
        if (container.size() == i) {
            out << element;
        } else {
            out << element << ", "s;
        }
    }
    return out;
}  
   
int main() {
    const vector<int> ages = {10, 5, 2, 12};
    cout << ages << endl;
    return 0;
}

//////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

template <typename Document, typename Term>

vector<double> ComputeTfIdfs (Document documents, Term term) {
    vector<double> tf_idf;
    double tf = 0;
    double idf = 0;
    int idf_count = 0;
    double inv_term_count = 0;
    for (const auto& document : documents) {
        if (!document.empty() && (count(document.begin(), document.end(), term)) > 0) {
            inv_term_count = 1.0 / document.size();
            tf = inv_term_count * count(document.begin(), document.end(), term);
            ++idf_count;
        } else {
            tf = 0;
        }
        tf_idf.push_back(tf);
    }
    if (idf_count > 0) {
        idf = log(static_cast<double>(documents.size()) / static_cast<double>(idf_count));
    }
    for (auto& tf : tf_idf) {
        tf = tf * idf;
    }
        
    return tf_idf;
}

int main() {
    const vector<vector<string>> documents = {
        {"белый"s, "кот"s, "и"s, "модный"s, "ошейник"s},
        {"пушистый"s, "кот"s, "пушистый"s, "хвост"s},
        {"ухоженный"s, "пёс"s, "выразительные"s, "глаза"s},
    };
    const auto tf_idfs = ComputeTfIdfs(documents, "кот"s);
    for (const double tf_idf : tf_idfs) {
        cout << tf_idf << " "s;
    }
    cout << endl;
    return 0;
}

////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;
template <typename Term>
map<Term, int> ComputeTermFreqs(const vector<Term>& terms) {
    map<Term, int> term_freqs;
    for (const Term& term : terms) {
        ++term_freqs[term];
    }
    return term_freqs;
}

struct Animal {
    string name;
    int age;
    int freq;
};

pair<string, int> FindMaxFreqAnimal(const vector<pair<string, int>>& animals) {
    // верните животного с максимальной частотой
    map<pair<string, int>, int> map_of_animals_and_freqs;
    vector<Animal> animals_and_freqs;
    pair<string, int> max_freq_animal;
    for (const auto& animal : animals) {
        ++map_of_animals_and_freqs[animal];
    }

    for (const auto& [name_and_age, freq] : map_of_animals_and_freqs) {
        animals_and_freqs.push_back({name_and_age.first, name_and_age.second, freq});
    }
    
    sort(animals_and_freqs.begin(), animals_and_freqs.end(),
        [] (const Animal& lhs, const Animal& rhs) {
            if (lhs.freq == rhs.freq) {
                return lhs.name < rhs.name;
            } else {
                return lhs.freq > rhs.freq;
            }
    });
    max_freq_animal.first = animals_and_freqs[0].name;
    max_freq_animal.second = animals_and_freqs[0].age;
    return max_freq_animal;
}

int main() {
    const vector<pair<string, int>> animals = {
        {"Murka"s, 5},  // 5-летняя Мурка
        {"Belka"s, 6},  // 6-летняя Белка
        {"Murka"s, 7},  // 7-летняя Мурка не та же, что 5-летняя!
        {"Murka"s, 5},  // Снова 5-летняя Мурка
        {"Belka"s, 6},  // Снова 6-летняя Белка
    };
    const pair<string, int> max_freq_animal = FindMaxFreqAnimal(animals);
    cout << max_freq_animal.first << " "s << max_freq_animal.second << endl;
}

////////////////////////////////////////////////////////
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

map<string, int> ComputeTermFreqs(const vector<string>& terms) {
    // реализуйте функцию
    map<string, int> words_term_freqs;
    for (const string& word : terms) {
        ++words_term_freqs[word];
    }
    return words_term_freqs;
}

int main() {
    const vector<string> terms = {"first"s, "time"s, "first"s, "class"s};
    for (const auto& [term, freq] : ComputeTermFreqs(terms)) {
        cout << term << " x "s << freq << endl;
    }
    // вывод:
    // class x 1
    // first x 2
    // time x 1
}

///////////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

enum class Status { ACTUAL, EXPIRED, DELETED };

struct Document {
    int id;
    Status status;
    double relevance;
    int rating;
    auto MakeKey() const {
        return tuple(status, rating * -1, relevance* -1);
    }
};

void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](auto& lhs, auto& rhs) {
            return lhs.MakeKey() < rhs.MakeKey();
         });
}

int main() {
    vector<Document> documents = {
        {100, Status::ACTUAL, 0.5, 4}, {101, Status::EXPIRED, 0.5, 4},
        {102, Status::ACTUAL, 1.2, 4}, {103, Status::DELETED, 1.2, 4},
        {104, Status::ACTUAL, 0.3, 5},
    };
    SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << static_cast<int>(document.status) << ' ' << document.relevance
             << ' ' << document.rating << endl;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

enum class Status { ACTUAL, EXPIRED, DELETED };

struct Document {
    int id;
    Status status;
    double relevance;
    int rating;
};

void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
            return tuple(lhs.status, -lhs.rating, -lhs.relevance) < tuple(rhs.status, -rhs.rating, -rhs.relevance);
         });
}

int main() {
    vector<Document> documents = {
        {100, Status::ACTUAL, 0.5, 4}, {101, Status::EXPIRED, 0.5, 4},
        {102, Status::ACTUAL, 1.2, 4}, {103, Status::DELETED, 1.2, 4},
        {104, Status::ACTUAL, 0.3, 5},
    };
    SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << static_cast<int>(document.status) << ' ' << document.relevance
             << ' ' << document.rating << endl;
    }

    return 0;
}
////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

enum class Status { ACTUAL, EXPIRED, DELETED };

struct Document {
    int id;
    Status status;
    double relevance;
    int rating;
};

void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
            if (lhs.status == rhs.status) {
                return pair(lhs.rating, lhs.relevance) > pair(rhs.rating, rhs.relevance);
            } else {
                return (lhs.status < rhs.status);
            }
            
         });
}

int main() {
    vector<Document> documents = {
        {100, Status::ACTUAL, 0.5, 4}, {101, Status::EXPIRED, 0.5, 4},
        {102, Status::ACTUAL, 1.2, 4}, {103, Status::DELETED, 1.2, 4},
        {104, Status::ACTUAL, 0.3, 5},
    };
    SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << static_cast<int>(document.status) << ' ' << document.relevance
             << ' ' << document.rating << endl;
    }

    return 0;
}

///////////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

struct Document {
    int id;
    double relevance;
    int rating;
};

void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
                return pair(lhs.rating, lhs.relevance) > pair(rhs.rating, rhs.relevance);
         });
}

int main() {
    vector<Document> documents = {{100, 0.5, 4}, {101, 1.2, 4}, {102, 0.3, 5}};
    SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << document.relevance << ' ' << document.rating << endl;
    }

    return 0;
}

sort(observations.begin(), observations.end(),
     [](const AnimalObservation& lhs, const AnimalObservation& rhs) {
          return lhs.days_ago < rhs.days_ago
               || (lhs.days_ago == rhs.days_ago
               && lhs.health_level < rhs.health_level);
     }); 



///////////////////////////////////////////////////////////////////////////
#include <iostream>

using namespace std;

class SearchServer {
    public:
        // перенесите сюда DocumentStatus
        enum class DocumentStatus {
        ACTUAL,
        IRRELEVANT,
        BANNED,
        REMOVED,
    };
};

int main() {
    // выведите численное значение статуса BANNED
    int banned = static_cast<int>(SearchServer::DocumentStatus::BANNED);
    cout << banned<< endl;
    return 0;
}


////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <vector>
//#include <cstdint>
 
using namespace std;
 
struct Document {
    int id;
    int rating;
};
 
void PrintDocuments(vector<Document> documents, size_t skip_start, size_t skip_finish) {
    //size_t doc_size = documents.size();
    if (documents.size() > 1) {
        sort(documents.begin(), documents.end(), [](const Document& lhs, const Document& rhs) {
        return lhs.rating > rhs.rating;
        });
 
        for (size_t i = 0; i < skip_finish; ++i) {
            documents.pop_back();
            if (documents.size() == 0) {
            break;
            }
        }
    
        for (size_t i = 0; i < skip_start; ++i) {
            documents.erase(documents.begin());
            if (documents.size() == 0) {
                break;
            }
        }
        
        if (documents.size() > 0) {
            for (auto i : documents) {
                cout << "{ id = "s << i.id << ", "s << "rating = "s << i.rating << " }"s << endl;
            }
        }
    }
}
 
int main() {
    PrintDocuments({{100, 5}, {101, 7}, {102, -4}, {103, 9}, {104, 1}}, 1, 2);
}

/////////////////////////////////////////////////
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

int CountAndAddNewDogs(const vector<string>& new_dogs, const map<string, int>& max_amount,
                       map<string, int>& shelter) {
    
    return count_if(new_dogs.begin(), new_dogs.end(), [&max_amount, &shelter](const string& n_d) {
        if (shelter[n_d] == max_amount.at(n_d)) {
            return false;
        } else if (shelter.count(n_d) == 0) {
            shelter[n_d] = 1;
            return true;
        } else if (shelter[n_d] < max_amount.at(n_d)) {
            ++shelter[n_d];
            return true;
        }
        return false;       
        }
    );
    
}

int main() {
    map<string, int> shelter {
        {"shepherd"s, 1},
        {"corgi"s, 3},
    };

    const map<string, int> max_amount {
        {"shepherd"s, 2},
        {"corgi"s, 3},
        {"shiba inu"s, 1},
    };

    cout << CountAndAddNewDogs({"shepherd"s, "shiba inu"s, "shiba inu"s, "corgi"s}, max_amount, shelter) << endl;
}

///////////////////////////////////////////////////
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int CountAndAddNewDogs(const vector<string>& new_dogs,
                       const map<string, int>& max_amount,
                       map<string, int>& shelter) {
    return count_if(new_dogs.begin(), new_dogs.end(),
        [&max_amount, &shelter](const string& name) {
            int& current_amount = shelter[name];
            if (current_amount < max_amount.at(name)) {
                ++current_amount;
                return true;
            } else {
                return false;
            }
        });
}

int main() {
    map<string, int> shelter {
        {"shepherd"s, 1},
        {"corgi"s, 3},
    };

    map<string, int> max_amount {
        {"shepherd"s, 2},
        {"corgi"s, 3},
        {"shiba inu"s, 1},
    };

    cout << CountAndAddNewDogs({"shepherd"s, "shiba inu"s, "shiba inu"s, "corgi"s}, max_amount, shelter) << endl;
}
*/