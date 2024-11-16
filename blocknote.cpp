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
    /* template <typename ModifierFn>
    void Write(ModifierFn&& modify) {
        EnsureUnique();
        // Теперь value_ — единственный владелец данных.

        std::forward<ModifierFn>(modify)(*value_);
    } */
    /* Value& Write() {
        EnsureUnique();

        return *value_;
    } */ //- так не надо

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

    /* s2.Write([](auto& value) {
        // Внутри этой функции можно изменить значение, содержащееся в s2.
        value = "World"s;
        value += '!';
    }); */

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