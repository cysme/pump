/*
 * Copyright (C) 2015-2018 ZhengHaiTao <ming8ren@163.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef pump_timer_queue_h
#define pump_timer_queue_h

#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

#include "pump/debug.h"
#include "pump/time/timer.h"
#include "pump/toolkit/mutil_freelock_queue.h"
#include "pump/toolkit/single_freelock_queue.h"
#include "pump/toolkit/block_freelock_queue.h"

namespace pump {
namespace time {

    struct timer_context {
        uint64_t overtime;
        timer_wptr ptr;
    };

    struct timer_greater {
        constexpr bool operator()(const timer_context *left, const timer_context *right) const {
            return left->overtime > right->overtime;
        }
    };

    class timer_queue
      : public toolkit::noncopyable {

      protected:
        typedef pump_function<void(timer_wptr&)> timeout_callback;

      public:
          /*********************************************************************************
         * Create instance
         ********************************************************************************/
        PUMP_INLINE static timer_queue_sptr create() {
            INLINE_OBJECT_CREATE(obj, timer_queue, ());
            return timer_queue_sptr(obj, object_delete<timer_queue>);
        }

        /*********************************************************************************
         * Deconstructor
         ********************************************************************************/
        ~timer_queue();

        /*********************************************************************************
         * Start
         ********************************************************************************/
        bool start(const timeout_callback &cb);

        /*********************************************************************************
         * Stop
         ********************************************************************************/
        PUMP_INLINE void stop() {
            started_.store(false);
        }

        /*********************************************************************************
         * Wait stopping
         ********************************************************************************/
        void wait_stopped();

        /*********************************************************************************
         * Add timer
         ********************************************************************************/
        PUMP_INLINE bool add_timer(timer_sptr &ptr, bool repeated = false) {
            return add_timer(std::forward<timer_sptr>(ptr), repeated);
        }

        /*********************************************************************************
         * Add timer
         ********************************************************************************/
        bool add_timer(timer_sptr &&ptr, bool repeated = false);

      protected:
        /*********************************************************************************
         * Observe thread
         ********************************************************************************/
        void __observe_thread();

        /*********************************************************************************
         * Observe timers
         ********************************************************************************/
        void __observe();

        /*********************************************************************************
         * Create timer context
         ********************************************************************************/
        PUMP_INLINE timer_context* __create_timer_context(timer_sptr &ptr) {
            timer_context *ctx = nullptr;
            if (free_contexts_.empty()) {
                ctx = object_create<timer_context>();
            } else {
                ctx = free_contexts_.front();
                free_contexts_.pop();
            }
            ctx->overtime = ptr->time();
            ctx->ptr = ptr;
            return ctx;
        }

      private:
        /*********************************************************************************
         * Constructor
         ********************************************************************************/
        timer_queue() noexcept;

      private:
        // Started status
        std::atomic_bool started_;

        // Timeout callback
        timeout_callback cb_;

        // Next observer time
        uint64_t next_observe_time_;

        // Observer thread
        std::shared_ptr<std::thread> observer_;

        // Timers
        std::queue<timer_context*> free_contexts_;
        typedef toolkit::mutil_freelock_queue<timer_sptr> timer_impl_queue;
        toolkit::block_freelock_queue<timer_impl_queue> new_timers_;
        std::priority_queue<timer_context*, std::vector<timer_context*>, timer_greater> timers_;
    };
    DEFINE_ALL_POINTER_TYPE(timer_queue);

}  // namespace time
}  // namespace pump

#endif
