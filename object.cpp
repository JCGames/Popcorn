#include "object.hpp"

using namespace pop;

Object::Object()
{
    this->type = ObjectType::NIL;
}

Object::Object(ObjectType type, std::shared_ptr<void> value)
{
    this->type = type;
    this->value = value;
}

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

Object Object::to_int32()
{
    if (type == ObjectType::INT32)
    {
        return *this;
    }
    else if (type == ObjectType::FLOAT32)
    {
        return Object(ObjectType::INT32, std::make_shared<int>(CAST(CASTS(value, float), int)));
    }
    else if (type == ObjectType::CHAR)
    {
        return Object(ObjectType::INT32, std::make_shared<int>(CAST(CASTS(value, char), int)));
    }
    else if (type == ObjectType::BOOL)
    {
        return Object(ObjectType::INT32, std::make_shared<int>(CAST(CASTS(value, bool), int)));
    }
    else
    {
        throw std::runtime_error("Cannot cast to int!");
    }
}

Object Object::to_float32()
{
    if (type == ObjectType::INT32)
    {
        return Object(ObjectType::FLOAT32, std::make_shared<float>(CAST(CASTS(value, int), float)));
    }
    else if (type == ObjectType::FLOAT32)
    {
        return *this;
    }
    else if (type == ObjectType::CHAR)
    {
        return Object(ObjectType::FLOAT32, std::make_shared<float>(CAST(CASTS(value, char), float)));
    }
    else if (type == ObjectType::BOOL)
    {
        return Object(ObjectType::FLOAT32, std::make_shared<float>(CAST(CASTS(value, bool), float)));
    }
    else
    {
        throw std::runtime_error("Cannot cast to float!");
    }
}

Object Object::to_char()
{
    if (type == ObjectType::INT32)
    {
        return Object(ObjectType::CHAR, std::make_shared<char>(CAST(CASTS(value, int), char)));
    }
    else if (type == ObjectType::FLOAT32)
    {
        return Object(ObjectType::CHAR, std::make_shared<char>(CAST(CASTS(value, float), char)));
    }
    else if (type == ObjectType::CHAR)
    {
        return *this;
    }
    else if (type == ObjectType::BOOL)
    {
        return Object(ObjectType::CHAR, std::make_shared<char>(CAST(CASTS(value, bool), char)));
    }
    else
    {
        throw std::runtime_error("Cannot cast to char!");
    }
}

Object Object::to_bool()
{
    if (type == ObjectType::INT32)
    {
        return Object(ObjectType::BOOL, std::make_shared<bool>(CAST(CASTS(value, int), bool)));
    }
    else if (type == ObjectType::FLOAT32)
    {
        return Object(ObjectType::BOOL, std::make_shared<bool>(CAST(CASTS(value, float), bool)));
    }
    else if (type == ObjectType::CHAR)
    {
        return Object(ObjectType::BOOL, std::make_shared<bool>(CAST(CASTS(value, char), bool)));
    }
    else if (type == ObjectType::BOOL)
    {
        return *this;
    }
    else
    {
        throw std::runtime_error("Cannot cast to bool!");
    }
}

Object Object::to_string()
{
    if (type == ObjectType::INT32)
    {
        std::string str = std::to_string(CASTS(value, int));
        return Object(ObjectType::STRING, std::make_shared<std::string>(str));
    }
    else if (type == ObjectType::FLOAT32)
    {
        std::string str = std::to_string(CASTS(value, float));
        return Object(ObjectType::STRING, std::make_shared<std::string>(str));
    }
    else if (type == ObjectType::CHAR)
    {
        std::string str = std::string(1, CASTS(value, char));
        return Object(ObjectType::STRING, std::make_shared<std::string>(str));
    }
    else if (type == ObjectType::BOOL)
    {
        if (*static_cast<bool*>(value.get()))
            return Object(ObjectType::STRING, std::make_shared<std::string>("true"));

        return Object(ObjectType::STRING, std::make_shared<std::string>("false"));
    }
    else if (type == ObjectType::STRING)
    {
        return Object(ObjectType::STRING, std::make_shared<std::string>(CASTS(value, std::string)));
    }
    else if (type == ObjectType::NIL)
    {
        return Object(ObjectType::STRING, std::make_shared<std::string>("Nil"));
    }
    else
    {
        throw std::runtime_error("Cannot cast to string!");
    }
}

Object& Object::operator+(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, int) + CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::FLOAT32;
            value = std::make_shared<float>(CASTS(value, float) + CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, char) + CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, bool) + CASTS(other.value, bool));
            return *this;
        }
        else if (type == ObjectType::STRING)
        {
            type = ObjectType::STRING;
            value = std::make_shared<std::string>(CASTS(value, std::string) + CASTS(other.value, std::string));
            return *this;
        }
    }

    throw std::runtime_error("Unable to add two objects!");
}

