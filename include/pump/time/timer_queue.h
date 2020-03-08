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

#include "pump/time/timer.h"
#include "function/function.h"

namespace pump {
	namespace time {

		class LIB_EXPORT timer_queue
		{
		protected:
			typedef function::function<void(timer_wptr&)> timer_pending_callback;

		public:
			/*********************************************************************************
			 * Constructor
			 ********************************************************************************/
			timer_queue();

			/*********************************************************************************
			 * Deconstructor
			 ********************************************************************************/
			virtual ~timer_queue();

			/*********************************************************************************
			 * Start
			 ********************************************************************************/
			bool start(const timer_pending_callback &cb);

			/*********************************************************************************
			 * Stop
			 ********************************************************************************/
			bool stop();

			/*********************************************************************************
			 * Add timer
			 ********************************************************************************/
			void add_timer(timer_sptr &ptr);

			/*********************************************************************************
			 * Delete timer
			 ********************************************************************************/
			void delete_timer(timer_sptr &ptr);

			/*********************************************************************************
			 * Get timers queue is empty
			 ********************************************************************************/
			bool is_empty() { return timers_.empty(); }

		protected:
			/*********************************************************************************
			 * Observe thread
			 ********************************************************************************/
			void __observe_thread();

			/*********************************************************************************
			 * Observe timers
			 ********************************************************************************/
			void __observe();

		private:
			bool started_;

			timer_pending_callback pending_cb_;

			uint64 next_observe_time_;

			std::shared_ptr<std::thread> observer_;

			std::mutex observer_mx_;
			std::condition_variable observer_cv_;

			std::multimap<uint64, timer_wptr> timers_;
		};
		DEFINE_ALL_POINTER_TYPE(timer_queue);

	}
}

#endif