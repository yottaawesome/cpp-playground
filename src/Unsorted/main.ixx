export module main;
import std;

// Adapted from the code and explanations here:
// https://en.cppreference.com/w/cpp/language/rule_of_three.html

namespace RuleOfZero 
{
    struct rule_of_zero
    {
        std::string cppstring;
        rule_of_zero(const std::string& arg) : cppstring(arg) {}
    };
}
namespace RuleOfThree 
{
    // If a class requires a user-defined destructor, a user-defined 
    // copy constructor, or a user-defined copy assignment operator, 
    // it almost certainly requires all three. 
    struct rule_of_three
    {
        char* cstring; // raw pointer used as a handle to a
        // dynamically-allocated memory block

        explicit rule_of_three(const char* s = "") : cstring(nullptr)
        {
            if (s)
            {
				auto size = std::strlen(s) + 1;
                cstring = new char[size]; // allocate
                std::copy_n(s, size, cstring); // populate
            }
        }

        ~rule_of_three() // I. destructor
        {
            delete[] cstring; // deallocate
        }

        rule_of_three(const rule_of_three& other) // II. copy constructor
            : rule_of_three(other.cstring) {
        }

        rule_of_three& operator=(const rule_of_three& other) // III. copy assignment
        {
            // implemented through copy-and-swap for brevity
            // note that this prevents potential storage reuse
            rule_of_three temp(other);
            std::swap(cstring, temp.cstring);
            return *this;
        }

        const char* c_str() const // accessor
        {
            return cstring;
        }
    };

    void Run()
    {
        rule_of_three o1{ "abc" };
        std::cout << o1.c_str() << ' ';
        auto o2{ o1 }; // II. uses copy constructor
        std::cout << o2.c_str() << ' ';
        rule_of_three o3("def");
        std::cout << o3.c_str() << ' ';
        o3 = o2; // III. uses copy assignment
        std::cout << o3.c_str() << '\n';
    }   // I. all destructors are called here
}
namespace RuleOfFive 
{
    /*
    * Because the presence of a user-defined (include = default or = delete declared) 
    * destructor, copy-constructor, or copy-assignment operator prevents implicit 
    * definition of the move constructor and the move assignment operator, any class 
    * for which move semantics are desirable, has to declare all five special member 
    * functions: 
    */
    struct rule_of_five
    {
        char* cstring; // raw pointer used as a handle to a
        // dynamically-allocated memory block
        explicit rule_of_five(const char* s = "") : cstring(nullptr)
        {
            if (s)
            {
                auto size = std::strlen(s) + 1;
                cstring = new char[size]; // allocate
                std::copy_n(s, size, cstring); // populate
            }
        }

        ~rule_of_five()
        {
            delete[] cstring; // deallocate
        }

        rule_of_five(const rule_of_five& other) // copy constructor
            : rule_of_five(other.cstring) {
        }

        rule_of_five(rule_of_five&& other) noexcept // move constructor
            : cstring(std::exchange(other.cstring, nullptr)) {
        }

        rule_of_five& operator=(const rule_of_five& other) // copy assignment
        {
            // implemented as move-assignment from a temporary copy for brevity
            // note that this prevents potential storage reuse
            return *this = rule_of_five(other);
        }

        rule_of_five& operator=(rule_of_five&& other) noexcept // move assignment
        {
            std::swap(cstring, other.cstring);
            return *this;
        }

        // alternatively, replace both assignment operators with copy-and-swap
        // implementation, which also fails to reuse storage in copy-assignment.
        //  rule_of_five& operator=(rule_of_five other) noexcept
        //  {
        //      std::swap(cstring, other.cstring);
        //      return *this;
        //  }
    };

    void Run()
    {
    }
}

namespace BufferTests
{
    struct Buffer
    {
        constexpr Buffer(int rows, int cols)
            : Data(rows* cols, 0), Rows(rows), Columns(cols)
        {
        }

        constexpr auto operator[](this auto&& self, int row, int columns) -> decltype(auto)
        {
            return std::forward_like<decltype(self)>(self.Data[row * self.Rows + columns]);
        }
        std::vector<int> Data;
        size_t Rows = 0;
        size_t Columns = 0;
    };

    static_assert(
        [] -> bool
        {
            Buffer buffer(3, 4);
            if (buffer.Data.size() != 12ull)
                return false;
            buffer[1, 2] = 42;
            if (buffer[1, 2] != 42)
                return false;
            return true;
        }());

    struct Q
    {
        int a[2][2];
        constexpr Q(auto&&...values)
            : a{ values... }
        {
        }
    };

    void Run()
    {
        Q q(0, 1, 2, 3);
        for (int row = 0; row < 2; ++row)
            for (int col = 0; col < 2; ++col)
                std::println("{}", q.a[row][col]);
    }
}



namespace Test
{
    template<typename T>
    concept is_arithmetic = std::is_arithmetic_v<T>;

