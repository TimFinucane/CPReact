#pragma once

#include "Observable.h"

namespace react
{
    /*
     * A connection array holds the connections for a reactive binding, and
     * will call a given array when any of these change.
     */
    struct ConnectionArray
    {
        using Connection = events::AutoConnection<>;
    public:
        ConnectionArray( const std::function<ChangeObserver>& onChange )
            : onChange( onChange ), count( 0 ), connections( nullptr )
        {
        }
        ~ConnectionArray()
        {
            clear();
        }

        template <typename... Args>
        void reset( Observable<Args>&... listenables )
        {
            clear();

            count = sizeof...(Args);
            connections = static_cast<Connection*>(::operator new(sizeof( Connection ) * count));

            bindListeners( 0, listenables... );
        }
        void clear()
        {
            if( count > 0 )
            {
                delete connections;
                count = 0;
            }
        }

    private:
        template <typename Arg, typename... Args>
        void        bindListeners( int i, Observable<Arg>& listenable, Observable<Args>&... listenables )
        {
            new (connections + i) Connection( listenable.addListener( onChange ) );

            bindListeners<Args...>( i + 1, listenables... );
        }
        template <typename T = void>
        void        bindListeners( int ) {}

        std::function<ChangeObserver>   onChange;

        Connection* connections;
        size_t      count;
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
            : Observable<Type>(),
            dependantConnections( [&](){ valid = false; } )
        {
        }
        Reactive( const Type& type )
            : Observable<Type>( type ),
            dependantConnections( [&](){ valid = false; } )
        {
        }
        Reactive( Type&& type )
            : Observable<Type>( std::move( type ) ),
            dependantConnections( [&](){ valid = false; } )
        {
        }
        ~Reactive()
        {
            unbind();
        }

        template <typename... Args>
        void    bind( const std::function<Type (const Args&...)>& binder, Observable<Args>&... listenables )
        {
            binding = std::bind( binder, (listenables.get())... );

            dependantConnections.reset( listenables... );

            valid = false;
        }
        void    unbind()
        {
            get(); // Ensure we have latest value

            binding = {};
            dependantConnections.clear();
        }

        const Type& get()
        {
            if( !valid )
                set( binding() );
                
            return Observable<Type>::get();
        }

    private:
        std::function<Type ()>          binding;

        ConnectionArray                 dependantConnections;

        bool                            valid = true;
    };
}