#include "object.hpp"

using namespace pop;

void Object::print()
{
    if (type == ObjectType::INT32)
    {
        std::cout << "Type: " << static_cast<int>(type) << "\n";
        std::cout << "Result: " << *static_cast<int*>(value.get()) << std::endl;
    }
    else if (type == ObjectType::FLOAT32)
    {
        std::cout << "Type: " << static_cast<int>(type) << "\n";
        std::cout << "Result: " << *static_cast<float*>(value.get()) << std::endl;
    }
    else if (type == ObjectType::CHAR)
    {
        std::cout << "Type: " << static_cast<int>(type) << "\n";
        std::cout << "Result: " << *static_cast<char*>(value.get()) << std::endl;
    }
    else if (type == ObjectType::BOOL)
    {
        std::cout << "Type: " << static_cast<int>(type) << "\n";
        std::cout << "Result: " << *static_cast<bool*>(value.get()) << std::endl;
    }
    else if (type == ObjectType::STRING)
    {
        std::cout << "Type: " << static_cast<int>(type) << "\n";
        std::cout << "Result: " << *static_cast<std::string*>(value.get()) << std::endl;
    }
}

std::string Object::to_string()
{
    if (type == ObjectType::INT32)
    {
        return std::to_string(*static_cast<int*>(value.get()));
    }
    else if (type == ObjectType::FLOAT32)
    {
        return std::to_string(*static_cast<float*>(value.get()));
    }
    else if (type == ObjectType::CHAR)
    {
        return std::string(1, *static_cast<char*>(value.get()));
    }
    else if (type == ObjectType::BOOL)
    {
        if (*static_cast<bool*>(value.get()))
            return "true";

        return "false";
    }
    else if (type == ObjectType::STRING)
    {
        return *static_cast<std::string*>(value.get());
    }
}

Object& Object::operator+(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, int, +, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, int, +, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, int, +, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, int, +, int, int);
        }
        else if (other.type == ObjectType::STRING)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(std::to_string(*static_cast<int*>(value.get())) + *static_cast<std::string*>(other.value.get()));
            return *this;
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(FLOAT32, float, +, int, float);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, float, +, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(FLOAT32, float, +, char, float);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(FLOAT32, float, +, bool, float);
        }
        else if (other.type == ObjectType::STRING)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(std::to_string(*static_cast<float*>(value.get())) + *static_cast<std::string*>(other.value.get()));
            return *this;
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, char, +, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, char, +, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, char, +, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, char, +, bool, int);
        }
        else if (other.type == ObjectType::STRING)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(*static_cast<char*>(value.get()) + *static_cast<std::string*>(other.value.get()));
            return *this;
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, bool, +, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, bool, +, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, bool, +, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, bool, +, bool, int);
        }
        else if (other.type == ObjectType::STRING)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(std::to_string(*static_cast<bool*>(value.get())) + *static_cast<std::string*>(other.value.get()));
            return *this;
        }
    }
    else if (type == ObjectType::STRING)
    {
        if (other.type == ObjectType::INT32)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(*static_cast<std::string*>(value.get()) + std::to_string(*static_cast<int*>(other.value.get())));
            return *this;
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(*static_cast<std::string*>(value.get()) + std::to_string(*static_cast<float*>(other.value.get())));
            return *this;
        }
        else if (other.type == ObjectType::CHAR)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(*static_cast<std::string*>(value.get()) + *static_cast<char*>(other.value.get()));
            return *this;
        }
        else if (other.type == ObjectType::BOOL)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(*static_cast<std::string*>(value.get()) + std::to_string(*static_cast<bool*>(other.value.get())));
            return *this;
        }
        else if (other.type == ObjectType::STRING)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(*static_cast<std::string*>(value.get()) + *static_cast<std::string*>(other.value.get()));
            return *this;
        }
    }

    throw std::runtime_error("Unable to add two objects!");
}

Object& Object::operator-(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, int, -, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, int, -, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, int, -, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, int, -, int, int);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(FLOAT32, float, -, int, float);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, float, -, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(FLOAT32, float, -, char, float);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(FLOAT32, float, -, bool, float);
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, char, -, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, char, -, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, char, -, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, char, -, bool, int);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, bool, -, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, bool, -, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, bool, -, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, bool, -, bool, int);
        }
    }

    throw std::runtime_error("Unable to subtract two objects!");
}

Object& Object::operator*(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, int, *, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, int, *, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, int, *, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, int, *, int, int);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(FLOAT32, float, *, int, float);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, float, *, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(FLOAT32, float, *, char, float);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(FLOAT32, float, *, bool, float);
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, char, *, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, char, *, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, char, *, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, char, *, bool, int);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, bool, *, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, bool, *, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, bool, *, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, bool, *, bool, int);
        }
    }

    throw std::runtime_error("Unable to multiply two objects!");
}

