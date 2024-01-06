export module erasure;
import std;

// Adapted from https://www.modernescpp.com/index.php/type-erasure
// typeErasure.cpp
export namespace TypeErasureA
{
	class Object // (2)
    {
        public:
            template <typename T> // (3)
            Object(T&& obj) : object(std::make_shared<Model<T>>(std::forward<T>(obj))) {}

            std::string getName() const 
            {                           // (4)
                return object->getName();
            }

        private:
            struct Concept // (5)
            {
                virtual ~Concept() = default;
                virtual std::string getName() const = 0;
            };

            template<typename T> // (6)
            struct Model final : Concept 
            {
                Model(const T& t) : object(t) {}
                std::string getName() const override 
                {
                    return object.getName();
                }
                private:
                    T object;
            };

            std::shared_ptr<const Concept> object;
    };

    void printName(const std::vector<Object>& vec) // (7)
    {
        for (const auto& v : vec) 
            std::cout << v.getName() << '\n';
    }

    struct Bar 
    {
        std::string getName() const // (8)
        {
            return "Bar";
        }
    };

    struct Foo 
    {
        std::string getName() const // (8)
        {
            return "Foo";
        }
    };

    void Run()
    {
        std::print("\n");
        std::vector<Object> vec{ Object(Foo()), Object(Bar()) };  // (1)
        printName(vec);
        std::print("\n");
    }
}

// From https://cplusplus.com/articles/oz18T05o/
export namespace TypeErasureB
{

    struct Weapon {
        bool can_attack() const { return true; } // All weapons can do damage
    };

    struct Armor {
        bool can_attack() const { return false; } // Cannot attack with armor...
    };

    struct Helmet {
        bool can_attack() const { return false; } // Cannot attack with helmet...
    };

    struct Scroll {
        bool can_attack() const { return false; }
    };

    struct FireScroll {
        bool can_attack() const { return true; }
    };

    struct Potion {
        bool can_attack() const { return false; }
    };


    struct PoisonPotion {
        bool can_attack() const { return true; }
    };


    class Object {
        struct ObjectConcept {
            virtual ~ObjectConcept() {}
            virtual bool has_attack_concept() const = 0;
            virtual std::string name() const = 0;
        };

        template< typename T > struct ObjectModel : ObjectConcept {
            ObjectModel(const T& t) : object(t) {}
            virtual ~ObjectModel() {}
            virtual bool has_attack_concept() const
            {
                return object.can_attack();
            }
            virtual std::string name() const
            {
                return typeid(object).name();
            }
        private:
            T object;
        };

        std::shared_ptr<ObjectConcept> object;

    public:
        template< typename T > Object(const T& obj) :
            object(new ObjectModel<T>(obj)) {}

        std::string name() const
        {
            return object->name();
        }

        bool has_attack_concept() const
        {
            return object->has_attack_concept();
        }
    };

    void Run() 
    {
        typedef std::vector< Object >    Backpack;
        typedef Backpack::const_iterator BackpackIter;

        Backpack backpack;

        backpack.push_back(Object(Weapon()));
        backpack.push_back(Object(Armor()));
        backpack.push_back(Object(Potion()));
        backpack.push_back(Object(Scroll()));
        backpack.push_back(Object(FireScroll()));
        backpack.push_back(Object(PoisonPotion()));

        std::cout << "Items I can attack with:" << std::endl;
        for (BackpackIter item = backpack.begin(); item != backpack.end(); ++item)
            if (item->has_attack_concept())
                std::cout << item->name();
    }
}