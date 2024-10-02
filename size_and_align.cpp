#include <cstddef>  // cstddef необходим для использования std::byte
#include <cassert>
#include <cstdint>  // cstdint необходим для использования uint32_t и int64_t
#include <iostream>
#include <map>
#include <string>

using namespace std;

struct Sportsman {
    int id;
    double height;
};

struct Vector3D {
    double x;
    double y;
    double z;
};

int main() {

    int p = 8;
    int* s = &p;
    int* r = nullptr;

    map<string, string>* string_to_string;

    int value = 1;

    // Указатель value_ptr1, ссылающийся на переменную value
    int* value_ptr1 = &value;
    // Указатель value_ptr2, копия указателя value_ptr1, также ссылается на переменную value
    int* value_ptr2 = value_ptr1;

    // Значения указателей на один и тот же объект будут равны
    assert(value_ptr1 == value_ptr2);

    // Значение объекта value, полученное напрямую и через указатель на него, будет одно и то же
    assert(*value_ptr1 == value && *value_ptr2 == value);

    // Изменение value видно через указатели на него
    value = 2;
    assert(*value_ptr1 == value && *value_ptr2 == value);

    // Изменяем значение value через указатель
    *value_ptr2 = 3;

    // Ожидаемо изменённое значение будет видно как при прямом доступе к объекту по его имени,
    // так и при косвенном обращении через указатель value_ptr1
    assert(*value_ptr1 == value && *value_ptr2 == value);

    cout << "char: size="s << sizeof(char) << ", alignment="s << alignof(char) << endl;
    cout << "int: size="s << sizeof(int) << ", alignment="s << alignof(int) << endl;
    cout << "double: size="s << sizeof(double) << ", alignment="s << alignof(double) << endl;
    cout << "Sportsman: size="s << sizeof(Sportsman) << ", alignment="s << alignof(Sportsman) << endl;

    cout << "char*: size:"s << sizeof(char*) << endl;
    cout << "int*: size:"s << sizeof(int*) << endl;
    cout << "double*: size:"s << sizeof(double*) << endl;
    cout << "Vector3D*: size:"s << sizeof(Vector3D*) << endl;
    cout << "map<string, string>*: size:"s << sizeof(string_to_string) << endl;
    cout << s << endl;

    return 0;
}