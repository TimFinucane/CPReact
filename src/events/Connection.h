#pragma once

#include <functional>
#include <algorithm>

namespace react::events
{
    /*
     * A connection handles a callback for generic notification, and
     * also notifying when the 'connection' is severed.
     */
    template <typename... Args>
    class Connection
    {
        using Callback = std::function<void( Args... )>;
        using Disconnecter = std::function<void()>;
    public:
        Connection( Callback&& callback )
            : Connection( std::forward<Callback>(callback), {} )
        {
        }
        Connection( Callback&& callback, std::function<void()> disconnecter )
            : callback( std::forward<Callback>( callback ) ),
            disconnecter( std::forward<Disconnecter>( disconnecter ) )
        {
        }
        ~Connection()
        {
            if( disconnecter )
                disconnecter();
        }

        /*
         * Calls the associated callback
         */
        void    call( Args&&... args )
        {
            callback( std::forward<Args>( args )... );
        }
        /*
         * Calls the associated callback
         */
        void    operator()( Args&&... args )
        {
            callback( std::forward<Args>( args )... );
        }

        bool    operator ==( const Connection<Args...>& con ) const
        {
            return callback.target<void( Args... )>() == con.callback.target<void (Args...)>() && 
                disconnecter.target<void ()>() == con.disconnecter.target<void()>();
        }

    private:
        Callback        callback;
        Disconnecter    disconnecter;
    };
}
