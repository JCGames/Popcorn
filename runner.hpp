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
    NULL_OBJ
};

class Object
{
    ObjectType type;

    public:
        bool boolValue;
        int intValue;
        double doubleValue;
        std::string strValue;

        Object()
        {
            type = ObjectType::NULL_OBJ;
        }

        Object(std::string strValue)
        {
            this->type = ObjectType::STRING;
            this->strValue = strValue;
        }

        Object(bool boolValue)
        {
            this->type = ObjectType::BOOLEAN;
            this->boolValue = boolValue;
        }

        Object(int intValue)
        {
            this->type = ObjectType::INTEGER;
            this->intValue = intValue;
        }

        Object(double doubleValue)
        {
            this->type = ObjectType::DOUBLE;
            this->doubleValue = doubleValue;
        }

        Object cast_to_bool()
        {
            switch (type)
            {
                case ObjectType::STRING:
                    return Object((strValue == "True" || strValue == "true") ? true : 
                        ((strValue == "False" || strValue == "false") ? false : 
                        throw std::runtime_error("Could not parse string into a boolean value!")));
                case ObjectType::BOOLEAN:
                    return Object(boolValue);
                case ObjectType::INTEGER:
                    return Object((bool)intValue);
                case ObjectType::DOUBLE:
                    return Object((bool)doubleValue);
            }

            return Object();
        }

        Object cast_to_int()
        {
            switch (type)
            {
                case ObjectType::BOOLEAN:
                    return Object((int)boolValue);
                case ObjectType::INTEGER:
                    return Object(intValue);
                case ObjectType::DOUBLE:
                    return Object((int)doubleValue);
                case ObjectType::STRING:
                    {
                        int value;

                        try {
                            value = std::stoi(strValue);
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
                    return Object((double)boolValue);
                case ObjectType::INTEGER:
                    return Object((double)intValue);
                case ObjectType::DOUBLE:
                    return Object(doubleValue);
                case ObjectType::STRING:
                    {
                        double value;

                        try {
                            value = std::stof(strValue);
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
                    return Object(strValue);
                case ObjectType::BOOLEAN:
                    return Object((std::string)(boolValue ? "True" : "False"));
                case ObjectType::INTEGER:
                    return Object(std::to_string(intValue));
                case ObjectType::DOUBLE:
                    return Object(std::to_string(doubleValue));
            }

            return Object();
        }

        Object add_to(Object obj)
        {
            switch (type)
            {
                case ObjectType::BOOLEAN:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(boolValue + obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(boolValue + obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(boolValue + obj.doubleValue);
                        case ObjectType::STRING:
                            return Object(cast_to_string().strValue + obj.strValue);
                    }

                    break;
                case ObjectType::INTEGER:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(intValue + obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(intValue + obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(intValue + obj.doubleValue);
                        case ObjectType::STRING:
                            return Object(cast_to_string().strValue + obj.strValue);
                    }

                    break;
                case ObjectType::DOUBLE:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(doubleValue + obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(doubleValue + obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(doubleValue + obj.doubleValue);
                        case ObjectType::STRING:
                            return Object(cast_to_string().strValue + obj.strValue);
                    }

                    break;
                case ObjectType::STRING:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(strValue + obj.cast_to_string().strValue);
                        case ObjectType::INTEGER:
                            return Object(strValue + obj.cast_to_string().strValue);
                        case ObjectType::DOUBLE:
                            return Object(strValue + obj.cast_to_string().strValue);
                        case ObjectType::STRING:
                            return Object(strValue + obj.strValue);
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
                            return Object(boolValue - obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(boolValue - obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(boolValue - obj.doubleValue);
                        case ObjectType::STRING:
                            throw std::runtime_error("Boolean cannot be subtracted by a string!");
                    }

                    break;
                case ObjectType::INTEGER:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(intValue - obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(intValue - obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(intValue - obj.doubleValue);
                        case ObjectType::STRING:
                            throw std::runtime_error("Boolean cannot be subtracted by a string!");
                    }

                    break;
                case ObjectType::DOUBLE:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(doubleValue - obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(doubleValue - obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(doubleValue - obj.doubleValue);
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
                            return Object(boolValue * obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(boolValue * obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(boolValue * obj.doubleValue);
                        case ObjectType::STRING:
                            throw std::runtime_error("Boolean cannot be multiplied by a string!");
                    }

                    break;
                case ObjectType::INTEGER:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(intValue * obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(intValue * obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(intValue * obj.doubleValue);
                        case ObjectType::STRING:
                            throw std::runtime_error("Boolean cannot be multiplied by a string!");
                    }

                    break;
                case ObjectType::DOUBLE:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(doubleValue * obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(doubleValue * obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(doubleValue * obj.doubleValue);
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
                            return Object(boolValue / obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(boolValue / obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(boolValue / obj.doubleValue);
                        case ObjectType::STRING:
                            throw std::runtime_error("Boolean cannot be divided by a string!");
                    }

                    break;
                case ObjectType::INTEGER:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(intValue / obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(intValue / obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(intValue / obj.doubleValue);
                        case ObjectType::STRING:
                            throw std::runtime_error("Boolean cannot be divided by a string!");
                    }

                    break;
                case ObjectType::DOUBLE:

                    switch (obj.type)
                    {
                        case ObjectType::BOOLEAN:
                            return Object(doubleValue / obj.boolValue);
                        case ObjectType::INTEGER:
                            return Object(doubleValue / obj.intValue);
                        case ObjectType::DOUBLE:
                            return Object(doubleValue / obj.doubleValue);
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
                    return Object(!boolValue);
                case ObjectType::INTEGER:
                    return Object(-1 * intValue);
                case ObjectType::DOUBLE:
                    return Object(-1 * doubleValue);
                case ObjectType::STRING:
                    throw std::runtime_error("Cannot negate a string!");
            }
        }

        ObjectType get_type()
        {
            return type;
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