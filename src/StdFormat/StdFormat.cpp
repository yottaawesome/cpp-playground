// Adappted from https://www.cppstories.com/2023/fun-print-tables-format/
#include <format>
#include <iostream>
#include <map>
#include <array>
#include <ranges>
#include <iomanip>
#include <chrono>
#include <algorithm>

template <typename T>
size_t MaxKeyLength1(const std::map<std::string, T>& m) {
	auto res = std::ranges::max_element(m, [](const auto& a, const auto& b) {
		return a.first.length() < b.first.length();
		});
	return res->first.length();
}

template <typename T>
size_t MaxKeyLength(const std::map<std::string, T>& m) {
	auto res = std::ranges::max_element(std::views::keys(m), [](const auto& a, const auto& b) {
		return a.length() < b.length();
		});
	return (*res).length();
}

int main() {
	using namespace std::chrono;

	constexpr size_t NumRows = 5;
	std::chrono::year_month_day startDate{ 2023y, month{February}, 20d };
	const std::map<std::string, std::array<double, NumRows>> productsToOrders{
		{ "apples", {100, 200, 50.5, 30, 10}},
		{ "bananas", {80, 10, 100, 120, 70}},
		{ "carrots", {130, 75, 25, 64.5, 128}},
		{ "tomatoes", {70, 100, 170, 80, 90}}
	};

	const auto ColLength = MaxKeyLength(productsToOrders) + 2;

	// print headers:
	std::cout << std::format("{:>{}}", "date", ColLength);
	for (const auto& name : std::views::keys(productsToOrders))
		std::cout << std::format("{:>{}}", name, ColLength);
	std::cout << '\n';

	// print values:
	for (size_t i = 0; i < NumRows; ++i) {
		const auto nextDay = sys_days{ startDate } + days{ i };
		std::cout << std::format("{:>{}}", nextDay, ColLength);
		for (const auto& values : std::views::values(productsToOrders)) {
			std::cout << std::format("{:>{}.2f}", values[i], ColLength);
		}
		std::cout << '\n';
	}
}