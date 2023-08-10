import std;

// Simplified from https://github.com/joboccara/NamedType/blob/master/include/NamedType/named_type_impl.hpp
// in order to understand the concept better.

template <typename T, template <typename> class crtpType>
struct crtp
{
    constexpr T& underlying()
    {
        return static_cast<T&>(*this);
    }
    constexpr T const& underlying() const
    {
        return static_cast<T const&>(*this);
    }
};

template <typename T>
struct PostIncrementable : crtp<T, PostIncrementable>
{
    constexpr T operator++(int)
    {
        return T(this->underlying().get()++);
    }
};

template <typename T>
using IsNotReference = typename std::enable_if<!std::is_reference<T>::value, void>::type;


// See https://learn.microsoft.com/en-us/cpp/cpp/empty-bases?view=msvc-170
template<typename T, typename Parameter, template <typename> class... Skills>
class __declspec(empty_bases) NamedType : public Skills<NamedType<T, Parameter, Skills...>>...
{
    public:
        NamedType() = default;

        explicit constexpr NamedType(T const& value) noexcept(std::is_nothrow_copy_constructible_v<T>) : value_(value)
        {
        }

        template <typename T_ = T, typename = IsNotReference<T_>>
        explicit constexpr NamedType(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
            : value_(std::move(value))
        {
        }

        constexpr T& get() noexcept
        {
            return value_;
        }

        constexpr std::remove_reference_t<T> const& get() const noexcept
        {
            return value_;
        }

        // conversions
        using ref = NamedType<T&, Parameter, Skills...>;
        operator ref()
        {
            return ref(value_);
        }

    private:
        T value_;
};

int main()
{
    NamedType<int, struct IntA> Blah1;
    NamedType<int, struct IntB, PostIncrementable> Blah2;

    Blah2++;

    std::cout << "Hello World!\n";
    return 0;
}
