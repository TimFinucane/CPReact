#pragma once

#include "bindings/Observable.h"
#include "bindings/Operation.h"
#include "bindings/Binding.h"

namespace react
{
    // TODO: Proper debugging info.
    class AlreadyBoundException : public std::exception
    {
    public:
        AlreadyBoundException()
            : std::exception( "Tried to assign a value to a reactive variable that has already been bound" )
        {}
    };

    /*
     * A reactive is an observable variable that can also be bound
     * to other observables, so that whenever it's dependants change, it does too.
     */
    template <typename Type>
    class Reactive : public Observable<Type>
    {
    public:
        Reactive()
            : value{}, binding( [&]() { invalidate(); }, [&]() { update(); } )
        {
        }
        Reactive( const Type& type )
            : value( type ), binding( [&](){ invalidate(); }, [&](){ update(); } )
        {
        }
        Reactive( Type&& type )
            : value( std::forward<Type>( type ) ), binding( [&](){ invalidate(); }, [&](){ update(); } )
        {
        }
        template <typename SimilarType>
        Reactive( Observable<SimilarType>& observable )
            : Reactive()
        {
            bind( []( SimilarType input ) { return (Type)input; }, observable );
        }
        template <typename Functor, typename... Inputs>
        explicit Reactive( Operation<Functor, Inputs...>&& operation )
            : Reactive()
        {
            std::tuple<Inputs&...> in_tuple = std::move( operation.inputs );
            bind_from_tuple( std::move( operation.functor ), in_tuple );
        }
        ~Reactive()
        {
            binding.clear();
        }

        /*
         * Sets the reactive's value to be the aggregate of one or more observables. The reactive keeps this
         * reaction.
         */
        template <typename Functor, typename... Inputs>
        void    bind( Functor&& binder, Observable<Inputs>&... inputs )
        {
            binding.reset( std::move(binder), inputs... );
            valid = false;
        }
        // Alternative binding to tuple
        template <typename Functor, typename... Inputs>
        void bind_from_tuple( Functor&& binder, const std::tuple<Inputs&...>& tuple )
        {
            bind_from_tuple_indices( std::move( binder ), tuple, std::index_sequence_for<Inputs...>{} );
        }

        /*
         * Removes the binding relationship. The reactive value will be left at the most recently set value.
         */
        void    unbind()
        {
            get(); // Ensure we have latest value. This helps ensure a well defined behaviour.

            binding.clear();
        }

        template <typename StandIn>
        void set( StandIn&& standin )
        {
            if( binding )
                throw AlreadyBoundException();

            // This way we retain the old value, but ensure the new value is
            // contained in the class
            Type temp = std::forward<StandIn>( standin );
            std::swap( value, temp );

            change.notify();
            valueChange.notify( temp, value );
        }

        const Type& get() const
        {
            if( !valid )
                const_cast<Reactive<Type>*>(this)->update();

            return value;
        }

        template <typename StandIn>
        const Reactive& operator =( StandIn&& newValue )
        {
            set( std::forward<StandIn>( newValue ) );
            return *this;
        }

        //using Observable::operator const Type &;

    private:
        template <typename Functor, typename TupleType, std::size_t... I>
        void bind_from_tuple_indices( Functor&& binder, const TupleType& tuple, std::index_sequence<I...> )
        {
            bind( std::move( binder ), std::get<I>( tuple )... );
        }

        void    update()
        {
            // This way we retain the old value, but ensure the new value is
            // contained in the class
            Type temp = binding();
            std::swap( value, temp );

            valid = true;

            valueChange.notify( temp, value );
        }
        void    invalidate()
        {
            valid = false;
            change.notify();
        }

        Binding<Type>       binding;

        mutable bool        valid = true;
        mutable Type        value;
    };
}