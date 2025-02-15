#include <cassert>
//#include <stdexcept>
#include <sstream>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

using namespace std::literals;
using namespace std;

class Gold {
public:
    // Чтобы скопировать золото, нужно его добыть
    Gold(const Gold& other) {
        *this = Mine(other.amount_);
    }

    // Просто перемещаем золото из одного места в другое
    Gold(Gold&& other) noexcept : amount_(other.amount_)  {
        other.amount_ = 0;
    }
    
    Gold& operator=(const Gold& rhs) {
        if (this != &rhs) {
            Dispose();  // Возвращаем прежнее количество золота
            *this = Mine(rhs.amount_); // И добываем новое
        }
        return *this;
    }

    Gold& operator=(Gold&& rhs) noexcept {
        Dispose(); // Возвращаем прежнее количества золота
        // ... и забираем содержимое rhs
        amount_ = rhs.amount_;
        rhs.amount_ = 0;
        return *this;
    }

    ~Gold() {
        Dispose();
    }

    int GetAmount() const noexcept {
        return amount_;
    }

    static int GetAvailableAmount() {
        return available_;
    }

    static Gold Mine(int amount) {
        if (amount <= 0) {
            throw std::out_of_range("Invalid amount of gold."s);
        }
        cout << "Mine "sv << amount << " units of gold. "sv;
        if (amount > available_) {
            // Недостаточно золота в недрах планеты
            ostringstream msg;
            msg << "Out of gold. Only "sv << available_ << " units are available."sv;
            throw runtime_error(msg.str());
        }
        available_ -= amount;
        cout << available_ << " units left."sv << endl;
        return Gold(amount);
    }

    [[nodiscard]] Gold Take(int amount) {
        if (amount > amount_ || amount < 0) {
            throw std::invalid_argument("Invalid gold amount"s);
        }
        amount_ -= amount;
        return Gold(amount);
    }
    
private:
    // Количество золота в недрах Земли
    static inline int available_ = 100;
    int amount_ = 0;

    // Конструктор объявлен приватным, чтобы создание золота было возможным только путём его добычи
    explicit Gold(int amount) noexcept
        : amount_(amount) {
    }

    void Dispose() noexcept {
        if (amount_ != 0) {
            available_ += amount_;
            cout << "Dispose "sv << amount_ << " units of gold. "sv << available_
                 << " units available."sv << endl;
            amount_ = 0;
        }
    }
};

class Person {
public:
    Person(string name, Gold gold)
        : name_(move(name))
        , gold_(move(gold))  //
    {
        cout << "Person "sv << name_ << " was created with "sv << gold_.GetAmount()
             << " units of gold."sv << endl;
    }

    // Запрещаем клонирование людей
    Person(const Person& other) = delete;
    Person& operator=(const Person&) = delete;

    ~Person() {
        cout << "Person "sv << name_ << " died."sv << endl;
    }
    
    // Метод вызывается у объектов, доступных по обычной lvalue-ссылке
    [[nodiscard]] const Gold& GetGold() const& {
        return gold_;
    }

    // Метод вызывается у объектов, доступных по rvalue-ссылке
    [[nodiscard]] Gold GetGold() && {
        return std::move(gold_);
    }
private:
    string name_;
    Gold gold_;
};        

Person MakePersonWithGold() {
    return Person("Scrooge McDuck"s, Gold::Mine(60)); // Было раньше 50
}

int main() {
    try {
        Gold gold = move(MakePersonWithGold().GetGold());
        Person chip("Chip"s, gold.Take(20));
        Person dale("Dale"s, gold.Take(20));
        Person donald("Donald Duck"s, gold.Take(20)); // Было раньше 10
    } catch (const exception& e) {
        cout << e.what() << endl;
    }

    Person person("Grigory Rasputin"s, Gold::Mine(10));
    // Объект g1 будет инициализирован конструктором копирования
    Gold g1 = person.GetGold();

    // g2 будет проинициализирован конструктором перемещения
    Gold g2 = Person("Ivan Susanin"s, Gold::Mine(10)).GetGold();

    // g3 будет проинициализирован конструктором перемещения
    Gold g3 = move(person).GetGold();
    assert(person.GetGold().GetAmount() == 0); // Золото перемещено от объекта person в g3

    // Ссылка g4 продлит жизнь золотому слитку, полученному у лепрекона до конца блока
    const Gold& g4 = Person("Leprechaun"s, Gold::Mine(15)).GetGold();
    assert(g4.GetAmount() == 15);
} 