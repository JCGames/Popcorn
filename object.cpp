#include "object.hpp"

using namespace run;

Object::Object()
{
    type = ObjectType::_NULL;
    value = NULL;
}

Object::Object(std::string strValue)
{
    this->type = ObjectType::STRING;
    this->value = new std::string(strValue);
}

Object::Object(bool boolValue)
{
    this->type = ObjectType::BOOLEAN;
    this->value = new bool(boolValue);
}

Object::Object(int intValue)
{
    this->type = ObjectType::INTEGER;
    this->value = new int(intValue);
}

Object::Object(double doubleValue)
{
    this->type = ObjectType::DOUBLE;
    this->value = new double(doubleValue);
}

Object::Object(const Object& other)
{
    this->type = other.type;

    switch (type)
    {
        case ObjectType::BOOLEAN:
            value = new bool(*static_cast<bool*>(other.value));
            break;
        case ObjectType::INTEGER:
            value = new int(*static_cast<int*>(other.value));
            break;
        case ObjectType::DOUBLE:
            value = new double(*static_cast<double*>(other.value));
            break;
        case ObjectType::STRING:
            value = new std::string(*static_cast<std::string*>(other.value));
            break;
    }
}

Object& Object::operator=(const Object& other) 
{
    deleteValue(*this);

    if (this != &other)
    {
        if (other.value != nullptr)
        {
            this->type = other.type;

            switch (type)
            {
                case ObjectType::BOOLEAN:
                    value = new bool(*static_cast<bool*>(other.value));
                    break;
                case ObjectType::INTEGER:
                    value = new int(*static_cast<int*>(other.value));
                    break;
                case ObjectType::DOUBLE:
                    value = new double(*static_cast<double*>(other.value));
                    break;
                case ObjectType::STRING:
                    value = new std::string(*static_cast<std::string*>(other.value));
                    break;
            }
        }
    }

    return *this;
}

Object::~Object()
{
    deleteValue(*this);
}

void Object::deleteValue(Object& obj)
{
    if (obj.value != nullptr)
    {
        switch (obj.type)
        {
            case ObjectType::BOOLEAN:
                delete static_cast<bool*>(obj.value);
                break;
            case ObjectType::INTEGER:
                delete static_cast<int*>(obj.value);
                break;
            case ObjectType::DOUBLE:
                delete static_cast<double*>(obj.value);
                break;
            case ObjectType::STRING:
                delete static_cast<std::string*>(obj.value);
                break;
        }
    }
}

/**
 * Getters
*/

bool Object::get_bool()
{
    return *(bool*)value;
}

int Object::get_int()
{
    return *(int*)value;
}

double Object::get_double()
{
    return *(double*)value;
}

std::string Object::get_str()
{
    return *(std::string*)value;
}

/**
 * Casting functions
*/

Object Object::cast_to_bool()
{
    switch (type)
    {
        case ObjectType::STRING:

            if (get_str() == "true" || get_str() == "True")
            {
                return Object(true);
            }
            else if (get_str() == "false" || get_str() == "False")
            {
                return Object(false);
            }

            int value;

            try {
                value = std::stoi(get_str());
            } catch (const std::exception& e) {
                throw std::runtime_error("Could not cast string to boolean!");
            }

            return Object(value >= 1);
        case ObjectType::BOOLEAN:
            return Object(get_bool());
        case ObjectType::INTEGER:
            return Object((bool)get_int());
        case ObjectType::DOUBLE:
            return Object((bool)get_double());
    }

    return Object();
}

Object Object::cast_to_int()
{
    switch (type)
    {
        case ObjectType::BOOLEAN:
            return Object((int)get_bool());
        case ObjectType::INTEGER:
            return Object(get_int());
        case ObjectType::DOUBLE:
            return Object((int)get_double());
        case ObjectType::STRING:
            {
                int value;

                try {
                    value = std::stoi(get_str());
                } catch (const std::exception& e) {
                    throw std::runtime_error(e.what());
                }

                return Object(value);
            }   
            break;
    }

    return Object();
}

