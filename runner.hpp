#ifndef RUNNER
#define RUNNER

#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include <stdexcept>
#include "ast.hpp"

enum class ObjectType
{
    BOOLEAN,
    INTEGER,
    DOUBLE,
    STRING,
    _NULL
};

class Object
{
    ObjectType type;
    void* value;

    public:
        Object()
        {
            type = ObjectType::_NULL;
            value = NULL;
        }

        Object(std::string strValue)
        {
            this->type = ObjectType::STRING;
            this->value = new std::string(strValue);
        }

        Object(bool boolValue)
        {
            this->type = ObjectType::BOOLEAN;
            this->value = new bool(boolValue);
        }

        Object(int intValue)
        {
            this->type = ObjectType::INTEGER;
            this->value = new int(intValue);
        }

        Object(double doubleValue)
        {
            this->type = ObjectType::DOUBLE;
            this->value = new double(doubleValue);
        }

        Object(const Object& other)
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

        Object& operator=(const Object& other) 
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

        ~Object()
        {
            deleteValue(*this);
        }

        static void deleteValue(Object& obj)
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

        bool get_bool()
        {
            return *(bool*)value;
        }

        int get_int()
        {
            return *(int*)value;
        }

        double get_double()
        {
            return *(double*)value;
        }

        std::string get_str()
        {
            return *(std::string*)value;
        }

        /**
         * Casting functions
        */

        Object cast_to_bool()
        {
            switch (type)
            {
                case ObjectType::STRING:
                    return Object((get_str() == "True" || get_str() == "true") ? true : 
                        ((get_str() == "False" || get_str() == "false") ? false : 
                        throw std::runtime_error("Could not parse string into a boolean value!")));
                case ObjectType::BOOLEAN:
                    return Object(get_bool());
                case ObjectType::INTEGER:
                    return Object((bool)get_int());
                case ObjectType::DOUBLE:
                    return Object((bool)get_double());
            }

            return Object();
        }

        Object cast_to_int()
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
        
        Object cast_to_double()
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
        
        Object cast_to_string()
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

        ObjectType get_type()
        {
            return type;
        }

        /**
         * Operations
        */

        Object add_to(Object obj)
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

        Object subtract_from(Object obj)
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

        Object multiplied_by(Object obj)
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

        Object divided_by(Object obj)
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

        Object negate()
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
};

struct _Variable
{
    std::string name;
    Object object;

    _Variable(std::string name, Object object)
    {
        this->name = name;
        this->object = object;
    }
};

class Runner
{
    std::vector<_Variable> _variables;

    Object call_function(ast::FunctionCall* funcCall);

    Object interpret(ast::Statement* stat);

    void add_variable(_Variable variable);
    void remove_variable(_Variable variable);
    _Variable& get_variable(std::string name);
    bool has_variable(std::string name);

    public:
        Runner();

        void run(ast::AST& ast);
};

#endif // RUNNER