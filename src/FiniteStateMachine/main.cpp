// Adapted from https://www.cppstories.com/2023/finite-state-machines-variant-vending-cpp/
import std;
import std.compat;

namespace helper 
{
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
    //template<class... Ts> overload(Ts...)->overload<Ts...>; // no need in C++20, MSVC?
}

namespace state 
{
    struct Idle { };
    struct AmountEntered 
    { 
        int amount{ 0 }; 
        int availableChange{ 0 }; 
    };
    struct ItemSelected 
    { 
        std::string item; 
        int availableChange{ 0 }; 
    };
    struct ChangeDispensed 
    { 
        int change{ 0 }; 
    };
}

using VendingState = std::variant<state::Idle, state::AmountEntered, state::ItemSelected, state::ChangeDispensed>;

namespace event 
{
    struct EnterAmount 
    { 
        int amount{ 0 }; 
    };
    struct SelectItem 
    { 
        std::string item; 
    };
    struct DispenseChange { };
    struct Reset { };
}

using PossibleEvent = std::variant<event::EnterAmount, event::SelectItem, event::DispenseChange, event::Reset>;

class VendingMachine 
{
    struct Item 
    { 
        std::string name; 
        unsigned quantity{ 0 }; 
        int price{ 0 }; 
    };

    public:
        void processEvent(const PossibleEvent& event) 
        {
            state_ = std::visit(
                helper::overload{
                    [this](const auto& state, const auto& evt) 
                    {
                        return onEvent(state, evt);
                    }
                }, 
                state_, 
                event
            );
        }

        VendingState onEvent(const state::Idle& idle, const event::EnterAmount& amount) 
        {
            std::cout << std::format("Idle -> EnterAmount: {}\n", amount.amount);
            return state::AmountEntered{ amount.amount, amount.amount };
        }

        VendingState onEvent(const state::AmountEntered& current, const event::EnterAmount& amount) 
        {
            std::cout << std::format("AmountEntered {} -> EnterAmount: {}\n", current.amount, amount.amount);
            return state::AmountEntered{ current.amount + amount.amount, current.availableChange + amount.amount };
        }

        VendingState onEvent(const state::AmountEntered& amountEntered, const event::SelectItem& item) 
        {
            std::cout << std::format("AmountEntered {} -> SelectItem: {}\n", amountEntered.amount, item.item);

            auto it = std::ranges::find(registry_, item.item, &Item::name);
            if (it == registry_.end())
                throw std::logic_error{ "Item not found in item registry." };

            if (it->quantity > 0 && it->price <= amountEntered.amount)
            {
                std::cout << "item found...\n";
                --(it->quantity);
                return state::ItemSelected{ item.item, amountEntered.availableChange - it->price };
            }
            return amountEntered;
        }

        VendingState onEvent(const state::ItemSelected& itemSelected, const event::DispenseChange& change) 
        {
            std::cout << std::format("ItemSelected -> DispenseChange {}\n", itemSelected.availableChange);
            return state::ChangeDispensed{ itemSelected.availableChange };
        }

        VendingState onEvent(const state::ChangeDispensed& changeDispensed, const event::Reset& reset) 
        {
            std::cout << "ChangeDispensed -> Reset\n";
            return state::Idle{ };
        }

        VendingState onEvent(const auto&, const auto&) 
        {
            throw std::logic_error{ "Unsupported event transition" };
        }

        void reportCurrentState() 
        {
            std::visit(
                helper::overload{ 
                    [](const state::Idle& idle) 
                    {
                        std::cout << "Idle, waiting...\n";
                    },
                    [](const state::AmountEntered& amountEntered) 
                    {
                        std::cout << std::format("AmountEntered: {}, available change: {}\n", amountEntered.amount, amountEntered.availableChange);
                    },
                    [](const state::ItemSelected& itemSelected) 
                    {
                        std::cout << std::format("ItemSelected: {}, available change: {}\n", itemSelected.item, itemSelected.availableChange);
                    },
                    [](const state::ChangeDispensed& changeDispensed) 
                    {
                        std::cout << std::format("ChangeDispensed: {}\n", changeDispensed.change);
                    }
                }, 
                state_
            );
        }

        void reportRegistry() 
        {
            std::cout << "available items: \n";
            for (auto&& item : registry_)
                std::cout << std::format(" {}, price {}, quantity {}\n", item.name, item.price, item.quantity);
        }

    private:
        std::vector<Item> registry_{
            Item {"Coke", 5, 50},
            Item {"Pepsi", 3, 45},
            Item {"Water", 4, 35},
            Item {"Snack", 5, 25}
        };

        VendingState state_;
};

void VendingMachineTest() 
{
    VendingMachine vm;
    vm.reportRegistry();

    try 
    {
        vm.processEvent(event::EnterAmount { 30 });
        vm.processEvent(event::EnterAmount { 30 });
        //vm.reportCurrentState();
        vm.processEvent(event::SelectItem { "Coke" });
        vm.reportRegistry();
        //vm.reportCurrentState();
        vm.processEvent(event::DispenseChange {});
        //vm.reportCurrentState();
        vm.processEvent(event::Reset { });
        //vm.reportCurrentState();
    }
    catch (std::exception& ex) 
    {
        std::cout << "Exception! " << ex.what() << '\n';
    }
}

int main()
{
    VendingMachineTest();
    return 0;
}
