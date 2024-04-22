#ifndef OBJECT
#define OBJECT

#include <memory>
#include <iostream>
#include <vector>
#include <string>

#include "parser.hpp"

#define RETURN_OP_RESULT(OBJECT_TYPE, LEFT_DATA_TYPE, OPERATOR, RIGHT_DATA_TYPE, OUTPUT_DATA_TYPE) type = ObjectType::OBJECT_TYPE; \
    value = std::make_shared<OUTPUT_DATA_TYPE>(*static_cast<LEFT_DATA_TYPE*>(value.get()) OPERATOR *static_cast<RIGHT_DATA_TYPE*>(other.value.get())); \
    return *this;

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

        void print();

        std::string to_string();

        Object& operator+(Object& other);
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