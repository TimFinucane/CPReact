#pragma once

#include <vector>

#include "events/Event.h"

namespace react
{
    using ChangeObserver = void();
    using ChangeNotifier = events::EventNotifier<>;

    /*
     * An observable is an abstact class defining the interface
     * for being able to observe changes in a value, but contains
     * no functionality for setting that value.
     */
    template <typename Type>
    class Observable
    {
    private:
        using ValueNotifier = events::EventNotifier<Type, Type>;

    public:
        using reactive_type = Type;

    public:
        /*
         * Sets the object to the new value and sends notifications
         */
        template <typename StandIn>
        const Observable& operator =( StandIn&& newValue )
        {
            set( std::forward<StandIn>( newValue ) );
            return *this;
        }

        const Type& operator ()() const
        {
            return get();
        }
        
        /*
         * Read EventNotifier::add to see more
         */
        template <typename Callback, typename = std::invoke_result_t<typename Callback>>
        ChangeNotifier::ConnectionInfo addListener( Callback&& callback, events::Disconnecter disconnector = {} )
        {
            return change.add( std::forward<Callback>( callback ), disconnector );
        }
        template <typename Callback, typename = std::invoke_result_t<typename Callback, typename Type, typename Type>>
        typename ValueNotifier::ConnectionInfo addListener( Callback&& callback, events::Disconnecter disconnector = {} )
        {
            return valueChange.add( std::forward<Callback>( callback ), disconnector );
        }

        /*
         * Gets the value of the object as a constant (so it may not change)
         */
        virtual const Type& get() const = 0;

    protected:
        /*
         * Informs of an updated value
         */
        void updatedValue( const Type& prev, const Type& next )
        {
            // We do this after in Observable as otherwise any changelistener calling get will get an old value
            change.notify();

            // We do this after as it follows the convention set by Reactive<>, the explanation for which will come later
            valueChange.notify( prev, next );
        }

        ChangeNotifier  change;
        ValueNotifier   valueChange;
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
}