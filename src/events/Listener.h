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
        Listener( const Callback& callback, std::function<void()> onSever = {} )
            : callback( callback ), onSever( std::forward<Disconnecter>(onSever) )
        {
        }
        Listener( Callback&& callback, std::function<void()> onSever = {} )
            : callback( std::forward<Callback>( callback ) ),
            onSever( std::forward<Disconnecter>(onSever) )
        {
        }
        ~Listener()
        {
            if(onSever)
                onSever();
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

        // Cleanly closes the listener to prevent a sever call
        void    close()
        {
            onSever = {};
        }

    private:
        Callback        callback;
        Disconnecter    onSever;
    };
}
