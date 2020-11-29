#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <Windows.h>

class A
{
	public:
		virtual ~A() {}
		virtual void DoX() { std::cout << "OK 1" << std::endl; }
};

class B : public A
{
	public:
		virtual ~B() {}
		virtual void DoX() override { std::cout << "OK 2" << std::endl; }
};

/*
template<typename T>
void FindIn(std::vector<T>& results, const std::vector<T>& search, const std::function<bool(const T&)>& predicate)
{
	auto iter = search.begin();

	while (iter != search.end())
	{
		iter = std::find_if(iter, search.end(), predicate);
		if (iter != search.end())
		{
			results.push_back(*iter);
			iter++;
		}
	}
}

void TestIterators()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	std::cout << st.wHour << std::endl;

	std::vector<int> search{ 0,1,5,0,5,9,4 };
	std::vector<int> results;
	FindIn<int>(results, search, [](const int& value) -> bool { return value == 5; });
	for (const int& i : results)
		std::cout << i << std::endl;
}
*/

int main()
{
	//testVector.push_back(B());

	return 0;
}
