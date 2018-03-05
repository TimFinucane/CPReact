#pragma once

// The further you go into this file the more you will find me an abomination of god.

#include <type_traits>
#include <tuple>

namespace react
{
    template <typename Type>
    class Bindable
    {
    public:
        using reactive_type = Type;

        /*
        * Gets the value of the object as a constant (so it may not change)
        */
        virtual const Type& get() const = 0;
    };

    // Method of calling a functor with tupled types
    template <typename Functor, typename TupleType, std::size_t ...I>
    auto call( Functor& functor, TupleType& tuple, std::index_sequence<I...> )
    {
        return functor( std::get<I>( tuple ).get()... );
    }
    template <typename Functor, typename... Inputs>
    auto call( Functor& functor, std::tuple<Inputs...>& tuple )
    {
        return call( functor, tuple, std::index_sequence_for<Inputs...>{} );
    }

    // This has been put here as every place observable is used, this should be too
    // The operation results as listed here aren't actual results, but instead classes that contain functors capable of
    // producing a result. This allows operations to be chained together easily, as well as being converted into bindings.
    template <typename Functor, typename... Inputs>
    class Operation // Breaks if I try to extend from bindable
    {
    public:
        using reactive_type = std::invoke_result_t<Functor, typename Inputs::reactive_type...>;
    public:
        Operation( Functor&& functor, Inputs&... inputs )
            : functor( std::move( functor ) ), inputs( inputs... )
        {}
        Operation( Functor&& functor, std::tuple<Inputs&...>&& inputs )
            : functor( std::move( functor ) ), inputs( std::move( inputs ) )
        {}

        reactive_type get()
        {
            return call( functor, inputs );
        }
        reactive_type operator()()
        {
            return get();
        }

        operator typename reactive_type()
        {
            return this->operator ()();
        }

        Functor                     functor;
        std::tuple<Inputs&...>      inputs;
    };

    // Template functions for auto making
    template <typename Functor, typename... Inputs>
    auto make_operation( Functor&& functor, Inputs&... inputs )
    {
        return Operation<Functor, Inputs...>( std::move( functor ), inputs... );
    }
    template <typename Functor, typename... Inputs>
    auto make_operation( Functor&& functor, std::tuple<Inputs&...>&& inputs )
    {
        return Operation<Functor, Inputs...>( std::move( functor ), std::move( inputs ) );
    }

    template <typename TypeA, typename TypeB, typename std::is_convertible<TypeB, int>::type>
    auto operator +( Bindable<TypeA>& a, TypeB& b )
    {
        return make_operation( std::move( [b]( const TypeA& valA ) { return valA + b; } ), a );
    }
    template <typename TypeA, typename TypeB, typename std::is_convertible<TypeB, int>::type>
    auto operator -( Bindable<TypeA>& a, TypeB& b )
    {
        return make_operation( std::move( [b]( const TypeA& valA ) { return valA - b; } ), a );
    }
    template <typename TypeA, typename TypeB, typename std::is_convertible<TypeB, int>::type>
    auto operator *( Bindable<TypeA>& a, TypeB& b )
    {
        return make_operation( std::move( [b]( const TypeA& valA ) { return valA * b; } ), a );
    }
    template <typename TypeA, typename TypeB, typename std::is_convertible<TypeB, int>::type>
    auto operator /( Bindable<TypeA>& a, TypeB& b )
    {
        return make_operation( std::move( [b]( const TypeA& valA ) { return valA / b; } ), a );
    }

