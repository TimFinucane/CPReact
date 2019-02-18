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

        ConnectionArray( ConnectionArray&& m ) = delete;
        ConnectionArray& operator =( ConnectionArray&& m ) = delete;

        // TODO: Add initial form of this method to call when no initial deletion needs to occur (no clear())
        template <typename... Args>
        void reset( std::tuple<Observable<Args>&...>& listenables )
        {
            clear();
            bindListeners<0, Args...>( listenables );
        }
        void clear()
        {
            onClose();
            connections.clear();
        }

    private:
        std::function<ChangeObserver>   onChange;
        std::function<void()>           onClose; // From what i understand, these functions are allowed to destroy themselves
    private:
        // Adds each of the tuple of observables to the list of listeners, whilst ensuring that no observable is listened to more
        // than once (no repetition).
        template <size_t item = 0, typename... Inputs>
        void bindListeners( std::tuple<Observable<Inputs>&...>& observables )
        {
            #pragma warning(suppress: 4127) // This compiler warning is incorrect, following is not constexpr (and will not compile if changed).
            if( item == 0 || !existsInTuple<item, Inputs...>( observables ) )
                connections.emplace_back( std::get<item>( observables ).addListener( onChange, [&](){ clear(); } ) );

            if constexpr( item + 1 < sizeof...(Inputs) )
                bindListeners<item + 1, Inputs...>( observables );
        }

        // Used for checking if an item exists already in the given tuple
        template <size_t item, size_t otherItem, typename... Inputs>
        static bool existsInTuple( const std::tuple<Observable<Inputs>&...>& observables )
        {
            if( &std::get<item>( observables ) == &std::get<otherItem>( observables ) )
                return true;

            if constexpr( otherItem > 0 )
                return existsInTuple<item, otherItem - 1, Inputs...>( observables );

            return false;
        }
        template <size_t item, typename... Inputs>
        static bool existsInTuple( const std::tuple<Observable<Inputs>&...>& observables )
        {
            if constexpr( item == 0 )
                return false;

            return existsInTuple<item, item - 1, Inputs...>( observables );
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
            : connections(onChange, [this, onClose]() { onClose(); response = {}; })
        {
        }

        // TODO: Add initial form of this method to call when no initial deletion needs to occur
        template <typename Functor, typename... Inputs>
        void reset( Functor binder, Observable<Inputs>&... inputs )
        {
            auto tuple = std::forward_as_tuple<Observable<Inputs>&...>( inputs... );
            connections.reset<Inputs...>( tuple );
            response = [binder, &inputs...]()->Out{ return binder(inputs.get()...); };
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