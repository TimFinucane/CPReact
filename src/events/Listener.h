#pragma once

#include <functional>
#include <algorithm>

namespace react::events
{
    using Disconnecter = std::function<void()>;

    /*
     * A connection handles a callback for generic notification, and
     * also notifying when the 'connection' is severed.
     */
    template <typename... Args>
    class Listener
    {
    public:
        using Callback = std::function<void( Args... )>;

    public:
        Listener( const Callback& callback, std::function<void()> disconnecter = {} )
            : callback( callback ), disconnecter( std::forward<Disconnecter>( disconnecter ) )
        {
        }
        Listener( Callback&& callback, std::function<void()> disconnecter = {} )
            : callback( std::forward<Callback>( callback ) ),
            disconnecter( std::forward<Disconnecter>( disconnecter ) )
        {
        }
        ~Listener()
        {
            if( disconnecter )
                disconnecter();
        }

        /*
         * Calls the associated callback
         */
        template <typename... ForwardReferencingArgs>
        void    call( ForwardReferencingArgs&&... args )
        {
            callback( std::forward<ForwardReferencingArgs>( args )... );
        }
        /*
         * Calls the associated callback
         */
        template <typename... ForwardReferencingArgs>
        void    operator()( ForwardReferencingArgs&&... args )
        {
            callback( std::forward<ForwardReferencingArgs>( args )... );
        }

        bool    operator ==( const Listener<Args...>& con ) const
        {
            return callback.target<void( Args... )>() == con.callback.target<void (Args...)>() && 
                disconnecter.target<void ()>() == con.disconnecter.target<void()>();
        }

    private:
        Callback        callback;
        Disconnecter    disconnecter;
    };
}
