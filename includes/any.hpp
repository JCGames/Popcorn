#ifndef ANY
#define ANY

#include <memory>
#include <iostream>
#include <typeinfo>

class any
{
    std::shared_ptr<void> _valueManager;
    std::string _typeName;

    public:
        static bool DO_ANY_DEBUG;

        any()
        {
            _typeName = "NULL";
            _valueManager = std::shared_ptr<void*>(nullptr); 
        }

        any(const any& other)
        {
            _valueManager = other._valueManager;
            _typeName = other._typeName;
        }

        any& operator=(const any& other)
        {
            _valueManager = other._valueManager;
            _typeName = other._typeName;
            return *this;
        }

        ~any()
        {
            if (DO_ANY_DEBUG)
                std::cout << "Memory located at [" << _valueManager.get() <<  "] is being deleted.\n";
        }

        template<typename T, typename... Args>
        void set_value(Args&&... args)
        {
            _valueManager = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
            _typeName = typeid(T).name();
        }

        template<typename T>
        T get_value()
        {
            if (_valueManager.get() != nullptr)
            {
                if (typeid(T).name() == _typeName)
                    return *static_cast<T*>(_valueManager.get());
                else
                    throw std::runtime_error("Could not cast " + _typeName + " to " + std::string(typeid(T).name()));
            }

            throw std::runtime_error("Could not get value because it was never defined.");
        }

        template<typename T>
        T* get_value_as_pointer()
        {
            if (_valueManager.get() != nullptr)
            {
                if (typeid(T).name() == _typeName)
                    return static_cast<T*>(_valueManager.get());
                else
                    throw std::runtime_error("Could not cast " + _typeName + " to " + std::string(typeid(T).name()));
            }

            throw std::runtime_error("Could not get value because it was never defined.");
        }

        bool is_null()
        {
            return _valueManager.get() == nullptr;
        }

        std::string get_type_name() const
        {
            return _typeName;
        }

        template<typename T, typename... Args>
        static any create(Args&&... args)
        {
            any result;
            result.set_value<T>(std::forward<Args>(args)...);
            return result;
        }
};

#endif // ANY