#pragma once

#include "bindings/Observable.h"

namespace react
{
    template <typename Type>
    class Constant : public Observable<Type>
    {
    public:
        /*
        * Creates the object through its default constructor
        */
        Constant()
            : value{} // Ensure object is destroyed last
        {
        }

        Constant( const Type& object )
            : value( object )
        {
        }
        /*
        * Creates the object through the supplied instance
        */
        Constant( Type&& object )
            : value( std::forward<Type>( object ) )
        {
        }

        template <typename StandIn>
        void set( StandIn&& standin )
        {
            // This way we retain the old value, but ensure the new value is
            // contained in the class
            Type temp = std::forward<StandIn>( standin );
            std::swap( value, temp );

            updatedValue( temp, value );
        }

        const Type& get() const
        {
            return value;
        }
         
        template <typename StandIn>
        Constant<Type>& operator =( StandIn&& standin )
        {
            set( standin );
            return *this;
        }

    private:
        Type    value;
    };

}