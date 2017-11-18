#pragma once

#include "Observable.h"
#include "Binding.h"

namespace react
{
    /*
     * A reactive is an observable variable that can also be bound
     * to other observables, so that whenever it's dependants change, it does too.
     */
    template <typename Type>
    class Reactive : public Observable<Type>
    {
    public:
        Reactive()
            : Observable<Type>(), binding( [&](){ valid = false; } )
        {
        }
        Reactive( const Type& type )
            : Observable<Type>( type ), binding( [&](){ valid = false; } )
        {
        }
        Reactive( Type&& type )
            : Observable<Type>( std::move( type ) ), binding( [&](){ valid = false; } )
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
         * Removes the binding relationship. The reactive value will left at the most recently set value.
         */
        void    unbind()
        {
            get(); // Ensure we have latest value

            binding.clear();
        }

        const Type& get()
        {
            if( !valid )
                set( binding() );

            return Observable<Type>::get();
        }

        using Observable::operator=;
        using Observable::operator const Type &;

    private:
        Binding<Type>       binding;

        bool                valid = true;
    };
}