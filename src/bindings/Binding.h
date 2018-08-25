#pragma once

#include <functional>

#include "Observable.h"

namespace react
{
    /*
     * A connection array holds the connections for a reactive binding, and
     * will call a given functor whenever any connection changes. It will also
     * destroy every connection if one gets destroyed. This can be listened to.
     */
    struct ConnectionArray
    {
        using Connection = events::AutoConnection<>;
    public:
        ConnectionArray( const std::function<ChangeObserver>& onChange, const std::function<void()>& onClose = {} )
            : onChange( onChange ), onClose( onClose )
        {
        }
        ConnectionArray()
            : ConnectionArray( {}, {} )
        {}
        ~ConnectionArray()
        {
            clear();
        }

        ConnectionArray( ConnectionArray&& m ) = delete;
        ConnectionArray& operator =( ConnectionArray&& m ) = delete;

        template <typename... Args>
        void reset( std::tuple<Observable<Args>&...>& listenables )
        {
            clear();
            clear();
            bindListeners<0, Args...>( listenables );
        }
        void clear()
        {
            connections.clear();
        }

        std::function<ChangeObserver>   onChange;
        std::function<void()>           onClose; // From what i understand, these functions are allowed to destroy themselves
    private:
        // Adds each of the tuple of observables to the list of listeners, whilst ensuring that no observable is listened to more
        // than once (no repetition).
        template <size_t item = 0, typename... Inputs>
        std::enable_if_t<item < sizeof...(Inputs)> bindListeners( std::tuple<Observable<Inputs>&...>& observables )
        {
            if( item == 0 || !existsInTuple<item, item - 1, Inputs...>( observables ) )
                connections.emplace_back( std::get<item>( observables ).addListener( onChange, [&](){ clear(); } ) );

            bindListeners<item + 1, Inputs...>( observables );
        }
        template <size_t item, typename... Inputs>
        std::enable_if_t<item == sizeof...(Inputs)> bindListeners( std::tuple<Observable<Inputs>&...>& ) {}
        
        // Used for checking if an item exists already in the given tuple
        template <size_t item, size_t otherItem, typename... Inputs>
        std::enable_if_t<otherItem != 0, bool> existsInTuple( std::tuple<Observable<Inputs>&...>& observables )
        {
            if( &std::get<item>( observables ) == &std::get<otherItem>( observables ) )
                return true;
            return existsInTuple<item, otherItem - 1, Inputs...>( observables );
        }
        template <size_t item, size_t otherItem, typename... Inputs>
        std::enable_if_t<otherItem == 0, bool> existsInTuple( std::tuple<Observable<Inputs>&...>& )
        {
            return false;
        }

        std::vector<Connection> connections;
    };

    template <typename Type>
    struct identity
    {
        using type = Type;
    };

    /*
     * A binding is the relationship that connects
     * inputs of observables with a function
     */
    template <typename Out>
    class Binding
    {
        template <typename... Inputs>
        using RelationFunc = typename identity<std::function<Out( const Inputs&... )>>::type;
    public:
        Binding( const std::function<ChangeObserver>& onChange, const std::function<void()>& onClose = {} )
            : connections( onChange, onClose )
        {
        }

        template <typename Functor, typename... Inputs>
        void reset( Functor binder, Observable<Inputs>&... inputs )
        {
            response = [binder, &inputs...]() -> Out{ return binder( inputs.get()... ); };
            auto tuple = std::forward_as_tuple<Observable<Inputs>&...>( inputs... );
            connections.reset<Inputs...>( tuple );
        }
        void reset()
        {
            response = {};
            connections.reset();
        }

        /*
         * Whether or not the connection is active
         */
        operator bool()
        {
            return (bool)response;
        }

        void clear()
        {
            connections.clear();
        }

        Out value()
        {
            return response();
        }
        Out operator()()
        {
            return response();
        }

    private:
        std::function<Out()>    response;
        ConnectionArray         connections;
    };

}