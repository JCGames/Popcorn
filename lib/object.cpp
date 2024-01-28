#include "object.hpp"
#include "diagnostics.hpp"

using namespace popcorn::runner;
using namespace popcorn::diagnostics;

/**
 * Creates a null object.
*/
Object::Object()
{
    this->type = DataType::_NULL;
    this->value = nullptr;
}

Object::Object(DataType type, void* value)
{
    this->type = type;
    this->value = value;
}

Object::Object(const Object& other)
{
    this->type = other.type;

    if (other.value != nullptr) 
    {
        switch (type)
        {
            case DataType::BOOLEAN:
                value = new bool(*static_cast<bool*>(other.value));
                break;
            case DataType::INTEGER:
                value = new int(*static_cast<int*>(other.value));
                break;
            case DataType::DOUBLE:
                value = new double(*static_cast<double*>(other.value));
                break;
            case DataType::STRING:
                value = new std::string(*static_cast<std::string*>(other.value));
                break;
            case DataType::ARRAY:
                value = new std::vector<Object>();
                break;
            case DataType::_NULL:
                value = nullptr;
                break;
        }
    }
}

Object& Object::operator=(const Object& other)
{
    if (this != &other)
    {
        delete_value(*this);

        if (other.value != nullptr)
        {
            this->type = other.type;

            switch (type)
            {
                case DataType::BOOLEAN:
                    value = new bool(*static_cast<bool*>(other.value));
                    break;
                case DataType::INTEGER:
                    value = new int(*static_cast<int*>(other.value));
                    break;
                case DataType::DOUBLE:
                    value = new double(*static_cast<double*>(other.value));
                    break;
                case DataType::STRING:
                    value = new std::string(*static_cast<std::string*>(other.value));
                    break;
                case DataType::ARRAY:
                    value = new std::vector<Object>(*static_cast<std::vector<Object>*>(other.value));
                    break;
            }
        }
    }

    return *this;
}

Object::~Object()
{
    delete_value(*this);
}

void Object::delete_value(Object& obj)
{
    if (obj.value != nullptr)
    {
        switch (obj.type)
        {
            case DataType::BOOLEAN:
                delete static_cast<bool*>(obj.value);
                break;
            case DataType::INTEGER:
                delete static_cast<int*>(obj.value);
                break;
            case DataType::DOUBLE:
                delete static_cast<double*>(obj.value);
                break;
            case DataType::STRING:
                delete static_cast<std::string*>(obj.value);
                break;
            case DataType::ARRAY:
                delete static_cast<std::vector<Object>*>(obj.value);
                break;
        }

        obj.value = nullptr;
    }
}

/**
 * Getters
*/

void* Object::get_value() const
{
    return value;
}

bool Object::get_bool()
{
    if (type == DataType::BOOLEAN)
        return *(bool*)value;
    else
        throw std::runtime_error("Object is not a boolean!");
}

int Object::get_int()
{
    if (type == DataType::INTEGER)
        return *(int*)value;
    else
        throw std::runtime_error("Object is not a integer!");
}

double Object::get_double()
{
    if (type == DataType::DOUBLE)
        return *(double*)value;
    else
        throw std::runtime_error("Object is not a double!");
}

std::string Object::get_str()
{
    if (type == DataType::STRING)
        return *(std::string*)value;
    else
        throw std::runtime_error("Object is not a string!");
}

std::vector<Object> Object::get_array()
{
    if (type == DataType::STRING)
        return *(std::vector<Object>*)value;
    else
        throw std::runtime_error("Object is not a string!");
}

/**
 * Casting functions
*/

Object Object::cast_to_bool()
{
    switch (type)
    {
        case DataType::STRING:

            if (get_str() == "true" || get_str() == "True")
            {
                return Object(DataType::BOOLEAN, new bool(true));
            }
            else if (get_str() == "false" || get_str() == "False")
            {
                return Object(DataType::BOOLEAN, new bool(false));
            }

            int value;

            try {
                value = std::stoi(get_str());
            } catch (const std::exception& e) {
                throw std::runtime_error("Could not cast string to boolean!");
            }

            return Object(DataType::BOOLEAN, new bool(value >= 1));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool()));
        case DataType::INTEGER:
            return Object(DataType::BOOLEAN, new bool(get_int()));
        case DataType::DOUBLE:
            return Object(DataType::BOOLEAN, new bool(get_double()));
    }

    Diagnostics::log_error("Could not cast to bool!");
    throw std::runtime_error("Could not cast to bool!");
}

