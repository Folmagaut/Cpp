#include <iostream>
#include <memory>
#include <new>
#include <string>

using namespace std;

class Cat {
public:
    Cat(string name, int age)
        : name_(move(name))
        , age_(age)  //
    {
        cout << "Hello from " << name_ << endl;
    }

    ~Cat() {
        cout << "Goodbye from "sv << name_ << endl;
    }

    void SayHello() const {
        cout << "Meow, my name is "sv << name_ << ". I'm "sv << age_ << " year old."sv << endl;
    }

private:
    string name_;
    int age_;
};

int main() {
    Cat cat1("Tom"s, 2);
    cat1.SayHello();

    Cat* cat2 = new Cat("Leo"s, 3);
    cat2->SayHello();
    delete cat2;

    auto cat3 = make_unique<Cat>("Felix"s, 4);
    cat3->SayHello();

    // этот способ не работает, нужен явный вызов дуструктора
    //alignas(Cat) char buf[sizeof(Cat)];
    //unique_ptr<Cat> cat5(new (&buf[0]) Cat("Luna"s, 1));
    //cat5->SayHello();

    alignas(Cat) char buf[sizeof(Cat)];
    Cat* cat4 = new (&buf[0]) Cat("Luna"s, 1);
    cat4->SayHello();
    cat4->~Cat();

    void* buf2 = operator new (sizeof(Cat));
    Cat* cat6 = new (buf2) Cat("Murka"s, 4);
    cat6->SayHello();
    cat6->~Cat();
    operator delete (buf2);
}