Object& Object::operator/(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, int, /, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, int, /, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, int, /, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, int, /, int, int);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(FLOAT32, float, /, int, float);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, float, /, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(FLOAT32, float, /, char, float);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(FLOAT32, float, /, bool, float);
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, char, /, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, char, /, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, char, /, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, char, /, bool, int);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, bool, /, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(FLOAT32, bool, /, float, float);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, bool, /, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, bool, /, bool, int);
        }
    }

    throw std::runtime_error("Unable to divide two objects!");
}

Object& Object::operator%(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, int, %, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            throw std::runtime_error("Unable to modulo two objects!");
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, int, %, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, int, %, bool, int);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        throw std::runtime_error("Unable to modulo two objects!");
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, char, %, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            throw std::runtime_error("Unable to modulo two objects!");
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, char, %, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, char, %, bool, int);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(INT32, bool, %, int, int);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            throw std::runtime_error("Unable to modulo two objects!");
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(INT32, bool, %, char, int);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(INT32, bool, %, bool, int);
        }
    }

    throw std::runtime_error("Unable to modulo two objects!");
}

Object& Object::operator==(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, int, ==, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, int, ==, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, int, ==, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, int, ==, int, bool);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, float, ==, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, float, ==, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, float, ==, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, float, ==, bool, bool);
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, char, ==, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, char, ==, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, char, ==, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, char, ==, bool, bool);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, bool, ==, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, bool, ==, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, bool, ==, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, bool, ==, bool, bool);
        }
    }

    throw std::runtime_error("Unable to equals two objects!");
}

Object& Object::operator!=(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, int, !=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, int, !=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, int, !=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, int, !=, int, bool);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, float, !=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, float, !=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, float, !=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, float, !=, bool, bool);
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, char, !=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, char, !=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, char, !=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, char, !=, bool, bool);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, bool, !=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, bool, !=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, bool, !=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, bool, !=, bool, bool);
        }
    }

    throw std::runtime_error("Unable to not equals two objects!");
}

Object& Object::operator>=(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, int, >=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, int, >=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, int, >=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, int, >=, int, bool);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, float, >=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, float, >=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, float, >=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, float, >=, bool, bool);
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, char, >=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, char, >=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, char, >=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, char, >=, bool, bool);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, bool, >=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, bool, >=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, bool, >=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, bool, >=, bool, bool);
        }
    }

    throw std::runtime_error("Unable to greater than or equals two objects!");
}

Object& Object::operator<=(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, int, <=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, int, <=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, int, <=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, int, <=, int, bool);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, float, <=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, float, <=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, float, <=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, float, <=, bool, bool);
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, char, <=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, char, <=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, char, <=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, char, <=, bool, bool);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, bool, <=, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, bool, <=, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, bool, <=, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, bool, <=, bool, bool);
        }
    }

    throw std::runtime_error("Unable to less than or equals two objects!");
}

Object& Object::operator>(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, int, >, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, int, >, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, int, >, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, int, >, int, bool);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, float, >, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, float, >, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, float, >, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, float, >, bool, bool);
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, char, >, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, char, >, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, char, >, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, char, >, bool, bool);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, bool, >, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, bool, >, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, bool, >, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, bool, >, bool, bool);
        }
    }

    throw std::runtime_error("Unable to greather than two objects!");
}

Object& Object::operator<(Object& other)
{
    if (type == ObjectType::INT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, int, <, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, int, <, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, int, <, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, int, <, int, bool);
        }
    }
    else if (type == ObjectType::FLOAT32)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, float, <, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, float, <, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, float, <, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, float, <, bool, bool);
        }
    }
    else if (type == ObjectType::CHAR)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, char, <, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, char, <, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, char, <, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, char, <, bool, bool);
        }
    }
    else if (type == ObjectType::BOOL)
    {
        if (other.type == ObjectType::INT32)
        {
            RETURN_OP_RESULT(BOOL, bool, <, int, bool);
        }
        else if (other.type == ObjectType::FLOAT32)
        {
            RETURN_OP_RESULT(BOOL, bool, <, float, bool);
        }
        else if (other.type == ObjectType::CHAR)
        {
            RETURN_OP_RESULT(BOOL, bool, <, char, bool);
        }
        else if (other.type == ObjectType::BOOL)
        {
            RETURN_OP_RESULT(BOOL, bool, <, bool, bool);
        }
    }

    throw std::runtime_error("Unable to less than two objects!");
}