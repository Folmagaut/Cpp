#pragma once

#include "identity_document.h"
#include <iostream>
#include <string>
#include <ctime>

using namespace std::string_view_literals;

class Passport {
public:
    Passport()
        : id_(), expiration_date_(GetExpirationDate()) 
    {
        SetVtable(this);
        std::cout << "Passport::Ctor()"sv << std::endl;
    }

    Passport(const Passport& other)
        : id_(other.id_)
        , expiration_date_(other.expiration_date_) 
    {
        SetVtable(this);
        std::cout << "Passport::CCtor()"sv << std::endl;
    }

    ~Passport() {
        IdentityDocument::SetVtable((IdentityDocument*)this);
        std::cout << "Passport::Dtor()"sv << std::endl;
    }

    void PrintVisa(const std::string& country) const {
        std::cout << "Passport::PrintVisa("sv << country << ") : "sv << GetID() << std::endl;
    }

    void PrintUniqueIDCount() {
        IdentityDocument::PrintUniqueIDCount();
    }

    struct Vtable {
        void (*VPrintId)(const Passport*);
        void (*VDelete)(Passport*);
    };

    static void SetVtable(Passport* doc) {
        *(Passport::Vtable**)doc = &vtable_;
    }

    Vtable* GetVtable() const {
        return reinterpret_cast<Vtable*>(id_.GetVtable());
    }

    void PrintID() const {
        GetVtable()->VPrintId(this);
    }

    void Delete() {
        GetVtable()->VDelete(this);
    }

    int GetID() const {
        return id_.GetID();
    }

    operator IdentityDocument() {
        return { id_ };
    }

private:
    IdentityDocument id_;
    static Vtable vtable_;

    const struct tm expiration_date_;

    tm GetExpirationDate() {
	    time_t t = time(nullptr);
	    tm exp_date = *localtime(&t);
	    exp_date.tm_year += 10;
	    mktime(&exp_date);
	    return exp_date;
	}

    static void PrintID(const Passport* doc) {
        std::cout << "Passport::PrintID() : "sv << doc->GetID();
        std::cout << " expiration date : "sv << doc->expiration_date_.tm_mday << "/"sv << doc->expiration_date_.tm_mon << "/"sv
                  << doc->expiration_date_.tm_year + 1900 << std::endl;
    }

    static void Delete(Passport* doc) {
        delete doc;
    }
};

Passport::Vtable Passport::vtable_ = { Passport::PrintID, Passport::Delete };

/* 
#pragma once

#include "identity_document.h"
#include <iostream>
#include <string>
#include <ctime>

using namespace std::string_view_literals;

class Passport {
public:
    Passport()
        : identity_doc_(), expiration_date_(GetExpirationDate())
    {
        Passport::SetVTable(this);
        std::cout << "Passport::Ctor()"sv << std::endl;
    }

    Passport(const Passport& other)
        : identity_doc_(other.identity_doc_)
        , expiration_date_(other.expiration_date_)
    {
        Passport::SetVTable(this);
        std::cout << "Passport::CCtor()"sv << std::endl;
    }

    ~Passport() {
        std::cout << "Passport::Dtor()"sv << std::endl;
        IdentityDocument::SetVTable((IdentityDocument*)this);
    }

    void Delete() {
        GetVtable()->delete_this(this);
    }

    void PrintID() const {
        GetVtable()->print_id(this);
    }

    void PrintUniqueIDCount() {
        IdentityDocument::PrintUniqueIDCount();
    }

    void PrintVisa(const std::string& country) const {
        GetVtable()->print_visa(this, country);
    }

    int GetID() const {
        return identity_doc_.GetID();
    }

    operator IdentityDocument() {
        return {identity_doc_};
    }

    using DeleteFunction = void(*)(Passport*);
    using PrintIDFunction = void(*)(const Passport*);
    using PrintVisaFunction = void(*)(const Passport*, const std::string& country);

    struct Vtable {
        DeleteFunction delete_this;
        PrintIDFunction print_id;
        PrintVisaFunction print_visa;
    };

    static void SetVTable(Passport* obj) {
        *(Passport::Vtable**)obj = &Passport::VTABLE;
    }

    const Vtable* GetVtable() const {
        return (const Passport::Vtable*)identity_doc_.GetVtable();
    }

    Vtable* GetVtable() {
        return (Passport::Vtable*)identity_doc_.GetVtable();
    }

    static Passport::Vtable VTABLE;

private:
    IdentityDocument identity_doc_;
    const tm expiration_date_;

    static void Delete(Passport* obj) {
        delete obj;
    }

    static void PrintID(const Passport* obj) {
        std::cout << "Passport::PrintID() : "sv << obj->GetID();
        std::cout << " expiration date : "sv << obj->expiration_date_.tm_mday << "/"sv << obj->expiration_date_.tm_mon << "/"sv
                  << obj->expiration_date_.tm_year + 1900 << std::endl;
    }

    static void PrintVisa(const Passport* obj, const std::string& country) {
        std::cout << "Passport::PrintVisa("sv << country << ") : "sv << obj->GetID() << std::endl;
    }

    tm GetExpirationDate() {
        time_t t = time(nullptr);
        tm exp_date = *localtime(&t);
        exp_date.tm_year += 10;
        mktime(&exp_date);
        return exp_date;
    }
};

Passport::Vtable Passport::VTABLE = { Passport::Delete,
                                      Passport::PrintID,
                                      Passport::PrintVisa };
*/