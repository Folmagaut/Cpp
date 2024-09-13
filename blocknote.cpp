#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
using namespace chrono;
using namespace literals;

class LogDuration {
public:
    LogDuration() {
    }

    ~LogDuration() {
        // эта переменная сохранит время уничтожения объекта:
        const auto end_time = steady_clock::now();
        const auto dur = end_time - start_time_;
        cerr << duration_cast<milliseconds>(dur).count() << " ms"s << endl;
    }

private:
    // Переменная будет инициализирована текущим моментом времени при
    // создании объекта класса.
    // Таким образом, она хранит время создания объекта.
    const steady_clock::time_point start_time_ = steady_clock::now();
};

int main() {
    cout << "Ожидание 5s..."s << endl;

    {
        LogDuration sleep_guard;
        // операция - ожидание 5 секунд
        this_thread::sleep_for(5s);
    }

    cout << "Ожидание завершено"s << endl;
}