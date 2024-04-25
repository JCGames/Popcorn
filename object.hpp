#ifndef OBJECT
#define OBJECT

#include <memory>
#include <iostream>
#include <vector>
#include <string>

#include "parser.hpp"

// Casts a non-pointer
#define CAST(value, type) static_cast<type>(value)

// Casts a pointer
#define CASTP(value, type) *static_cast<type*>(value)

// Casts a shared pointer
#define CASTS(value, type) *static_cast<type*>(value.get())

namespace pop
{
    /**
     * The type of object to store.
    */
    enum class ObjectType : char
    {
        INT32,
        FLOAT32,
        CHAR,
        NIL,
        BOOL,
        STRING
    };

    /**
     * An object that can represents many data types.
    */
    struct Object
    {
        ObjectType type;
        std::shared_ptr<void> value;

        Object();
        Object(ObjectType type, std::shared_ptr<void> value);

        void print();

        Object to_int32();
        Object to_float32();
        Object to_char();
        Object to_bool();
        Object to_string();

        Object& operator+(Object& other);
        Object& operator-();
        Object& operator-(Object& other);
        Object& operator*(Object& other);
        Object& operator/(Object& other);
        Object& operator%(Object& other);
        Object& operator==(Object& other);
        Object& operator!=(Object& other);
        Object& operator>=(Object& other);
        Object& operator<=(Object& other);
        Object& operator>(Object& other);
        Object& operator<(Object& other);
    };
}

#endif