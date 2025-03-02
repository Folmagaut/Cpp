#pragma once

#include <iostream>
#include <string>

using namespace std::string_view_literals;

class IdentityDocument {
public:
    IdentityDocument()
        : unique_id_(++unique_id_count_) 
    {
        SetVtable(this);
        std::cout << "IdentityDocument::Ctor() : "sv << unique_id_ << std::endl;
    }

    ~IdentityDocument() {
        --unique_id_count_;
        std::cout << "IdentityDocument::Dtor() : "sv << unique_id_ << std::endl;
    }

    IdentityDocument(const IdentityDocument& other)
        : unique_id_(++unique_id_count_) 
    {
        SetVtable(this);
        std::cout << "IdentityDocument::CCtor() : "sv << unique_id_ << std::endl;
    }

    IdentityDocument& operator=(const IdentityDocument&) = delete;

    static void PrintUniqueIDCount() {
        std::cout << "unique_id_count_ : "sv << unique_id_count_ << std::endl;
    }

//protected:
    int GetID() const {
        return unique_id_;
    }

    struct Vtable {
        void (*VPrintId)(const IdentityDocument*);
        void (*VDelete)(IdentityDocument*);
    };

    static void SetVtable(IdentityDocument* doc) {
        *(Vtable**) doc = &vtable_;
    }

    void PrintID() const {
        GetVtable()->VPrintId(this);
        //std::cout << "IdentityDocument::PrintID() : "sv << unique_id_ << std::endl;
    }

    Vtable* GetVtable() const {
        return vptr_;
    }
    
    void Delete() {
        GetVtable()->VDelete(this);
    }

private:
    Vtable* vptr_;
    static Vtable vtable_;
    static int unique_id_count_;
    int unique_id_;

    static void PrintID(const IdentityDocument* doc) {
        std::cout << "IdentityDocument::PrintID() : "sv << doc->unique_id_ << std::endl;
    }

    static void Delete(IdentityDocument* doc) {
        delete(doc);
    }
};

int IdentityDocument::unique_id_count_ = 0;
IdentityDocument::Vtable IdentityDocument::vtable_ = { IdentityDocument::PrintID, IdentityDocument::Delete };

/* 
#pragma once

#include <iostream>
#include <string>

using namespace std::string_view_literals;

class IdentityDocument {
public:
    IdentityDocument()
        : unique_id_(++unique_id_count_)
    {
        // Установка vptr в реальной ситуации происходит до того, как объект сконструирован.
        //   Чтобы подчеркнуть это, используем статический метод
           IdentityDocument::SetVTable(this);
           std::cout << "IdentityDocument::Ctor() : "sv << unique_id_ << std::endl;
       }
   
       IdentityDocument(const IdentityDocument& other)
           : unique_id_(++unique_id_count_)
       {
           IdentityDocument::SetVTable(this);
           std::cout << "IdentityDocument::CCtor() : "sv << unique_id_ << std::endl;
       }
   
       ~IdentityDocument() {
           // В момент вызова этого деструктора компилятор точно знает тип объекта.
            //  Здесь вызов виртуального метода не требуется
           --unique_id_count_;
           std::cout << "IdentityDocument::Dtor() : "sv << unique_id_ << std::endl;
       }
   
       void Delete() {
           // Если объект удаляется динамически, то используется позднее связывание,
           //   и происходит вызов виртуального метода
           GetVtable()->delete_this(this);
       }
   
       IdentityDocument& operator=(const IdentityDocument&) = delete;
   
       void PrintID() const {
           // Это обычный виртуальный метод. Воспользуемся vtpr
           GetVtable()->print_id(this);
       }
   
       static void PrintUniqueIDCount() {
           // Метод не виртуальный
           std::cout << "unique_id_count_ : "sv << unique_id_count_ << std::endl;
       }
   
       int GetID() const {
           return unique_id_;
       }
   
       using DeleteFunction = void(*)(IdentityDocument*);
       using PrintIDFunction = void(*)(const IdentityDocument*);
   
       struct Vtable {
           DeleteFunction delete_this;
           PrintIDFunction print_id;
       };
   
       static void SetVTable(IdentityDocument* obj) {
           // Указатель vptr хранится в самом начале объекта,
            //  поэтому чтобы получить к нему доступ,
            //  можно воспользоваться C-style преобразованием
           // obj - указатель. При этом в начале объекта тоже лежит указатель - vptr.
            //  То есть можно сказать, что obj - это указатель на указатель. Иначе говоря,
            //  двойной указатель. Укажем это явно, а потом разыменуем, чтобы задать адрес,
            //  на который указывает vptr
           *(IdentityDocument::Vtable**)obj = &IdentityDocument::VTABLE;
       }
   
       const Vtable* GetVtable() const {
           return vptr_;
       }
   
       Vtable* GetVtable() {
           return vptr_;
       }
   
       static IdentityDocument::Vtable VTABLE;
   
   private:
       IdentityDocument::Vtable* vptr_;
       static int unique_id_count_;
       int unique_id_;
   
       static void Delete(IdentityDocument* obj) {
           // В этот момент тип объекта известен. Просто удаляем указатель.
           //Вызов delete запустит процесс вызовов деструкторов.
           delete obj;
       }
   
       static void PrintID(const IdentityDocument* obj) {
           std::cout << "IdentityDocument::PrintID() : "sv << obj->unique_id_ << std::endl;
       }
   };
   
   int IdentityDocument::unique_id_count_ = 0;
   IdentityDocument::Vtable IdentityDocument::VTABLE = { IdentityDocument::Delete,
                                                         IdentityDocument::PrintID };
*/