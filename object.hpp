#ifndef OBJECT
#define OBJECT

#include <string>
#include <stdexcept>

namespace run 
{
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

        void deleteValue(Object& obj);
        
        public:
            Object();
            Object(std::string strValue);
            Object(bool boolValue);
            Object(int intValue);
            Object(double doubleValue);
            Object(const Object& other);
            Object& operator=(const Object& other);
            ~Object();

            /**
             * Getters
            */

            bool get_bool();
            int get_int();
            double get_double();
            std::string get_str();

            /**
             * Casting functions
            */

            Object cast_to_bool();
            Object cast_to_int();
            Object cast_to_double();
            Object cast_to_string();
            ObjectType get_type();

            /**
             * Operations
            */

            Object add_to(Object obj);
            Object subtract_from(Object obj);
            Object multiplied_by(Object obj);
            Object divided_by(Object obj);
            Object negate();
    };
}

#endif // OBJECT