    template<is_arithmetic TArithmetic, std::uint32_t VRows, std::uint32_t VColumns>
    struct matrix
    {
        static_assert(VRows > 0 and VColumns > 0, "Matrix must have at least one row and one column.");
        constexpr matrix() = default;
        constexpr matrix(const matrix&) = default;

        constexpr matrix(std::convertible_to<TArithmetic> auto...values)
            : Values{ static_cast<TArithmetic>(values)... }
        { }

        constexpr auto operator[](
            this auto&& self,
            std::uint32_t row,
            std::uint32_t column
        ) noexcept -> decltype(auto)
        {
            return std::forward_like<decltype(self)>(self.Values[row][column]);
        }

        constexpr auto rows() const noexcept -> std::uint32_t
        {
            return VRows;
        }

        constexpr auto columns() const noexcept -> std::uint32_t
        {
            return VColumns;
        }

        constexpr auto set(
            std::uint32_t row,
            std::uint32_t column,
            std::convertible_to<TArithmetic> auto value
        ) noexcept -> void
        {
            Values[row][column] = static_cast<TArithmetic>(value);
		}

		constexpr auto operator*(this auto&& self, TArithmetic scale) noexcept -> decltype(auto)
        {
            decltype(auto) result =
                [](auto&& s) -> decltype(auto)
                {
                    if constexpr (std::is_const_v<std::remove_reference_t<decltype(s)>>)
                        return matrix{ s };
                    else
                        return std::forward<decltype(s)>(s);
                }(std::forward<decltype(self)>(self));

            for (std::uint32_t r = 0; r < VRows; ++r)
                for (std::uint32_t c = 0; c < VColumns; ++c)
                    result[r,c]*=scale;
            return result;
        }

        TArithmetic Values[VRows][VColumns]{};
    };

	using matrix4x4_f = matrix<float, 4, 4>;
    constexpr matrix4x4_f Identity4{
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    static_assert(
        []
        {
            matrix4x4_f f(Identity4);
            f * 4;
			Identity4 * 4;
            return f[0,0]==4 and Identity4[0,0]==1;
        }()
    );

    void Test()
    {
        matrix4x4_f f(Identity4);
        f*4;
    }
}

struct X { float Value = 0; };
struct Y { float Value = 0; };
struct Z { float Value = 0; };
template<typename T>
concept is_xyz = std::same_as<T, X> or std::same_as<T, Y> or std::same_as<T, Z>;

struct Vector
{
	Vector(float x, float y, float z) 
        : x(x), y(y), z(z) { }
    Vector(is_xyz auto... values)
	{
        ([this, values]
        {
            if constexpr(std::same_as<decltype(values), X>)
                x = values.Value;
            if constexpr (std::same_as<decltype(values), Y>)
                y = values.Value;
            if constexpr (std::same_as<decltype(values), Z>)
                z = values.Value;
        }(), ...);
	}

	float x = 0, y = 0, z = 0;
};

struct A { A(int...) {} };
struct B : A { using A::A; };
struct C { ~C() { std::println("Being destroyed."); } };

namespace SomeTestNamespace
{
    struct A {};
    struct B {};
    struct C {};
	using Messages = std::variant<A, B, C>;


    struct Base 
    {
        void Process(this auto& self, const Messages& message)
        {
            std::visit(
                [&self](auto&& msg)
                {
                    using T = std::decay_t<decltype(msg)>;
                    if constexpr (std::is_same_v<T, A>)
                    {
                        self.OnMessage(msg);
                    }
                    else if constexpr (std::is_same_v<T, B>)
                    {
                        self.OnMessage(msg);
                    }
                    else if constexpr (std::is_same_v<T, C>)
                    {
                        self.OnMessage(msg);
                    }
                },
				message);
        }
    };

    struct Derived : Base
    {
	};
}


// Adapted from https://www.youtube.com/watch?v=9laCL5GixNk
namespace BindFront
{
    int add(int x, int y) {
        return x + y;
    }

    void use(const auto& f) {}

    template<auto Func, auto...Constexprs, typename...Param>
    constexpr auto bind_front(Param && ... param) 
    {
        if constexpr (sizeof...(param) == 0) 
        {
            return
                []<typename...Inner>(Inner&&...inner) static 
                {
                    return Func(Constexprs..., std::forward<Inner>(inner)...);
                };
        }
        else 
        {
            return
                [...param = std::forward<Param>(param)]<typename...Inner>(Inner&&...inner) 
                {
                    return Func(Constexprs..., param..., std::forward<Inner>(inner)...);
                };
        }
    }

    void Run()
    {
        auto add5 = bind_front<add, 5>();
        use(add5);
	}
}

extern "C" int main()
{
    auto i = new C;
    auto ptrA = std::unique_ptr<C>(i);
    ptrA = std::unique_ptr<C>(i);


    /*B b(1,2,3,3);
    Vector v{ Y{1} };
    float f[2][2]{
        {0,1},
        {2,3}
    };
    std::ranges::for_each(f, [](auto&& row) {
        std::ranges::for_each(row, [](auto&& v) {
            std::println("{}", v);
            });
		});*/
	return 0;
}
