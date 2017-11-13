#pragma once

#include <vector>

#include "Listener.h"

namespace React
{
    /*
     * A reactive is an object of the given Type
     * for which modifications can be listened to.
     */
    template <typename Type>
    class Observable : public ChangeObservable
    {
    public:
        /*
         * Creates the object through its default constructor
         */
        Observable()
            : object{}
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
            Type temp{ std::forward<Type>(object) };
            object = std::forward<Type>(newValue);

            // We do this after as otherwise any changelistener calling get will get an old value,
            notifyChange();

            // We do this after as it follows the convention set by Reactive<>, the explanation for which will come later
            notifyValue( temp, object );
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

        using ChangeObservable::addListener;
        using ChangeObservable::removeListener;
        void    addListener( const ValueListener<Type>& listener )
        {
            if( !listener )
                throw;  // TODO: Something

            valueListeners.emplace_back( listener );
        }
        void    removeListener( const ValueListener<Type>& listener )
        {
            std::remove_if( valueListeners.begin(), valueListeners.end(), 
                            [&]( const ValueListener<Type>& element ){ return element.type() == listener.type(); } )
        }

    protected:
        void    notifyValue( const Type& oldValue, const Type& newValue )
        {
            for( ValueListener<Type> listener : valueListeners )
                listener( oldValue, newValue );
        }

        std::vector<ValueListener<Type>>    valueListeners;

    private:
        Type object;
    };
}