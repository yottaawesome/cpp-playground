// Adapted from https://www.modernescpp.com/index.php/type-erasure
// typeErasure.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Object // (2)
{
    public:
        template <typename T> // (3)
        Object(T&& obj) : object(std::make_shared<Model<T>>(std::forward<T>(obj))) {}

        std::string getName() const 
        {                           // (4)
            return object->getName();
        }

    private:
        struct Concept // (5)
        {
            virtual ~Concept() = default;
            virtual std::string getName() const = 0;
        };

        template<typename T> // (6)
        struct Model final : Concept 
        {
            Model(const T& t) : object(t) {}
            std::string getName() const override 
            {
                return object.getName();
            }
            private:
                T object;
        };

        std::shared_ptr<const Concept> object;
};

void printName(const std::vector<Object>& vec) // (7)
{
    for (const auto& v : vec) 
        std::cout << v.getName() << '\n';
}

struct Bar 
{
    std::string getName() const // (8)
    {
        return "Bar";
    }
};

struct Foo 
{
    std::string getName() const // (8)
    {
        return "Foo";
    }
};

int main()
{
    std::cout << '\n';
    std::vector<Object> vec{ Object(Foo()), Object(Bar()) };  // (1)
    printName(vec);
    std::cout << '\n';

    return 0;
}