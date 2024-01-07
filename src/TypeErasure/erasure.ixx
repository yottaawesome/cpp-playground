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

// Adapted from https://github.com/aherrmann/rubber_types
// See http://aherrmann.github.io/programming/2014/10/19/type-erasure-with-merged-concepts/
export namespace RubberTypes
{
    namespace detail 
    {
        template <class T>
        class Holder 
        {
            public:
                using Data = T;

                Holder(T obj) : data_(std::move(obj)) {}
                virtual ~Holder() = default;
                T& get() { return data_; }
                const T& get() const { return data_; }

            private:
                T data_;
        };

        template <class Concept_, template <class> class Model>
        class Container 
        {
            public:
                using Concept = Concept_;

                template <class T>
                Container(T obj)
                    : self_(std::make_shared<Model<Holder<T>>>(std::move(obj))) {}

                const Concept& get() const { return *self_.get(); }

            private:
                std::shared_ptr<const Concept> self_;
        };

        // Helpers for spec merging.
        template <class Spec>
        using ConceptOf = typename Spec::Concept;
        template <class Spec, class Holder>
        using ModelOf = typename Spec::template Model<Holder>;
        template <class Spec, class Container>
        using ExternalInterfaceOf =
            typename Spec::template ExternalInterface<Container>;
        template <class Spec>
        using ContainerOf =
            detail::Container<typename Spec::Concept, Spec::template Model>;

    } // namspace detail

    template <class Spec_>
    class TypeErasure : public detail::ExternalInterfaceOf<Spec_, detail::ContainerOf<Spec_>> 
    {
        using Base =
            detail::ExternalInterfaceOf<Spec_, detail::ContainerOf<Spec_>>;

        public:
            using Base::Base;
            using Spec = Spec_;
    };

    template <class SpecA, class SpecB>
    struct MergeSpecs 
    {
        struct Concept : public virtual detail::ConceptOf<SpecA>,
            public virtual detail::ConceptOf<SpecB> {};

        template <class Holder>
        struct Model : public detail::ModelOf<SpecA, detail::ModelOf<SpecB, Holder>>, public virtual Concept 
        {
            using Base = detail::ModelOf<SpecA, detail::ModelOf<SpecB, Holder>>;
            using Base::Base;
        };

        template <class Container>
        struct ExternalInterface
            : public detail::ExternalInterfaceOf<SpecA, detail::ExternalInterfaceOf<SpecB, Container>> 
        {

            using Base = detail::ExternalInterfaceOf<
                SpecA, detail::ExternalInterfaceOf<SpecB, Container>>;
            using Base::Base;
        };
    };

    struct GreeterSpec 
    {
        struct Concept 
        {
            virtual ~Concept() = default;
            virtual void greet(const std::string& name) const = 0;
        };

        template <class Holder>
        struct Model : public Holder, public virtual Concept 
        {
            using Holder::Holder;
            virtual void greet(const std::string& name) const override 
            {
                this->Holder::get().greet(name);
            }
        };

        template <class Container>
        struct ExternalInterface : public Container 
        {
            using Container::Container;
            void greet(const std::string& name) const 
            {
                this->Container::get().greet(name);
            }
        };
    };

    struct OpenerSpec 
    {
        struct Concept 
        {
            virtual ~Concept() = default;
            virtual void open() const = 0;
        };

        template <class Holder>
        struct Model : public Holder, public virtual Concept 
        {
            using Holder::Holder;
            virtual void open() const override 
            {
                this->Model::get().open();
            }
        };

        template <class Container>
        struct ExternalInterface : public Container 
        {
            using Container::Container;
            void open() const 
            {
                this->Container::get().open();
            }
        };
    };

    using Greeter = TypeErasure<GreeterSpec>;
    using Opener = TypeErasure<OpenerSpec>;
    using OpenerAndGreeter = TypeErasure<MergeSpecs<OpenerSpec, GreeterSpec>>;

    class English 
    {
        public:
            void greet(const std::string& name) const 
            {
                std::cout << "Good day " << name << ". How are you?\n";
            }

            void open() const 
            {
                std::cout << "Squeak...\n";
            }
    };

    class French 
    {
        public:
            void greet(const std::string& name) const 
            {
                std::cout << "Bonjour " << name << ". Comment ca va?\n";
            }

            void open() const 
            {
                std::cout << "Couic...\n";
            }
    };

    void open_door(const Opener& o) 
    {
        o.open();
    }

    void greet_tom(const Greeter& g) 
    {
        g.greet("Tom");
    }

    void open_door_and_greet_john(const OpenerAndGreeter& g) 
    {
        g.open();
        g.greet("John");
    }

    void Run() 
    {
        English en;
        French fr;

        open_door(en);
        open_door(fr);
        std::cout << "----------------\n";
        greet_tom(en);
        greet_tom(fr);
        std::cout << "----------------\n";
        open_door_and_greet_john(en);
        open_door_and_greet_john(fr);
    }
}