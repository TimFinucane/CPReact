#pragma once

#include <list>

#include "Connection.h"

namespace react::events
{
    /*
     * Allows multiple connections to be attached and be signalled
     */
    template <typename... Args>
    class EventNotifier
    {
    public:
        using ConnectionType = Connection<Args...>;
        using iterator = typename std::list<ConnectionType>::iterator;

        struct ConnectionInfo
        {
        public:
            ConnectionInfo( EventNotifier& origin, const iterator& connection )
                : origin( origin ), connection( connection )
            {
            }

            operator const iterator&() const
            {
                return connection;
            }
            void remove()
            {
                origin.remove( connection );
            }

        private:
            EventNotifier&  origin;
            iterator        connection;
        };

    public:
        EventNotifier() {}

        template <typename... MethodArgs>
        ConnectionInfo  add( MethodArgs&&... args )
        {
            list.emplace_front( std::forward<MethodArgs>( args )... );

            return ConnectionInfo( *this, list.begin() );
        }
        void            remove( const iterator& it )
        {
            list.erase( it );
        }

        void            emit( Args&&... args )
        {
            for( auto& con : list )
                con( std::forward<Args>( args )... );
        }

        // This is to ensure the ConnectionInfo is never compromised
        EventNotifier( EventNotifier&& e ) = delete;
        EventNotifier( const EventNotifier& e ) = delete;

        const EventNotifier& operator =( EventNotifier&& ) = delete;
        const EventNotifier& operator =( const EventNotifier& ) = delete;

    private:
        std::list<ConnectionType> list;
    };

    /*
    * An auto connection will sever it's connection when
    * it is destroyed. It is not used by default but can be
    * created by:
    *  AutoConnection con = EventNotifier.add( Connection );
    * If the connection is severed before this closes, nothing happens
    */
    template <typename... Args>
    class AutoConnection
    {
        using ConnectionInfo = typename EventNotifier<Args...>::ConnectionInfo;
    public:
        AutoConnection( const ConnectionInfo& info )
            : info( info )
        {
        }
        ~AutoConnection()
        {
            info.remove();
        }

    private:
        ConnectionInfo info;
    };
}