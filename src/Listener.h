#pragma once

#include <functional>
#include <algorithm>

namespace React
{
	/*
     * Notified when an object changes only
     */
    using ChangeListener = std::function<void ()>;

    /*
     * Notified of both the old and the new value when the instance is being changed
     */
    template <typename Type>
    using ValueListener = std::function<void ( const Type&, const Type& )>;

    class ChangeObservable
    {
    public:
        void    addListener( const ChangeListener& listener )
        {
            if( !listener )
                throw; // TODO: Some kind of specific exception

            changeListeners.emplace_back( listener );
        }
        void    removeListener( const ChangeListener& listener )
        {
            std::remove_if( changeListeners.begin(), changeListeners.end(),
                            [&]( const ChangeListener& element ){ return element.target<void()>() == listener.target<void()>(); } );
        }

    protected:
        void    notifyChange()
        {
            for( ChangeListener& listener : changeListeners )
                listener();
        }

    private:
        std::vector<ChangeListener> changeListeners;
    };
}