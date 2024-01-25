#ifndef ANY
#define ANY

#include <memory>
#include <iostream>
#include <typeinfo>

bool DO_ANY_DEBUG = false;

class Any
{
    std::shared_ptr<void> _value_manager;
    std::string typeName;

    public:
        Any()
        {
            typeName = "NULL";
            _value_manager = std::shared_ptr<void*>(nullptr);
        }

        Any(const char* value)
        {
            set_value<std::string>(std::string(value));
        }

        template<typename T>
        Any(T value)
        {
            set_value<T>(value);
        }

        ~Any()
        {
            if (DO_ANY_DEBUG)
                std::cout << "Memory located at [" << _value_manager.get() <<  "] is being deleted.\n";
        }

        template<typename T, typename... Args>
        void set_value(Args&&... args)
        {
            _value_manager = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
            typeName = typeid(T).name();
        }

        template<typename T>
        T get_value()
        {
            if (_value_manager.get() != nullptr)
            {
                if (typeid(T).name() == typeName)
                    return *static_cast<T*>(_value_manager.get());
                else
                    throw std::runtime_error("Could not cast " + typeName + " to " + std::string(typeid(T).name()));
            }

            throw std::runtime_error("Could not delete value because it was never defined.");
        }

        std::string get_type_name() const
        {
            return typeName;
        }
};

#endif // ANY