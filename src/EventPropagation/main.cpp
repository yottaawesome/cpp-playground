import std;

namespace Events
{
    struct Y
    {
        template<typename T>
        auto M(this T self)
        {
            if constexpr (requires (const T & t) { t.Blah(); })
                return self.Blah();
            else
                return self.Default();
        }

        auto Default()
        {
            return 1;
        }
    };

    struct X : Y
    {
        auto Blah(this auto self)
        {
            return 10;
        }
    };

    struct SomeEvent { int X = 0; };
    struct SomeOtherEvent {};

    struct HandlerA
    {
    };

    struct HandlerB
    {
        void Handle(SomeEvent event)
        {
            std::println("I was successfully called {}.", event.X);
        }
    };

    template<typename T, typename...Ts>
    concept OneOf = (std::same_as<std::remove_cvref_t<T>, Ts> or ...);

    template<typename...Ts>
    struct Registrations
    {
        std::tuple<std::vector<Ts>...> Tuples{ std::vector<Ts>{}... };

        void AddHandler(OneOf<Ts...> auto&& handle)
        {
            Get<decltype(handle)>().push_back(handle);
        }

        template<typename T>
        auto& Get()
        {
            return std::get<std::vector<std::remove_cvref_t<T>>>(Tuples);
        }

        void Raise(auto&& event)
        {
            ([this]<typename M = Ts>(auto&& event)
            {
                if constexpr (requires (M m) { m.Handle(event); })
                {
                    auto& handlers = Get<M>();
                    for (auto&& handler : handlers)
                        handler.Handle(event);
                }
            }(event), ...);
        }
    };


    void Run()
    {
        std::tuple AA{ HandlerA{} } ;
        std::tuple BB{ HandlerB{} };
        auto M = std::tuple_cat(AA, BB);
        Registrations<decltype(M)> registrationsA;



        Registrations<HandlerA, HandlerB> registrations;
        registrations.AddHandler(HandlerA{});
        registrations.AddHandler(HandlerB{});
        registrations.Raise(SomeEvent{12});
    }
}


int main()
{
    Events::Run();
    return 0;
}
