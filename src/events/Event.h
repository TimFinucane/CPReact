#pragma once

#include <list>

#include "Listener.h"

namespace react::events
{
    /*
     * Allows multiple connections to be attached and be signalled
     */
    template <typename... Args>
    class EventNotifier
    {
    public:
        using ListenerType = Listener<Args...>;
        using iterator = typename std::list<ListenerType>::iterator;

        struct ConnectionInfo
        {
        public:
            ConnectionInfo( EventNotifier& origin, std::shared_ptr<ListenerType>& listenerRef )
                : origin( origin ), ptr( listenerRef )
            {
            }

            /*
             * Removes the listener this connection info refers to from the list
             */
            void close()
            {
                if( !ptr.expired() )
                {
                    for( auto it = origin.list.begin(), end = origin.list.end(); it != end; it++  )
                    {
                        if( !ptr.owner_before( *it ) && !(*it).owner_before( ptr ) )
                        {
                            // Swap and pop
                            std::iter_swap( it, origin.list.end() - 1 );
                            origin.list.pop_back();
                            break;
                        }
                    }
                }
                // ptr should be expired here
            }

        private:
            EventNotifier&              origin;
            std::weak_ptr<ListenerType> ptr;
        };

    public:
        EventNotifier() {}

        template <typename... MethodArgs>
        ConnectionInfo  add( MethodArgs&&... args )
        {
            return ConnectionInfo( *this, list.emplace_back( std::make_shared<ListenerType>( std::forward<MethodArgs>( args )... ) ) );
        }

        template <typename... MethodArgsUsedForForwardingReference>
        void            notify( MethodArgsUsedForForwardingReference&&... args )
        {
            for( auto& con : list )
                (*con)( std::forward<MethodArgsUsedForForwardingReference>( args )... );
        }

        // This is to ensure the ConnectionInfo is never compromised
        EventNotifier( EventNotifier&& e ) = delete;
        EventNotifier( const EventNotifier& e ) = delete;

        const EventNotifier& operator =( EventNotifier&& ) = delete;
        const EventNotifier& operator =( const EventNotifier& ) = delete;

    private:
        std::vector<std::shared_ptr<ListenerType>>  list;
    };

    /*
     * An auto connection will sever it's connection when
     * it is destroyed. It is not used by default but can be
     * created by:
     *  AutoConnection con = EventNotifier.add( Listener );
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
            info.close();
        }

        AutoConnection( const AutoConnection& ) = delete;
        AutoConnection& operator =( const AutoConnection& ) = delete;
        AutoConnection( AutoConnection&& ) = default;
        AutoConnection& operator =( AutoConnection&& ) = default;

        // Close the connection early
        void close()
        {
            info.close();
        }

    private:
        ConnectionInfo info;
    };
}