Object Object::cast_to_double()
{
    switch (type)
    {
        case ObjectType::BOOLEAN:
            return Object((double)get_bool());
        case ObjectType::INTEGER:
            return Object((double)get_int());
        case ObjectType::DOUBLE:
            return Object(get_double());
        case ObjectType::STRING:
            {
                double value;

                try {
                    value = std::stof(get_str());
                } catch (const std::exception& e) {
                    throw std::runtime_error(e.what());
                }

                return Object(value);
            }   
            break;
    }

    return Object();
}

Object Object::cast_to_string()
{
    switch (type)
    {
        case ObjectType::STRING:
            return Object(get_str());
        case ObjectType::BOOLEAN:
            return Object((std::string)(get_bool() ? "True" : "False"));
        case ObjectType::INTEGER:
            return Object(std::to_string(get_int()));
        case ObjectType::DOUBLE:
            return Object(std::to_string(get_double()));
    }

    return Object();
}

ObjectType Object::get_type()
{
    return type;
}

/**
 * Operations
*/

Object Object::add_to(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() + obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() + obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() + obj.get_double());
                case ObjectType::STRING:
                    return Object(cast_to_string().get_str() + obj.get_str());
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() + obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() + obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() + obj.get_double());
                case ObjectType::STRING:
                    return Object(cast_to_string().get_str() + obj.get_str());
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() + obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() + obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() + obj.get_double());
                case ObjectType::STRING:
                    return Object(cast_to_string().get_str() + obj.get_str());
            }

            break;
        case ObjectType::STRING:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_str() + obj.cast_to_string().get_str());
                case ObjectType::INTEGER:
                    return Object(get_str() + obj.cast_to_string().get_str());
                case ObjectType::DOUBLE:
                    return Object(get_str() + obj.cast_to_string().get_str());
                case ObjectType::STRING:
                    return Object(get_str() + obj.get_str());
            }

            break;
    }

    return Object();
}

Object Object::modulus_by(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() % obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() % obj.get_int());
                case ObjectType::DOUBLE:
                    throw std::runtime_error("Cannot modulus by a double!");
                case ObjectType::STRING:
                    throw std::runtime_error("Cannot modulus by a string!");
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() % obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() % obj.get_int());
                case ObjectType::DOUBLE:
                    throw std::runtime_error("Cannot modulus by a double!");
                case ObjectType::STRING:
                    throw std::runtime_error("Cannot modulus by a string!");
            }

            break;
        case ObjectType::DOUBLE:

            throw std::runtime_error("Double cannot be moduloed!");

            break;
        case ObjectType::STRING:

            throw std::runtime_error("String cannot be moduloed!");

            break;
    }

    return Object();
}

Object Object::subtract_from(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() - obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() - obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() - obj.get_double());
                case ObjectType::STRING:
                    throw std::runtime_error("Boolean cannot be subtracted by a string!");
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() - obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() - obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() - obj.get_double());
                case ObjectType::STRING:
                    throw std::runtime_error("Boolean cannot be subtracted by a string!");
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() - obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() - obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() - obj.get_double());
                case ObjectType::STRING:
                    throw std::runtime_error("Double cannot be subtracted by a string!");
            }

            break;
        case ObjectType::STRING:

            throw std::runtime_error("Cannot subtract from a string");

            break;
    }

    return Object();
}

Object Object::multiplied_by(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() * obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() * obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() * obj.get_double());
                case ObjectType::STRING:
                    throw std::runtime_error("Boolean cannot be multiplied by a string!");
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() * obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() * obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() * obj.get_double());
                case ObjectType::STRING:
                    throw std::runtime_error("Boolean cannot be multiplied by a string!");
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() * obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() * obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() * obj.get_double());
                case ObjectType::STRING:
                    throw std::runtime_error("Double cannot be multiplied by a string!");
            }

            break;
        case ObjectType::STRING:

            throw std::runtime_error("Cannot multiply by a string");

            break;
    }

    return Object();
}