    template <typename FunctorA, typename... InputsA, typename TypeB>
    auto operator +( Operation<FunctorA, InputsA...>&& a, TypeB& b )
    {
        FunctorA functorA( a.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA, b]( const typename InputsA::reactive_type&... argsA ) { return functorA( argsA... ) + b; } ),
            std::move( a.inputs )
        );
    }
    template <typename FunctorA, typename... InputsA, typename TypeB>
    auto operator -( Operation<FunctorA, InputsA...>&& a, TypeB& b )
    {
        FunctorA functorA( a.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA, b]( const typename InputsA::reactive_type&... argsA ) { return functorA( argsA... ) - b; } ),
            std::move( a.inputs )
        );
    }
    template <typename FunctorA, typename... InputsA, typename TypeB>
    auto operator *( Operation<FunctorA, InputsA...>&& a, TypeB& b )
    {
        FunctorA functorA( a.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA, b]( const typename InputsA::reactive_type&... argsA ) { return functorA( argsA... ) * b; } ),
            std::move( a.inputs )
        );
    }
    template <typename FunctorA, typename... InputsA, typename TypeB>
    auto operator /( Operation<FunctorA, InputsA...>&& a, TypeB& b )
    {
        FunctorA functorA( a.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA, b]( const typename InputsA::reactive_type&... argsA ) { return functorA( argsA... ) / b; } ),
            std::move( a.inputs )
        );
    }

    template <typename TypeA, typename TypeB>
    auto operator +( Bindable<TypeA>& a, Bindable<TypeB>& b )
    {
        return make_operation(
            std::move( []( const TypeA& valA, const TypeB& valB ) { return valA + valB; } ),
            a, b
        );
    }
    template <typename TypeA, typename TypeB>
    auto operator -( Bindable<TypeA>& a, Bindable<TypeB>& b )
    {
        return make_operation(
            std::move( []( const TypeA& valA, const TypeB& valB ) { return valA - valB; } ),
            a, b
        );
    }
    template <typename TypeA, typename TypeB>
    auto operator *( Bindable<TypeA>& a, Bindable<TypeB>& b )
    {
        return make_operation(
            std::move( []( const TypeA& valA, const TypeB& valB ) { return valA * valB; } ),
            a, b
        );
    }
    template <typename TypeA, typename TypeB>
    auto operator /( Bindable<TypeA>& a, Bindable<TypeB>& b )
    {
        return make_operation(
            std::move( []( const TypeA& valA, const TypeB& valB ) { return valA / valB; } ),
            a, b
        );
    }

    template <typename FunctorA, typename... InputsA, typename TypeB>
    auto operator +( Operation<FunctorA, InputsA...>&& a, Bindable<TypeB>& b )
    {
        FunctorA functorA( a.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA]( const typename InputsA::reactive_type&... argsA, const TypeB& b ) { return functorA( argsA... ) + b; } ),
            std::move( std::tuple_cat( std::move( a.inputs ), std::tie( b ) ) )
        );
    }
    template <typename FunctorA, typename... InputsA, typename TypeB>
    auto operator -( Operation<FunctorA, InputsA...>&& a, Bindable<TypeB>& b )
    {
        FunctorA functorA( a.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA]( const typename InputsA::reactive_type&... argsA, const TypeB& b ) { return functorA( argsA... ) - b; } ),
            std::move( std::tuple_cat( std::move( a.inputs ), std::tie( b ) ) )
        );
    }
    template <typename FunctorA, typename... InputsA, typename TypeB>
    auto operator *( Operation<FunctorA, InputsA...>&& a, Bindable<TypeB>& b )
    {
        FunctorA functorA( a.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA]( const typename InputsA::reactive_type&... argsA, const TypeB& b ) { return functorA( argsA... ) * b; } ),
            std::move( std::tuple_cat( std::move( a.inputs ), std::tie( b ) ) )
        );
    }
    template <typename FunctorA, typename... InputsA, typename TypeB>
    auto operator /( Operation<FunctorA, InputsA...>&& a, Bindable<TypeB>& b )
    {
        FunctorA functorA( a.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA]( const typename InputsA::reactive_type&... argsA, const TypeB& b ) { return functorA( argsA... ) / b; } ),
            std::move( std::tuple_cat( std::move( a.inputs ), std::tie( b ) ) )
        );
    }

    template <typename TypeA, typename FunctorB, typename... InputsB>
    auto operator +( Bindable<TypeA>& a, Operation<FunctorB, InputsB...>&& b )
    {
        FunctorB functorB( b.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorB]( const TypeA& a, const typename InputsB::reactive_type&... argsB ) { return a + functorB( argsB... ); } ),
            std::move( std::tuple_cat( std::tie( a ), std::move( b.inputs ) ) )
        );
    }
    template <typename TypeA, typename FunctorB, typename... InputsB>
    auto operator -( Bindable<TypeA>& a, Operation<FunctorB, InputsB...>&& b )
    {
        FunctorB functorB( b.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorB]( const TypeA& a, const typename InputsB::reactive_type&... argsB ) { return a - functorB( argsB... ); } ),
            std::move( std::tuple_cat( std::tie( a ), std::move( b.inputs ) ) )
        );
    }
    template <typename TypeA, typename FunctorB, typename... InputsB>
    auto operator *( Bindable<TypeA>& a, Operation<FunctorB, InputsB...>&& b )
    {
        FunctorB functorB( b.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorB]( const TypeA& a, const typename InputsB::reactive_type&... argsB ) { return a * functorB( argsB... ); } ),
            std::move( std::tuple_cat( std::tie( a ), std::move( b.inputs ) ) )
        );
    }
    template <typename TypeA, typename FunctorB, typename... InputsB>
    auto operator /( Bindable<TypeA>& a, Operation<FunctorB, InputsB...>&& b )
    {
        FunctorB functorB( b.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorB]( const TypeA& a, const typename InputsB::reactive_type&... argsB ) { return a / functorB( argsB... ); } ),
            std::move( std::tuple_cat( std::tie( a ), std::move( b.inputs ) ) )
        );
    }

    template <typename FunctorA, typename... InputsA, typename FunctorB, typename... InputsB>
    auto operator +( Operation<FunctorA, InputsA...>&& a, Operation<FunctorB, InputsB...>&& b )
    {
        FunctorA functorA( a.functor );
        FunctorB functorB( b.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA, functorB]( const typename InputsA::reactive_type&... argsA, const typename InputsB::reactive_type&... argsB ) { return functorA( argsA... ) + functorB( argsB... ); } ),
            std::move( std::tuple_cat( std::move( a.inputs ), std::move( b.inputs ) ) )
        );
    }
    template <typename FunctorA, typename... InputsA, typename FunctorB, typename... InputsB>
    auto operator -( Operation<FunctorA, InputsA...>&& a, Operation<FunctorB, InputsB...>&& b )
    {
        FunctorA functorA( a.functor );
        FunctorB functorB( b.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA, functorB]( const typename InputsA::reactive_type&... argsA, const typename InputsB::reactive_type&... argsB ) { return functorA( argsA... ) - functorB( argsB... ); } ),
            std::move( std::tuple_cat( std::move( a.inputs ), std::move( b.inputs ) ) )
        );
    }
    template <typename FunctorA, typename... InputsA, typename FunctorB, typename... InputsB>
    auto operator *( Operation<FunctorA, InputsA...>&& a, Operation<FunctorB, InputsB...>&& b )
    {
        FunctorA functorA( a.functor );
        FunctorB functorB( b.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA, functorB]( const typename InputsA::reactive_type&... argsA, const typename InputsB::reactive_type&... argsB ) { return functorA( argsA... ) * functorB( argsB... ); } ),
            std::move( std::tuple_cat( std::move( a.inputs ), std::move( b.inputs ) ) )
        );
    }
    template <typename FunctorA, typename... InputsA, typename FunctorB, typename... InputsB>
    auto operator /( Operation<FunctorA, InputsA...>&& a, Operation<FunctorB, InputsB...>&& b )
    {
        FunctorA functorA( a.functor );
        FunctorB functorB( b.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA, functorB]( const typename InputsA::reactive_type&... argsA, const typename InputsB::reactive_type&... argsB ) { return functorA( argsA... ) / functorB( argsB... ); } ),
            std::move( std::tuple_cat( std::move( a.inputs ), std::move( b.inputs ) ) )
        );
    }
}