Object& Object::operator-()
{
    if (type == ObjectType::INT32)
    {
        type = ObjectType::INT32;
        value = std::make_shared<int>(-1 * CASTS(value, int));
        return *this;
    }
    else if (type == ObjectType::FLOAT32)
    {
        type = ObjectType::FLOAT32;
        value = std::make_shared<float>(-1 * CASTS(value, float));
        return *this;
    }
    else if (type == ObjectType::CHAR)
    {
        type = ObjectType::INT32;
        value = std::make_shared<int>(-1 * CASTS(value, char));
        return *this;
    }
    else if (type == ObjectType::BOOL)
    {
        type = ObjectType::INT32;
        value = std::make_shared<int>(-1 * CASTS(value, bool));
        return *this;
    }

    throw std::runtime_error("Unable to negate object!");
}

Object& Object::operator-(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, int) - CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::FLOAT32;
            value = std::make_shared<float>(CASTS(value, float) - CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, char) - CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, bool) - CASTS(other.value, bool));
            return *this;
        }
    }

    throw std::runtime_error("Unable to subtract two objects!");
}

Object& Object::operator*(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, int) * CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::FLOAT32;
            value = std::make_shared<float>(CASTS(value, float) * CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, char) * CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, bool) * CASTS(other.value, bool));
            return *this;
        }
    }

    throw std::runtime_error("Unable to multiply two objects!");
}

Object& Object::operator/(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, int) / CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::FLOAT32;
            value = std::make_shared<float>(CASTS(value, float) / CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, char) / CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, bool) / CASTS(other.value, bool));
            return *this;
        }
    }

    throw std::runtime_error("Unable to divide two objects!");
}

Object& Object::operator%(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, int) % CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, char) % CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::INT32;
            value = std::make_shared<int>(CASTS(value, bool) % CASTS(other.value, bool));
            return *this;
        }
    }

    throw std::runtime_error("Unable to modulo two objects!");
}

Object& Object::operator==(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, int) == CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, float) == CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, char) == CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, bool) == CASTS(other.value, bool));
            return *this;
        }
        else if (type == ObjectType::STRING)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, std::string) == CASTS(other.value, std::string));
            return *this;
        }
    }

    throw std::runtime_error("Unable to equals two objects!");
}

Object& Object::operator!=(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, int) != CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, float) != CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, char) != CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, bool) != CASTS(other.value, bool));
            return *this;
        }
        else if (type == ObjectType::STRING)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, std::string) != CASTS(other.value, std::string));
            return *this;
        }
    }

    throw std::runtime_error("Unable to not equals two objects!");
}

Object& Object::operator>=(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, int) >= CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, float) >= CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, char) >= CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, bool) >= CASTS(other.value, bool));
            return *this;
        }
        else if (type == ObjectType::STRING)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, std::string) >= CASTS(other.value, std::string));
            return *this;
        }
    }

    throw std::runtime_error("Unable to greater than or equals two objects!");
}

Object& Object::operator<=(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, int) <= CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, float) <= CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, char) <= CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, bool) <= CASTS(other.value, bool));
            return *this;
        }
        else if (type == ObjectType::STRING)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, std::string) <= CASTS(other.value, std::string));
            return *this;
        }
    }

    throw std::runtime_error("Unable to less than or equals two objects!");
}

Object& Object::operator>(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, int) > CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, float) > CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, char) > CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, bool) > CASTS(other.value, bool));
            return *this;
        }
        else if (type == ObjectType::STRING)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, std::string) > CASTS(other.value, std::string));
            return *this;
        }
    }

    throw std::runtime_error("Unable to greather than two objects!");
}

Object& Object::operator<(Object& other)
{
    if (type == other.type)
    {
        if (type == ObjectType::INT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, int) < CASTS(other.value, int));
            return *this;
        }
        else if (type == ObjectType::FLOAT32)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, float) < CASTS(other.value, float));
            return *this;
        }
        else if (type == ObjectType::CHAR)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, char) < CASTS(other.value, char));
            return *this;
        }
        else if (type == ObjectType::BOOL)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, bool) < CASTS(other.value, bool));
            return *this;
        }
        else if (type == ObjectType::STRING)
        {
            type = ObjectType::BOOL;
            value = std::make_shared<bool>(CASTS(value, std::string) < CASTS(other.value, std::string));
            return *this;
        }
    }

    throw std::runtime_error("Unable to less than two objects!");
}