Object Object::divided_by(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() / obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() / obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() / obj.get_double());
                case ObjectType::STRING:
                    throw std::runtime_error("Boolean cannot be divided by a string!");
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() / obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() / obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() / obj.get_double());
                case ObjectType::STRING:
                    throw std::runtime_error("Boolean cannot be divided by a string!");
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() / obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() / obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() / obj.get_double());
                case ObjectType::STRING:
                    throw std::runtime_error("Double cannot be divided by a string!");
            }

            break;
        case ObjectType::STRING:

            throw std::runtime_error("Cannot divided by a string");

            break;
    }

    return Object();
}

Object Object::negate()
{
    switch (type)
    {
        case ObjectType::BOOLEAN:
            return Object(!get_bool());
        case ObjectType::INTEGER:
            return Object(-1 * get_int());
        case ObjectType::DOUBLE:
            return Object(-1 * get_double());
        case ObjectType::STRING:
            throw std::runtime_error("Cannot negate a string!");
    }

    return Object();
}

Object Object::equals(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() == obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() == obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() == obj.get_double());
                case ObjectType::STRING:
                    return Object(get_bool() == obj.cast_to_bool().get_bool());
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() == obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() == obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() == obj.get_double());
                case ObjectType::STRING:
                    return Object(get_int() == obj.cast_to_int().get_int());
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() == obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() == obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() == obj.get_double());
                case ObjectType::STRING:
                    return Object(get_double() == obj.cast_to_double().get_double());
            }

            break;
        case ObjectType::STRING:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    try {
                        return Object(cast_to_bool().get_bool() == obj.get_bool());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a boolean value!");
                    }
                case ObjectType::INTEGER:
                    try {
                        return Object(cast_to_int().get_int() == obj.get_int());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to an integer value!");
                    }
                case ObjectType::DOUBLE:
                    try {
                        return Object(cast_to_double().get_double() == obj.get_double());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a double value!");
                    }
                case ObjectType::STRING:
                    return Object(get_str() == obj.get_str());
            }

            break;
    }

    return Object();
}

Object Object::not_equals(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() != obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() != obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() != obj.get_double());
                case ObjectType::STRING:
                    return Object(get_bool() != obj.cast_to_bool().get_bool());
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() != obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() != obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() != obj.get_double());
                case ObjectType::STRING:
                    return Object(get_int() != obj.cast_to_int().get_int());
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() != obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() != obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() != obj.get_double());
                case ObjectType::STRING:
                    return Object(get_double() != obj.cast_to_double().get_double());
            }

            break;
        case ObjectType::STRING:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    try {
                        return Object(cast_to_bool().get_bool() != obj.get_bool());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a boolean value!");
                    }
                case ObjectType::INTEGER:
                    try {
                        return Object(cast_to_int().get_int() != obj.get_int());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to an integer value!");
                    }
                case ObjectType::DOUBLE:
                    try {
                        return Object(cast_to_double().get_double() != obj.get_double());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a double value!");
                    }
                case ObjectType::STRING:
                    return Object(get_str() != obj.get_str());
            }

            break;
    }

    return Object();
}

Object Object::greater_than(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() > obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() > obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() > obj.get_double());
                case ObjectType::STRING:
                    return Object(get_bool() > obj.cast_to_bool().get_bool());
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() > obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() > obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() > obj.get_double());
                case ObjectType::STRING:
                    return Object(get_int() > obj.cast_to_int().get_int());
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() > obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() > obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() > obj.get_double());
                case ObjectType::STRING:
                    return Object(get_double() > obj.cast_to_double().get_double());
            }

            break;
        case ObjectType::STRING:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    try {
                        return Object(cast_to_bool().get_bool() > obj.get_bool());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a boolean value!");
                    }
                case ObjectType::INTEGER:
                    try {
                        return Object(cast_to_int().get_int() > obj.get_int());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to an integer value!");
                    }
                case ObjectType::DOUBLE:
                    try {
                        return Object(cast_to_double().get_double() > obj.get_double());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a double value!");
                    }
                case ObjectType::STRING:
                    return Object(get_str() > obj.get_str());
            }

            break;
    }

    return Object();
}

