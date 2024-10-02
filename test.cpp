/*



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