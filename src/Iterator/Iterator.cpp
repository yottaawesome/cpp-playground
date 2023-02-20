// Adapted from https://www.modernescpp.com/index.php?option=com_content&view=article&id=683&catid=58

#include <iostream>

class Generator 
{
    int begin_{};
    int end_{};

    public:
        Generator(int begin, int end) : begin_{ begin }, end_{ end } {}

        class Iterator 
        {
            int value_{};

            public:
                explicit Iterator(int pos) : value_{ pos } {}

                int operator*() const { return value_; }           // (3)

                Iterator& operator++() 
                {                           // (4)
                    ++value_;
                    return *this;
                }

                bool operator!=(const Iterator& other) const 
                {      // (5)
                    return value_ != other.value_;
                }
        };

        Iterator begin() const { return Iterator{ begin_ }; }     // (1)
        Iterator end() const { return Iterator{ end_ }; }         // (2)
};

int main() {

    std::cout << '\n';

    Generator gen{ 100, 110 };
    for (auto v : gen) std::cout << v << " ";

    std::cout << "\n\n";
}