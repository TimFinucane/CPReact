#pragma once

#include <type_traits>
#include <tuple>

#include "Observable.h"

namespace react
{
    // Method of calling a functor with tupled types
    template <typename Functor, typename TupleType, std::size_t ...I>
    static auto call( Functor& functor, TupleType& tuple, std::index_sequence<I...> )
    {
        return functor( std::get<I>( tuple ).get()... );
    }
    template <typename Functor, typename... Inputs>
    static auto call( Functor& functor, std::tuple<Inputs...>& tuple )
    {
        return call( functor, tuple, std::index_sequence_for<Inputs...>{} );
    }

    // This has been put here as every place observable is used, this should be too
    // The operation results as listed here aren't actual results, but instead classes that contain functors capable of
    // producing a result. This allows operations to be chained together easily, as well as being converted into bindings.
    template <typename Functor, typename... Inputs>
    class Operation
    {
        using Result = typename std::invoke_result_t<Functor, typename Inputs::value_type...>;
    public:
        Operation( Functor&& functor, Inputs&... inputs )
            : functor( std::move( functor ) ), inputs( inputs... )
        {}
        Operation( Functor&& functor, std::tuple<Inputs&...>&& inputs )
            : functor( std::move( functor ) ), inputs( std::move( inputs ) )
        {}

        Result get()
        {
            return call( functor, inputs );
        }
        Result operator()()
        {
            return get();
        }

        operator typename Result()
        {
            return this->operator ()();
        }

        Functor                     functor;
        std::tuple<Inputs&...>     inputs;
    };
    template <typename Functor, typename... Inputs>
    auto make_operation( Functor&& functor, Inputs&... inputs )
    {
        return Operation<Functor, Inputs...>( std::move( functor ), inputs... );
    }
    template <typename Functor, typename... Inputs>
    auto make_operation( Functor&& functor, std::tuple<Inputs&...>&& inputs )
    {
        return Operation<Functor, typename std::decay<Inputs>::type...>( std::move( functor ), std::move( inputs ) );
    }

    template <typename TypeA, typename TypeB>
    auto operator *( Observable<TypeA>& a, Observable<TypeB>& b )
    {
        return make_operation( std::move( []( const TypeA& valA, const TypeB& valB ) { return valA * valB; } ), a, b );
    }

    template <typename FunctorA, typename... InputsA, typename FunctorB, typename... InputsB>
    auto operator +( Operation<FunctorA, InputsA...>&& a, Operation<FunctorB, InputsB...>&& b )
    {
        FunctorA functorA( a.functor );
        FunctorB functorB( b.functor );
        // Copy over the functors into this functor. With any luck this will all be inlined.
        // Then take the inputs for each operation and squeeze them into here
        return make_operation(
            std::move( [functorA, functorB]( const typename InputsA::value_type&... argsA, const typename InputsB::value_type&... argsB ) { return functorA( argsA... ) + functorB( argsB... ); } ),
            std::move( std::tuple_cat( std::move( a.inputs ), std::move( b.inputs ) ) )
        );
    }
}