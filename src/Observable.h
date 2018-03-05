#pragma once

#include <vector>

#include "events/Event.h"
#include "bindings/Operation.h"

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
    class Observable : public Bindable<Type>
    {
    private:
        using ValueNotifier = events::EventNotifier<Type, Type>;

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

        /*explicit operator const Type&()
        {
            return get();
        }
        */
        /*
         * Read EventNotifier::add to see more
         */
        auto    addListener( const typename ChangeNotifier::ListenerType::Callback& callback, events::Disconnecter disconnector = {} )
        {
            return change.add( callback, disconnector );
        }
        auto    addListener( const typename ValueNotifier::ListenerType::Callback& callback, events::Disconnecter disconnector = {} )
        {
            return valueChange.add( callback, disconnector );
        }

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
}