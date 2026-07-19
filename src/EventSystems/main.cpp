import std;

template<typename T, typename...TArgs>
concept OneOf = (std::same_as<std::remove_pointer_t<std::remove_cvref_t<T>>, TArgs> or ...);

namespace ApproachA
{
	struct EventA
	{
	};

	template<typename TConsumer, typename TEvent>
	concept Consumes = requires(TEvent event, TConsumer* consumer)
	{
		{ consumer->OnEvent(event) } -> std::same_as<void>;
	};

	template<typename...TConsumers>
	class EventBus
	{
	public:
		std::tuple<std::vector<TConsumers*>...> consumers;

		template<typename TEvent>
		void Subscribe(OneOf<TConsumers...> auto* consumer)
		{
			std::get<std::vector<decltype(consumer)>>(consumers).push_back(consumer);
		}

		void Emit(const auto& event)
		{
			using TEvent = std::remove_cvref_t<decltype(event)>;

			([&](auto&& vec)
			{
				if constexpr (Consumes<TConsumers, TEvent>)
				{
					for (auto* consumer : vec)
					{
						consumer->OnEvent(std::forward<decltype(event)>(event));
					}
				}
			}(std::get<std::vector<TConsumers*>>(consumers)), ...);
		}

		void Unsubscribe(const auto& event, auto* consumer)
		{
			using TEvent = std::remove_cvref_t<decltype(event)>;
			auto& vec = std::get<std::vector<std::remove_cvref_t<decltype(*consumer)>*>>(consumers);
			vec.erase(std::remove(vec.begin(), vec.end(), consumer), vec.end());
		}

		void UnsubscribeAll(auto* consumer)
		{
			([&](auto&& vec)
			{
				vec.erase(std::remove(vec.begin(), vec.end(), consumer), vec.end());
			}(std::get<std::vector<TConsumers*>>(consumers)), ...);
		}
	};

	class ConsumerA
	{
	public:
		void SubscribeToEventBus(auto&& bus)
		{
			bus.Subscribe<EventA>(this);
		}

		void OnEvent(const EventA& event)
		{
			std::println("ConsumerA received EventA");
		}
	};

	void Run()
	{
		auto eventBus = EventBus<ConsumerA>{};
		auto c = ConsumerA{};
		c.SubscribeToEventBus(eventBus);
		eventBus.Emit(EventA{});
	}
}

namespace ApproachB
{
	struct EventA
	{};

	template<typename T>
	class IBus
	{
	public:
		virtual ~IBus() = default;
		virtual void Subscribe(T* consumer) = 0;
	};

	template<typename TConsumer, typename TBus>
	class IBusImpl : public IBus<TConsumer>
	{
	public:
		void Subscribe(TConsumer* consumer) override
		{
			static_cast<TBus*>(this)->template SubscribeAs<TConsumer>(consumer);
		}
	};

	template<typename... TConsumers>
	class EventBus : public IBusImpl<TConsumers, EventBus<TConsumers...>>...
	{
	public:
		//using IBusImpl<TConsumers, EventBus<TConsumers...>>::Subscribe...;

		template<OneOf<TConsumers...> TConsumer>
		void SubscribeAs(TConsumer* consumer)
		{
			GetConsumers<TConsumer>().push_back(consumer);
		}

		template<OneOf<TConsumers...> TConsumer>
		auto GetConsumers() -> std::vector<TConsumer*>&
		{
			return std::get<std::vector<TConsumer*>>(consumers);
		}
	private:
		std::tuple<std::vector<TConsumers*>...> consumers;
	};

	class Consumer
	{
	public:
		Consumer(IBus<Consumer>* bus)
		{
			bus->Subscribe(this);
		}
	};

	void Run()
	{
		auto bus = EventBus<Consumer>{};
		auto c = Consumer{ &bus };
	}
}

namespace ApproachC
{
	struct EventA {};
	struct EventB {};

	template<typename TEvent>
	struct CallbackType
	{
		using Callback = auto(*)(const TEvent& event, void* arg)->void;
		Callback callback;
		void* arg = nullptr;
		constexpr void Run(const TEvent& event)
		{
			callback(event, arg);
		}
		constexpr auto operator==(const CallbackType& other) const -> bool
		{
			return callback == other.callback and arg == other.arg;
		}
	};

	template<typename TEvent>
	struct EventSubscribers
	{
		std::vector<CallbackType<TEvent>> Subscribers;
		void Subscribe(CallbackType<TEvent>::Callback callback, void* arg)
		{
			Subscribers.push_back(CallbackType<TEvent>{callback, arg});
		}
		void Unsubscribe(CallbackType<TEvent>::Callback callback, void* arg)
		{
			Subscribers.erase(
				std::remove_if(
					Subscribers.begin(),
					Subscribers.end(),
					[callback, arg](const CallbackType<TEvent>& subscriber)
					{
						return subscriber.callback == callback && subscriber.arg == arg;
					}
				),
				Subscribers.end()
			);
		}
	};

	template<typename...TEvents>
	class EventBus
	{
	public:
		template<typename TEvent>
		void Subscribe(CallbackType<TEvent>::Callback callback, void* arg)
		{
			std::get<EventSubscribers<TEvent>>(consumers).Subscribe(callback, arg);
		}
		void Emit(OneOf<TEvents...> auto&& event)
		{
			auto& subscribers = std::get<EventSubscribers<std::remove_cvref_t<decltype(event)>>>(consumers);
			for (auto&& subscriber : subscribers.Subscribers)
			{
				subscriber.Run(event);
			}
		}
	private:
		std::tuple<EventSubscribers<TEvents>...> consumers;
	};

	class ConsumerA
	{
	public:
		void SubscribeToEventBus(auto&& bus)
		{
			bus.Subscribe<EventA>(
				[](const EventA& event, void* arg) 
				{
					std::println("ConsumerA received EventA");
				}, 
				this
			);
		}
	};

	void Run()
	{
		auto eventBus = EventBus<EventA, EventB>{};
		auto consumerA = ConsumerA{};
		consumerA.SubscribeToEventBus(eventBus);
		eventBus.Emit(EventA{});
	}
}

auto main() -> int
{
	ApproachB::Run();
	return 0;
}


