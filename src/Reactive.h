#pragma once

#include "Observable.h"
#include "Binding.h"

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
            : Observable<Type>(), binding( [&](){ invalidate(); }, [&](){ update(); } )
        {
        }
        Reactive( const Type& type )
            : Observable<Type>( type ), binding( [&](){ invalidate(); }, [&](){ update(); } )
        {
        }
        Reactive( Type&& type )
            : Observable<Type>( std::forward<Type>( type ) ), binding( [&](){ invalidate(); }, [&](){ update(); } )
        {
        }
        ~Reactive()
        {
            binding.clear();
        }

        /*
         * Sets the reactive's value to be the aggregate of one or more observables. The reactive keeps this
         * reaction.
         */
        template <typename... Inputs>
        void    bind( typename const Binding<Type>::RelationFunc<Inputs...>& binder, Observable<Inputs>&... inputs )
        {
            binding.reset( binder, inputs... );

            valid = false;
        }

        /*
         * Removes the binding relationship. The reactive value will be left at the most recently set value.
         */
        void    unbind()
        {
            get(); // Ensure we have latest value. This helps ensure a well defined behaviour.

            binding.clear();
        }

        const Type& get()
        {
            if( !valid )
                update();

            return Observable<Type>::get();
        }

        template <typename StandIn>
        const Reactive& operator =( StandIn&& newValue )
        {
            if( binding )
                throw AlreadyBoundException();

            set( std::forward<StandIn>( newValue ) );
            return *this;
        }

        using Observable::operator const Type &;

    private:
        void    update()
        {
            Type temp{ std::forward<Type>( object ) };
            object = std::forward<Type>( binding() );

            valid = true;

            valueChange.notify( temp, object );
        }
        void    invalidate()
        {
            valid = false;
            change.notify();
        }

        Binding<Type>       binding;

        bool                valid = true;
    };
}