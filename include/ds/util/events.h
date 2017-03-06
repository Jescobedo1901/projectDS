
#ifndef DS_UTIL_EVENTS_H
#define	DS_UTIL_EVENTS_H

#include <shared_mutex>

#include "util.h"

/**
 * Simple event listener helper file
 */
namespace ds {  namespace util {
    
    template<typename Event_type>
    struct event_listener {    
        
        typedef Event_type event_type;
        virtual void on_event(const event_type& e) = 0;
        
    };
    
    /**
     * Event channel class
     */    
    template<typename Event_listener_type>
    struct event_channel {
        
        event_channel() : listener_mutex(), listeners() {}
        virtual ~event_channel() {}
        
        typedef Event_listener_type event_listener_type;
                   
        /**
         * Add a listener to the channel.
         * returns true on success.
         */
        bool add_listener(Event_listener_type& listener) {
            std::unique_lock<std::shared_timed_mutex> lock(listener_mutex);
            auto end = this->listeners.end(),
                 find = std::find(this->listeners.begin(), end, &listener);
            if(find == end) {
                this->listeners.push_back(&listener);
                return true;
            } else {
                return false;
            }
        }
        /**
         * Check if event listener is registered with channel
         * returns true on success. 
         */
        bool is_listening(Event_listener_type& listener) {
            std::shared_lock<std::shared_timed_mutex> lock(listener_mutex);
            auto end = this->listeners.end(),
                 find = std::find(this->listeners.begin(), end, &listener);
            return find == end;
        }
        
        
        /**
         * Remove event listener from channel
         * returns true on success.
         */
        bool remove_listener(Event_listener_type& listener) {
            std::unique_lock<std::shared_timed_mutex> lock(listener_mutex);
            auto end = this->listeners.end(),
                 remove = std::remove(this->listeners.begin(), end, &listener);
            if(remove != end) {
                this->listeners.erase(remove, end);
                return true;
            } else {
                return false;
            }                       
        }
        
        void fire(typename Event_listener_type::event_type& event) {
            std::shared_lock<std::shared_timed_mutex> lock(this->listener_mutex);
            auto it = this->listeners.begin(),
                 end = this->listeners.end();
            for(; it != end; ++it) {
                (*it)->on_event(event);
            }
        }

    protected:        
        std::shared_timed_mutex listener_mutex;
        std::vector<Event_listener_type*> listeners;
        
    };
    
}}

#endif	/* DS_UTIL_EVENTS_H */
