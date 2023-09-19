// Based off https://qiita.com/angeart/items/94734d68999eca575881
#pragma once
#include <iostream>
#include <tuple>
#include <type_traits>

namespace stx
{
    namespace lambda_detail
    {
        template<class Ret, class Cls, class IsMutable, class... Args>
        struct types
        {
            using is_mutable = IsMutable;

            enum { arity = sizeof...(Args) };

            using return_type = Ret;

            template<size_t i>
            struct arg
            {
                typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
            };
        };
    }

    template<class Ld>
    struct lambda_type
        : lambda_type<decltype(&Ld::operator())>
    {};

    template<class Ret, class Cls, class... Args>
    struct lambda_type<Ret(Cls::*)(Args...)>
        : lambda_detail::types<Ret, Cls, std::true_type, Args...>
    {};

    template<class Ret, class Cls, class... Args>
    struct lambda_type<Ret(Cls::*)(Args...) const>
        : lambda_detail::types<Ret, Cls, std::false_type, Args...>
    {};

};

int main() 
{
    std::cout << "[is mutable lambda]" << std::endl;
    {
        auto test = [](int a) mutable->long {return static_cast<long>(a); };
        std::cout << "ret type : " << std::is_same<stx::lambda_type<decltype(test)>::return_type, long>::value << std::endl;
        std::cout << "arg size : " << stx::lambda_type<decltype(test)>::arity << std::endl;
        std::cout << "arg 0 type : " << std::is_same<stx::lambda_type<decltype(test)>::arg<0>::type, int>::value << std::endl;
        std::cout << "is mutable : " << std::is_same<stx::lambda_type<decltype(test)>::is_mutable, std::true_type>::value << std::endl;
    }
    std::cout << "[is normal lambda]" << std::endl;
    {
        auto test = [](int a, int b)->long {return static_cast<long>(a); };
        std::cout << "ret type : " << std::is_same<stx::lambda_type<decltype(test)>::return_type, long>::value << std::endl;
        std::cout << "arg size : " << stx::lambda_type<decltype(test)>::arity << std::endl;
        std::cout << "arg 0 type : " << std::is_same<stx::lambda_type<decltype(test)>::arg<0>::type, int>::value << std::endl;
        std::cout << "is mutable : " << std::is_same<stx::lambda_type<decltype(test)>::is_mutable, std::true_type>::value << std::endl;
    }
    return 0;
}