/**
 * 
 * @author Christian Marcellino
 * @date 12/30/2023
 * 
 * This class maintains objects during the runtime of the popcorn program.
 * 
*/

#ifndef OBJECT
#define OBJECT

#include <string>
#include <stdexcept>
#include "parser.hpp"

namespace popcorn::runner
{
    enum class DataType
    {
        BOOLEAN,
        INTEGER,
        DOUBLE,
        STRING,
        ARRAY,
        FUNCTION,
        CLASS,
        _NULL
    };

    struct Function;
    struct Variable;
    struct Class;

    class Object
    {
        DataType type;
        void* value;

        void delete_value(Object& obj);
        
        public:
            Object();
            Object(DataType type, void* value);
            Object(const Object& other);
            Object& operator=(const Object& other);
            ~Object();

            /**
             * Getters
            */

            void* get_value() const;
            bool get_bool();
            int get_int();
            double get_double();
            std::string get_str();
            std::vector<Object>& get_array();
            Class& get_class();
            Function& get_function();
            DataType get_type();

            /**
             * Casting functions
            */

            Object cast_to_bool();
            Object cast_to_int();
            Object cast_to_double();
            Object cast_to_string();

            /**
             * Operations
            */

            Object add_to(Object obj);
            Object modulus_by(Object obj);
            Object subtract_from(Object obj);
            Object multiplied_by(Object obj);
            Object divided_by(Object obj);
            Object negate();
            Object equals(Object obj);
            Object not_equal_to(Object obj);
            Object greater_than(Object obj);
            Object less_than(Object obj);
            Object greater_than_or_equal_to(Object obj);
            Object less_than_or_equal_to(Object obj);
            Object power(Object obj);
    };

    struct Function
    {
        std::string name;
        popcorn::parser::Node* root;

        Function()
        {
            name = "";
            root = nullptr;
        }

        Function(std::string name, popcorn::parser::Node* root)
        {
            this->name = name;
            this->root = root;
        }
    };

    struct Variable
    {
        std::string name;
        Object value;
    };

    struct Class
    {
        std::string name;
        std::vector<std::pair<std::string, Variable>> variableMembers;
        std::vector<std::pair<std::string, Function>> functionMembers;
    };
}

#endif // OBJECT