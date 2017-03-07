#ifndef DS_UTIL_HELPER_H
#define DS_UTIL_HELPER_H

#include <chrono>
#include <iostream>

namespace ds { namespace util {
    
    /**
     * Helpful structure to create scoped XLock for
     * multithreaded X11 programming
     */
    struct XLockDisplayGuard {
        XLockDisplayGuard(Display* d) : display(d) {
            XLockDisplay(this->display);
        }
        virtual ~XLockDisplayGuard () {
            XUnlockDisplay(this->display);
        }
    private:
        Display* display;
    };


    namespace detail {
        struct noop {
            void operator()() {}
        };
    }
    template<int Frequency, bool MissingDeadlineIsFailure, bool Debug = false>
    struct Timer {
        
        const int frequency = Frequency;
        const bool missing_deadline_is_failure = MissingDeadlineIsFailure;


        template<typename Function, typename Condition, typename DeadlineHandler>
        inline void run(Function&& function,
                        const Condition& cond,
                        DeadlineHandler&& deadlineHandler) {
            using dur_nano = std::chrono::nanoseconds;
            using dur_sec = std::chrono::seconds;
            using hr_clock = std::chrono::high_resolution_clock;
            int const updatesPerSecond = 60; //60 updates per second
            const dur_nano stepDuration = dur_nano(dur_sec(1)) / updatesPerSecond;

            hr_clock::time_point last = hr_clock::now();

            while(cond) {
                auto runtime = hr_clock::now() - last;
                if(runtime < stepDuration) {
                    auto sleepFor = stepDuration - runtime;
                    std::this_thread::sleep_for(sleepFor);
                    last += stepDuration; //normal flow
                } else if(MissingDeadlineIsFailure) {
                    deadlineHandler();
                } else {
//                    std::cout   << "Deadline missed by "
//                                << (stepDuration - runtime).count()
//                                << "ns."
//                                << std::endl;
                    //reset to now, or timer will have bias towards
                    //and try to "catch up"
                    last = hr_clock::now();
                }
            }
        }
        
        template<typename Function, typename Condition>
        inline void run(Function&& function, const Condition& cond) {
            return run(std::move(function), cond, detail::noop());
        }
    };
}}
#endif /* DS_UTIL_HELPER_H */

