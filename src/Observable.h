#pragma once

#include <vector>

#include "events/Event.h"

namespace react
{
    using ChangeObserver = void ();
    using ChangeNotifier = events::EventNotifier<>;

    /*
     * A reactive is an object of the given Type
     * for which modifications can be listened to.
     */
    template <typename Type>
    class Observable
    {
        using ValueNotifier = events::EventNotifier<Type, Type>;

    public:
        /*
         * Creates the object through its default constructor
         */
        Observable()
            : object{}, change{}, valueChange{} // Ensure object is destroyed last
        {
        }
        /*
         * Creates the object through the supplied instance
         */
        Observable( Type&& object )
            : object( std::forward<Type>( object ) )
        {
        }

        /*
         * Sets the object to the new value and sends notifications
         */
        void    set( Type&& newValue )
        {
            Type temp{ std::forward<Type>( object ) };
            object = std::forward<Type>( newValue );

            updatedValue( temp, object );
        }

        /*
         * Gets the value of the object as a constant (so it may not change)
         */
        virtual const Type& get()
        {
            return object;
        }

        /*
         * Sets the object to the new value and sends notifications
         */
        const Type& operator =( Type&& newValue )
        {
            set( std::forward<Type>( newValue ) );
            return get();
        }

        operator const Type&()
        {
            return get();
        }

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

        Type object;
    };
}