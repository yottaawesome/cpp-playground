export module randomstuff:altvariant;
import std;

export namespace AnotherVariant
{
	// In C++26, we can specify a concept in the template parameter list 
	// for another concept. This is the syntax being proposed:
	// template<typename T, template<typename T> concept U>
	// concept TestConcept = U<T>;
	// This would simplify the code below.
	template<typename U, typename T>
	concept InvokeTypeIs = std::same_as<T, std::invoke_result_t<U>>;

	template<typename T>
	concept ConnectionLike = true;
	template<typename T>
	concept DatabaseLike = true;

	template<typename T>
	concept ProducesDatabase = DatabaseLike<std::invoke_result_t<T>>;
	template<typename T>
	concept ProducesConnection = ConnectionLike<std::invoke_result_t<T>>;

	template<typename T, InvokeTypeIs<T> auto VFactory>
	struct SomeStruct
	{
		using FactoryReturnType = std::invoke_result_t<decltype(VFactory)>;

		T t = VFactory();
		FactoryReturnType v = VFactory();
	};

	template<ProducesConnection auto VConnectionFactory, ProducesDatabase auto VDatabaseFactory>
	struct Application
	{
		using ConnectionType = std::invoke_result_t<decltype(VConnectionFactory)>;
		using DatabaseType = std::invoke_result_t<decltype(VDatabaseFactory)>;

		void Run()
		{
		}

		ConnectionType connection = VConnectionFactory();
		DatabaseType database = VDatabaseFactory();
	};

	void Run()
	{
		Application<[] { return 1; }, [] { return 2; }> app;

		auto factory = []() { return 42; };
		SomeStruct<int, factory> s;
	}
}

export namespace AltVariant
{
	struct DatabaseA {};
	struct DatabaseB {};

	template<typename T>
	auto GetDatabase(T option)
	{
		if constexpr (option == 1)
		{
			return DatabaseA{};
		}
		else
		{
			return DatabaseB{};
		}
	}

	struct ConnectionA {};
	struct ConnectionB {};

	template<typename T>
	auto GetConnection(T option)
	{
		if constexpr (option == 1)
		{
			return ConnectionA{};
		}
		else
		{
			return ConnectionB{};
		}
	}

	template<typename T>
	concept DatabaseLike = true;
	template<typename T>
	concept ConnectionLike = true;

	template<typename TToCheck, typename T>
	concept TypeOrVoid = (std::same_as<T, TToCheck> or std::same_as<T, void>);

	void RunWithOptions(ConnectionLike auto conn, DatabaseLike auto db)
	{
	}

	template<typename TConnection = void, typename TDatabase = void>
	void DetermineOptions()
	{
		if constexpr (std::same_as<TConnection, void>)
		{
			int conn = 1;
			if (conn == 1)
				DetermineOptions<ConnectionA>();
			else
				DetermineOptions<ConnectionB>();
		}
		else if constexpr (std::same_as<TDatabase, void>)
		{
			int db = 1;
			if (db == 1)
				DetermineOptions<TConnection, DatabaseA>();
			else
				DetermineOptions<TConnection, DatabaseB>();
		}
		else
		{
			RunWithOptions(TConnection{}, TDatabase{});
		}
	}

	template<ConnectionLike TConnection, DatabaseLike TDatabase>
	struct Application
	{
		void Run()
		{
		}
		TConnection connection;
		TDatabase database;
	};

	template<typename...TOptions>
	void DetermineOptions2()
	{
		if constexpr (sizeof...(TOptions) == 0)
		{
			int conn = 1;
			if (conn == 1)
				DetermineOptions2<ConnectionA>();
			else
				DetermineOptions2<ConnectionB>();
		}
		if constexpr (sizeof...(TOptions) == 1)
		{
			int db = 1;
			if (db == 1)
				DetermineOptions2<TOptions..., DatabaseA>();
			else
				DetermineOptions2<TOptions..., DatabaseB>();
		}
		else if  constexpr (sizeof...(TOptions) == 2)
		{
			RunWithOptions(TOptions{}...);
		}
	}
	
	void SetConnection(ConnectionLike auto conn, auto...other)
	{
		RunWithOptions(std::forward<decltype(conn)>(conn), std::forward<decltype(other)>(other)...);
	}

	void SetDatabase(DatabaseLike auto&& db)
	{
		int conn = 1;
		if (conn == 1)
			SetConnection(ConnectionA{}, std::forward<decltype(db)>(db));
		else
			SetConnection(ConnectionB{}, std::forward<decltype(db)>(db));
	}

	void Run()
	{
		int db = 1;
		if (db == 1)
			SetDatabase(DatabaseA{});
		else
			SetDatabase(DatabaseB{});
	}
}