Object Object::less_than(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() < obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() < obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() < obj.get_double());
                case ObjectType::STRING:
                    return Object(get_bool() < obj.cast_to_bool().get_bool());
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() < obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() < obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() < obj.get_double());
                case ObjectType::STRING:
                    return Object(get_int() < obj.cast_to_int().get_int());
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() < obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() < obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() < obj.get_double());
                case ObjectType::STRING:
                    return Object(get_double() < obj.cast_to_double().get_double());
            }

            break;
        case ObjectType::STRING:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    try {
                        return Object(cast_to_bool().get_bool() < obj.get_bool());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a boolean value!");
                    }
                case ObjectType::INTEGER:
                    try {
                        return Object(cast_to_int().get_int() < obj.get_int());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to an integer value!");
                    }
                case ObjectType::DOUBLE:
                    try {
                        return Object(cast_to_double().get_double() < obj.get_double());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a double value!");
                    }
                case ObjectType::STRING:
                    return Object(get_str() < obj.get_str());
            }

            break;
    }

    return Object();
}

Object Object::greater_than_equals(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() >= obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() >= obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() >= obj.get_double());
                case ObjectType::STRING:
                    return Object(get_bool() >= obj.cast_to_bool().get_bool());
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() >= obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() >= obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() >= obj.get_double());
                case ObjectType::STRING:
                    return Object(get_int() >= obj.cast_to_int().get_int());
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() >= obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() >= obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() >= obj.get_double());
                case ObjectType::STRING:
                    return Object(get_double() >= obj.cast_to_double().get_double());
            }

            break;
        case ObjectType::STRING:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    try {
                        return Object(cast_to_bool().get_bool() >= obj.get_bool());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a boolean value!");
                    }
                case ObjectType::INTEGER:
                    try {
                        return Object(cast_to_int().get_int() >= obj.get_int());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to an integer value!");
                    }
                case ObjectType::DOUBLE:
                    try {
                        return Object(cast_to_double().get_double() >= obj.get_double());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a double value!");
                    }
                case ObjectType::STRING:
                    return Object(get_str() >= obj.get_str());
            }

            break;
    }

    return Object();
}

Object Object::less_than_equals(Object obj)
{
    switch (type)
    {
        case ObjectType::BOOLEAN:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_bool() <= obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_bool() <= obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_bool() <= obj.get_double());
                case ObjectType::STRING:
                    return Object(get_bool() <= obj.cast_to_bool().get_bool());
            }

            break;
        case ObjectType::INTEGER:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_int() <= obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_int() <= obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_int() <= obj.get_double());
                case ObjectType::STRING:
                    return Object(get_int() <= obj.cast_to_int().get_int());
            }

            break;
        case ObjectType::DOUBLE:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    return Object(get_double() <= obj.get_bool());
                case ObjectType::INTEGER:
                    return Object(get_double() <= obj.get_int());
                case ObjectType::DOUBLE:
                    return Object(get_double() <= obj.get_double());
                case ObjectType::STRING:
                    return Object(get_double() <= obj.cast_to_double().get_double());
            }

            break;
        case ObjectType::STRING:

            switch (obj.type)
            {
                case ObjectType::BOOLEAN:
                    try {
                        return Object(cast_to_bool().get_bool() <= obj.get_bool());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a boolean value!");
                    }
                case ObjectType::INTEGER:
                    try {
                        return Object(cast_to_int().get_int() <= obj.get_int());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to an integer value!");
                    }
                case ObjectType::DOUBLE:
                    try {
                        return Object(cast_to_double().get_double() <= obj.get_double());
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Cannot cast the string \"" + get_str() + "\" to a double value!");
                    }
                case ObjectType::STRING:
                    return Object(get_str() <= obj.get_str());
            }

            break;
    }

    return Object();
}