Object Object::cast_to_int()
{
    switch (type)
    {
        case DataType::BOOLEAN:
            return Object(DataType::INTEGER, new int(get_bool()));
        case DataType::INTEGER:
            return Object(DataType::INTEGER, new int(get_int()));
        case DataType::DOUBLE:
            return Object(DataType::INTEGER, new int(get_double()));
        case DataType::STRING:
            {
                int value;

                try {
                    value = std::stoi(get_str());
                } catch (const std::exception& e) {
                    Diagnostics::log_error("Could not cast string to integer.");
                }

                return Object(DataType::INTEGER, new int(value));
            }   
            break;
    }

    Diagnostics::log_error("Could not cast to int!");
    throw std::runtime_error("Could not cast to int!");
}

Object Object::cast_to_double()
{
    switch (type)
    {
        case DataType::BOOLEAN:
            return Object(DataType::DOUBLE, new double(get_bool()));
        case DataType::INTEGER:
            return Object(DataType::DOUBLE, new double(get_int()));
        case DataType::DOUBLE:
            return Object(DataType::DOUBLE, new double(get_double()));
        case DataType::STRING:
            {
                double value;

                try {
                    value = std::stof(get_str());
                } catch (const std::exception& e) {
                    Diagnostics::log_error("Could not cast string to double.");
                }

                return Object(DataType::DOUBLE, new double(value));
            }   
            break;
    }

    Diagnostics::log_error("Could not cast to double!");
    throw std::runtime_error("Could not cast to double!");
}

Object Object::cast_to_string()
{
    switch (type)
    {
        case DataType::STRING:
            return Object(DataType::STRING, new std::string(get_str()));
        case DataType::BOOLEAN:
            return Object(DataType::STRING, new std::string(get_bool() ? "True" : "False"));
        case DataType::INTEGER:
            return Object(DataType::STRING, new std::string(std::to_string(get_int())));
        case DataType::DOUBLE:
            return Object(DataType::STRING, new std::string(std::to_string(get_double())));
        case DataType::_NULL:
            return Object(DataType::STRING, new std::string("NULL"));
    }

    Diagnostics::log_error("Could not cast to string!");
    throw std::runtime_error("Could not cast to string!");
}

