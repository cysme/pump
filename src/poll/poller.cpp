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

#include "pump/poll/poller.h"

namespace pump {
namespace poll {

    poller::poller() noexcept
        : started_(false), cev_cnt_(0), cevents_(1024), tev_cnt_(0), tevents_(1024) {
    }

    bool poller::start() {
        if (started_.load())
            return false;

        started_.store(true);

        worker_.reset(object_create<std::thread>([&]() {
                          while (started_.load() || !trackers_.empty()) {
                              __handle_channel_events();

                              __handle_channel_tracker_events();

                              if (cev_cnt_.load() > 0 || tev_cnt_.load() > 0)
                                  __poll(0);
                              else
                                  __poll(3);
                          }
                      }),
                      object_delete<std::thread>);

        return true;
    }

    void poller::wait_stopped() {
        if (worker_) {
            worker_->join();
            worker_.reset();
        }
    }

#if !defined(PUMP_HAVE_IOCP)
    bool poller::add_channel_tracker(channel_tracker_sptr &tracker) {
        // Mark tracker started
        if (started_.load() && tracker->set_tracked(true)) {
            // Mark tracker started
            tracker->mark_started(true);
            // Create tracker event
            auto tev = object_create<channel_tracker_event>(tracker, TRACKER_EVENT_ADD);
            PUMP_DEBUG_CHECK(tevents_.enqueue(tev));
            tev_cnt_++;

            return true;
        }

        return false;
    }

    void poller::remove_channel_tracker(channel_tracker_sptr &tracker) {
        // Mark tracker no started
        tracker->mark_started(false);
        // Create tracker event
        auto tev = object_create<channel_tracker_event>(tracker, TRACKER_EVENT_DEL);
        PUMP_DEBUG_CHECK(tevents_.enqueue(tev));
        tev_cnt_++;
    }

    void poller::resume_channel_tracker(channel_tracker_ptr tracker) {
        if (tracker->set_tracked(true) && tracker->is_started()) {
            __resume_channel_tracker(tracker);
        }
    }
#endif

    void poller::push_channel_event(channel_sptr &c, uint32 event) {
        if (started_.load()) {
            auto cev = object_create<channel_event>(c, event);
            PUMP_DEBUG_CHECK(cevents_.enqueue(cev));
            cev_cnt_++;
        }
    }

    void poller::__handle_channel_events() {
        channel_event_ptr ev = nullptr;
        int32 cnt = cev_cnt_.exchange(0);
        while (cnt > 0 && cevents_.try_dequeue(ev)) {
            PUMP_LOCK_WPOINTER(ch, ev->ch);
            if (ch)
                ch->handle_channel_event(ev->event);

            object_delete(ev);

            cnt--;
        }
    }

    void poller::__handle_channel_tracker_events() {
        auto cnt = tev_cnt_.exchange(0);
        channel_tracker_event_ptr ev = nullptr;
        while (cnt > 0 && tevents_.try_dequeue(ev)) {
            do {
                auto tracker = ev->tracker.get();

                PUMP_LOCK_SPOINTER(ch, tracker->get_channel());
                if (ch == nullptr) {
                    trackers_.erase(tracker);
                    break;
                }

                if (ev->event == TRACKER_EVENT_ADD) {
                    // Must be tracked
                    PUMP_ASSERT(tracker->is_tracked());
                    // Insert into tracker list
                    trackers_[tracker] = ev->tracker;
                    PUMP_DEBUG_CHECK(__add_channel_tracker(tracker));
                } else if (ev->event == TRACKER_EVENT_DEL) {
                    // Try to untrack channel
                    //__remove_channel_tracker(tracker);
                    // Remove from tracker list
                    trackers_.erase(tracker);
                }

                ch->handle_tracker_event(ev->event);

            } while (false);

            object_delete(ev);

            cnt--;
        }
    }

}  // namespace poll
}  // namespace pump
