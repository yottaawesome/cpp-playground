import std;

namespace Events
{
    struct Y
    {
        template<typename T>
        auto M(this T self)
        {
            if constexpr (requires (T t) { t.Blah(); })
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

    void J(auto y)
    {
        std::println("{}", y.M());
    }

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

        void AddHandler(this auto& self, OneOf<Ts...> auto&& handle)
        {
            self.Get<decltype(handle)>().push_back(handle);
        }

        template<typename T>
        auto& Get(this auto& self)
        {
            return std::get<std::vector<std::remove_cvref_t<T>>>(self.Tuples);
        }

        void Raise(this auto& self, auto&& event)
        {
            ([]<typename M = Ts>(auto& self, auto&& event)
            {
                if constexpr (requires (M m) { m.Handle(event); })
                {
                    auto& handlers = self.Get<M>();
                    for (auto&& handler : handlers)
                        handler.Handle(event);
                }
            }(self, event), ...);
        }
    };

    template<typename...Ts>
    struct Registrations<std::tuple<Ts...>> : Registrations<Ts...> { };

    void Run()
    {
        X x;
        J(x);

        std::tuple AA{ HandlerA{} } ;
        std::tuple BB{ HandlerB{} };
        auto M = std::tuple_cat(AA, BB);

        Registrations<decltype(M)> registrationsA;
        registrationsA.AddHandler(HandlerA{});
        registrationsA.AddHandler(HandlerB{});
        registrationsA.Raise(SomeEvent{ 12 });

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