DataType Object::get_type()
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
        case DataType::INTEGER:
            return Object(DataType::INTEGER, new int(get_int() + obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::DOUBLE, new double(get_double() + obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() + obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            return Object(DataType::STRING, new std::string(get_str() + obj.cast_to_string().get_str()));
    }

    Diagnostics::log_error("Could not add.");
    throw std::runtime_error("Could not add.");
}

Object Object::modulus_by(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::INTEGER, new int(get_int() % obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            Diagnostics::log_error("Cannot do modulus on a double.");
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() % obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            Diagnostics::log_error("Cannot do modulus on a string.");
    }

    Diagnostics::log_error("Could not do modulus.");
    throw std::runtime_error("Could not do modulus.");
}

Object Object::subtract_from(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::INTEGER, new int(get_int() - obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::DOUBLE, new int(get_double() - obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() - obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            Diagnostics::log_error("Cannot do subtraction on a string.");
    }

    Diagnostics::log_error("Could not subtract.");
    throw std::runtime_error("Could not subtract.");
}

Object Object::multiplied_by(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::INTEGER, new int(get_int() * obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::DOUBLE, new int(get_double() * obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() * obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            Diagnostics::log_error("Cannot do multiplication on a string.");
    }

    Diagnostics::log_error("Could not multiply.");
    throw std::runtime_error("Could not multiply.");
}

Object Object::divided_by(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::INTEGER, new int(get_int() / obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::DOUBLE, new int(get_double() / obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() / obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            Diagnostics::log_error("Cannot do division on a string.");
    }

    Diagnostics::log_error("Could not divide.");
    throw std::runtime_error("Could not divide.");
}

Object Object::negate()
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::INTEGER, new int(-1 * get_int()));
        case DataType::DOUBLE:
            return Object(DataType::DOUBLE, new int(-1 * get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(!get_bool()));
        case DataType::STRING:
            Diagnostics::log_error("Cannot negate a string.");
    }

    Diagnostics::log_error("Could not negate.");
    throw std::runtime_error("Could not negate.");
}

Object Object::equals(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::BOOLEAN, new bool(get_int() == obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::BOOLEAN, new bool(get_double() == obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() == obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            return Object(DataType::BOOLEAN, new bool(get_str() == obj.cast_to_string().get_str()));
    }

    Diagnostics::log_error("Could not equate.");
    throw std::runtime_error("Could not equate.");
}

Object Object::not_equal_to(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::BOOLEAN, new bool(get_int() != obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::BOOLEAN, new bool(get_double() != obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() != obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            return Object(DataType::BOOLEAN, new bool(get_str() != obj.cast_to_string().get_str()));
    }

    Diagnostics::log_error("Could not un-equate.");
    throw std::runtime_error("Could not un-equate.");
}

Object Object::greater_than(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::BOOLEAN, new bool(get_int() > obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::BOOLEAN, new bool(get_double() > obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() > obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            return Object(DataType::BOOLEAN, new bool(get_str() > obj.cast_to_string().get_str()));
    }

    Diagnostics::log_error("Could not apply greater than.");
    throw std::runtime_error("Could not apply greater than.");
}

Object Object::less_than(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::BOOLEAN, new bool(get_int() < obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::BOOLEAN, new bool(get_double() < obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() < obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            return Object(DataType::BOOLEAN, new bool(get_str() < obj.cast_to_string().get_str()));
    }

    Diagnostics::log_error("Could not apply less than.");
    throw std::runtime_error("Could not apply less than.");
}

Object Object::greater_than_or_equal_to(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::BOOLEAN, new bool(get_int() >= obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::BOOLEAN, new bool(get_double() >= obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() >= obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            return Object(DataType::BOOLEAN, new bool(get_str() >= obj.cast_to_string().get_str()));
    }

    Diagnostics::log_error("Could not apply greater than or equal to.");
    throw std::runtime_error("Could not apply greater than or equal to.");
}

Object Object::less_than_or_equal_to(Object obj)
{
    switch (type)
    {
        case DataType::INTEGER:
            return Object(DataType::BOOLEAN, new bool(get_int() <= obj.cast_to_int().get_int()));
        case DataType::DOUBLE:
            return Object(DataType::BOOLEAN, new bool(get_double() <= obj.cast_to_double().get_double()));
        case DataType::BOOLEAN:
            return Object(DataType::BOOLEAN, new bool(get_bool() <= obj.cast_to_bool().get_bool()));
        case DataType::STRING:
            return Object(DataType::BOOLEAN, new bool(get_str() <= obj.cast_to_string().get_str()));
    }

    Diagnostics::log_error("Could not apply less than or equal to.");
    throw std::runtime_error("Could not apply less than or equal to.");
}

Object Object::power(Object obj)
{
    if (obj.type != DataType::INTEGER)
        Diagnostics::log_error("Only integer exponents are supported.");

    int exponent = obj.get_int();
    bool wasNegative = false;

    if (exponent < 0)
    {
        exponent *= -1;
        wasNegative = true;
    }

    switch (type)
    {
        case DataType::INTEGER:
            {
                if (get_int() == 0)
                    return Object(DataType::INTEGER, new int(0));

                int result = 1;

                for (int i = 0; i < exponent; ++i)
                    result = result * get_int();

                return Object(DataType::INTEGER, new int((wasNegative && result != 0) ? 1 / result : result));
            }
        case DataType::DOUBLE:
            {
                if (get_double() == 0)
                    return Object(DataType::INTEGER, new double(0));

                double result = 1;

                for (int i = 0; i < exponent; ++i)
                    result = result * get_double();

                return Object(DataType::DOUBLE, new double((wasNegative && result != 0) ? 1 / result : result));
            }
        case DataType::BOOLEAN:
            Diagnostics::log_warning("Should not do powers on a boolean.");
            return Object(DataType::BOOLEAN, new bool(get_bool()));
        case DataType::STRING:
            Diagnostics::log_error("Cannot do powers on a string.");
    }

    Diagnostics::log_error("Could not do power.");
    throw std::runtime_error("Could not do power.");
}