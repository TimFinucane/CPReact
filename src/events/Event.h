#pragma once

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

        struct ConnectionInfo
        {
        public:
            ConnectionInfo( EventNotifier& origin, const ConnectionType& connection )
                : origin( origin ), connection( connection )
            {
            }

            operator const ConnectionType&() const
            {
                return connection;
            }
            void remove()
            {
                origin.remove( connection );
            }

        private:
            const ConnectionType&   connection;
            EventNotifier&          origin;
        };
    public:
        template <typename... MethodArgs>
        ConnectionInfo  add( MethodArgs&&... args )
        {
            return ConnectionInfo( *this, list.emplace_back( std::forward<MethodArgs>( args )... ) );
        }
        void            remove( const ConnectionType& connection )
        {
            list.erase( std::remove_if( list.begin(), list.end(), 
                            [&]( const ConnectionType& el ) {  return std::addressof(el) == std::addressof(connection); } ), 
                        list.end() );
        }

        void            emit( Args&&... args )
        {
            for( auto& con : list )
                con( std::forward<Args>( args )... );
        }
    private:
        std::vector<ConnectionType> list;
    };

    /*
    * An auto connection will sever it's connection when
    *  it is destroyed. It is not used by default but can be
    *  created by:
    * AutoConnection con = EventNotifier.add( Connection );
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