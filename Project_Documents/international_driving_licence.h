#pragma once

#include "driving_licence.h"
#include <iostream>
#include <string>

using namespace std::string_view_literals;

class InternationalDrivingLicence {
public:
    InternationalDrivingLicence() : id_() {
        SetVtable(this);
        std::cout << "InternationalDrivingLicence::Ctor()"sv << std::endl;
    }

    InternationalDrivingLicence(const InternationalDrivingLicence& other)
        : id_(other.id_) 
    {
        SetVtable(this);
        std::cout << "InternationalDrivingLicence::CCtor()"sv << std::endl;
    }

    ~InternationalDrivingLicence() {
        DrivingLicence::SetVtable((DrivingLicence*)this);
        std::cout << "InternationalDrivingLicence::Dtor()"sv << std::endl;
    }

    void PrintID() const {
        GetVtable()->VPrintId(this);
        //std::cout << "InternationalDrivingLicence::PrintID() : "sv << GetID() << std::endl;
    }

    struct Vtable {
        void (*VPrintId)(const InternationalDrivingLicence*);
        void (*VDelete)(InternationalDrivingLicence*);
        int (*VGetId)(const InternationalDrivingLicence*);
    };

    void SetVtable(InternationalDrivingLicence* doc) {
        *(InternationalDrivingLicence::Vtable**)doc = &vtable_;
    }

    Vtable* GetVtable() const {
        return reinterpret_cast<Vtable*>(id_.GetVtable());
    }

    void Delete() {
        GetVtable()->VDelete(this);
    }

    int GetID() const {
        return GetVtable()->VGetId(this);
    }

    operator DrivingLicence() {
        return id_;
    }

private:
    DrivingLicence id_;
    static Vtable vtable_;

    static void PrintID(const InternationalDrivingLicence* doc) {
        std::cout << "InternationalDrivingLicence::PrintID() : "sv << doc->id_.GetID() << std::endl;
    }

    static void Delete(InternationalDrivingLicence* doc) {
        delete doc;
    }

    static int GetID(const InternationalDrivingLicence* doc) {
        return doc->id_.GetID();
    }    
};

InternationalDrivingLicence::Vtable InternationalDrivingLicence::vtable_ = { InternationalDrivingLicence::PrintID, InternationalDrivingLicence::Delete, InternationalDrivingLicence::GetID };

/* 
#pragma once

#include "driving_licence.h"
#include <iostream>
#include <string>

using namespace std::string_view_literals;

class InternationalDrivingLicence {
public:
    InternationalDrivingLicence() 
        : driving_lic_() 
    {
        InternationalDrivingLicence::SetVTable(this);
        std::cout << "InternationalDrivingLicence::Ctor()"sv << std::endl;
    }

    InternationalDrivingLicence(const InternationalDrivingLicence& other)
        : driving_lic_(other.driving_lic_)
    {
        InternationalDrivingLicence::SetVTable(this);
        std::cout << "InternationalDrivingLicence::CCtor()"sv << std::endl;
    }

    ~InternationalDrivingLicence() {
        std::cout << "InternationalDrivingLicence::Dtor()"sv << std::endl;
        DrivingLicence::SetVTable((DrivingLicence*)this);
    }

    void Delete() {
        GetVtable()->delete_this(this);
    }

    void PrintID() const {
        GetVtable()->print_id(this);
    }

    int GetID() const {
        return driving_lic_.GetID();
    }

    operator DrivingLicence() const {
        return {driving_lic_};
    }

    using DeleteFunction = void(*)(InternationalDrivingLicence*);
    using PrintIDFunction = void(*)(const InternationalDrivingLicence*);

    struct Vtable {
        DeleteFunction delete_this;
        PrintIDFunction print_id;
    };

    static void SetVTable(InternationalDrivingLicence* obj) {
        *(InternationalDrivingLicence::Vtable**)obj = &InternationalDrivingLicence::VTABLE;
    }

    const Vtable* GetVtable() const {
        return (const InternationalDrivingLicence::Vtable*)driving_lic_.GetVtable();
    }

    Vtable* GetVtable() {
        return (InternationalDrivingLicence::Vtable*)driving_lic_.GetVtable();
    }

    static InternationalDrivingLicence::Vtable VTABLE;

private:
    DrivingLicence driving_lic_;

    static void Delete(InternationalDrivingLicence* obj) {
        delete obj;
    }

    static void PrintID(const InternationalDrivingLicence* obj) {
        std::cout << "InternationalDrivingLicence::PrintID() : "sv << obj->GetID() << std::endl;
    }
};

InternationalDrivingLicence::Vtable InternationalDrivingLicence::VTABLE = { InternationalDrivingLicence::Delete,
                                                                            InternationalDrivingLicence::PrintID };
*/