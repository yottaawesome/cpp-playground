module;

#include <tuple>
#include <vector>

export module multivector;

// Adapted from https ://johnnysswlab.com/the-true-price-of-virtual-functions-in-c/
export
{
    template <typename... Types>
    class multivector {
    public:
        template <typename T, typename... Args>
        void emplace_back(const Args&... args) {
            std::vector<T>& my_vector = get_vector<T>();
            my_vector.emplace_back(args...);
        }

        template <typename T>
        void push_back(const T& val) {
            std::vector<T>& my_vector = get_vector<T>();
            my_vector.push_back(val);
        }

        template <typename T>
        void push_back(T&& val) {
            std::vector<T>& my_vector = get_vector<T>();
            my_vector.push_back(val);
        }

        template <typename Function>
        void for_all(Function fn) {
            for_all_p<Function, Types...>(fn);
        }

        template <typename T, typename Function>
        void for_all(Function fn) {
            for_all_p<Function, T>(fn);
        }

        template <typename T>
        std::vector<T>& get_vector() {
            return std::get<std::vector<T>>(m_vectors);
        }

        void reserve(size_t size) {}

    private:
        template <typename Function, typename T>
        void for_all_p(Function fn) {
            std::vector<T>& my_vector = get_vector<T>();
            for (auto it = my_vector.begin(); it != my_vector.end(); ++it) {
                fn(*it);
            }
        }

        template <typename Function, typename T, typename Arg, typename... Args>
        void for_all_p(Function fn) {
            for_all_p<Function, T>(fn);
            for_all_p<Function, Arg, Args...>(fn);
        }

        std::tuple<std::vector<Types>...> m_vectors;
    };
}
