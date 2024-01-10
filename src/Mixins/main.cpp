import std;

// Adapted from https://www.modernescpp.com/index.php/c-is-still-lazy
// crtpEquality.cpp
namespace ExampleA
{
    template<class Derived>
    class Equality {};

    template <class Derived>
    bool operator == (Equality<Derived> const& op1, Equality<Derived> const& op2) {
        Derived const& d1 = static_cast<Derived const&>(op1);
        Derived const& d2 = static_cast<Derived const&>(op2);
        return !(d1 < d2) && !(d2 < d1);
    }

    template <class Derived>
    bool operator != (Equality<Derived> const& op1, Equality<Derived> const& op2) {
        Derived const& d1 = static_cast<Derived const&>(op1);
        Derived const& d2 = static_cast<Derived const&>(op2);
        return !(op1 == op2);
    }

    struct Apple :public Equality<Apple> {
        Apple(int s) : size{ s } {};
        int size;
    };

    bool operator < (Apple const& a1, Apple const& a2) {
        return a1.size < a2.size;
    }

    struct Man :public Equality<Man> {
        Man(std::string n) : name{ n } {}
        std::string name;
    };

    bool operator < (Man const& m1, Man const& m2) {
        return m1.name < m2.name;
    }


    void Run() 
    {

        std::cout << std::boolalpha << std::endl;

        Apple apple1{ 5 };
        Apple apple2{ 10 };
        std::cout << "apple1 == apple2: " << (apple1 == apple2) << std::endl;

        Man man1{ "grimm" };
        Man man2{ "jaud" };
        std::cout << "man1 != man2: " << (man1 != man2) << std::endl;

        std::cout << std::endl;
    }
}

// Adapted from https://www.fluentcpp.com/2017/12/12/mixin-classes-yang-crtp/
namespace ExampleB
{
    template<typename Printable>
    struct RepeatPrint
    {
        void repeat(unsigned int n) const
        {
            while (n-- > 0)
            {
                static_cast<Printable const&>(*this).print();
            }
        }
    };

    class Name : public RepeatPrint<Name>
    {
        public:
            Name(std::string firstName, std::string lastName)
                : firstName_(std::move(firstName))
                , lastName_(std::move(lastName)) {}

            void print() const
            {
                std::cout << lastName_ << ", " << firstName_ << '\n';
            }

        private:
            std::string firstName_;
            std::string lastName_;
    };

    void Run()
    {
        Name ned("Eddard", "Stark");
        ned.repeat(10);
    }
}

namespace ExampleC
{
    using namespace std;

    struct Number
    {
        typedef int value_type;
        int n;
        void set(int v) { n = v; }
        int get() const { return n; }
    };

    template <typename BASE, typename T = typename BASE::value_type>
    struct Undoable : public BASE
    {
        typedef T value_type;
        T before;
        void set(T v) { before = BASE::get(); BASE::set(v); }
        void undo() { BASE::set(before); }
    };

    template <typename BASE, typename T = typename BASE::value_type>
    struct Redoable : public BASE
    {
        typedef T value_type;
        T after;
        void set(T v) { after = v; BASE::set(v); }
        void redo() { BASE::set(after); }
    };

    typedef Redoable< Undoable<Number> > ReUndoableNumber;

    void Run()
    {
        ReUndoableNumber mynum;
        mynum.set(42); mynum.set(84);
        cout << mynum.get() << '\n';  // 84
        mynum.undo();
        cout << mynum.get() << '\n';  // 42
        mynum.redo();
        cout << mynum.get() << '\n';  // back to 84
    }
}

int main() 
{
    return 0;
}
