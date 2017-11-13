#pragma once

#include "Observable.h"

namespace React
{
    template <typename Type>
    class Reactive : public Listenable<Type>
    {
    public:
        Reactive()
            : Listenable<Type>()
        {
        }
        Reactive( const Type& type )
            : Listenable<Type>( type )
        {
        }
        Reactive( Type&& type )
            : Listenable<Type>( std::forward<Type>( type ) )
        {
        }
        ~Reactive()
        {
            unbind();
        }

        // TODO: Invalidation and lazy loading
        template <typename... Args>
        void    bind( std::function<Type (const Args&...)> binder, Listenable<Args>&... listenables )
        {
            binding = std::bind( binder, (listenables.get())... );

            bindListeners<Args...>( listenables... );

            change();
        }
        void    unbind()
        {
            get(); // Ensure we have latest value

            for( ChangeListenable* listenable : bindingTargets )
                listenable->removeListener( onChange );

            bindingTargets.clear();
        }

        const Type& get()
        {
            if( !valid )
                set( binding() );
                
            return Listenable<Type>::get();
        }

    private:
        template <typename Arg, typename... Args>
        void        bindListeners( Listenable<Arg>& listenable, Listenable<Args>&... listenables )
        {
            listenable.addListener( onChange );
            bindingTargets.push_back( &listenable );

            bindListeners<Args...>( listenables... );
        }
        template <typename t = void>
        void        bindListeners() {}

        void        change()
        {
            if( valueListeners.size() > 0 )
                set( binding() );
            else
                valid = false;
        }

        std::function<Type ()>          binding;
        ChangeListener                  onChange = [this](){ change(); };
        std::vector<ChangeListenable*>  bindingTargets;
        bool                            valid = true;
    };
}