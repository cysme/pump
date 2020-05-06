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

#ifndef pump_utils_spin_mutex_h
#define pump_utils_spin_mutex_h

#include "pump/deps.h"

namespace pump {
	namespace utils {

		class LIB_PUMP spin_mutex
		{
		public:
			/*********************************************************************************
			 * Constructor
			 ********************************************************************************/
			spin_mutex(int32 per_loop = 3) PUMP_NOEXCEPT;

			/*********************************************************************************
			 * Deconstructor
			 ********************************************************************************/
			~spin_mutex() = default;

			/*********************************************************************************
			 * Lock
			 ********************************************************************************/
			void lock();

			/*********************************************************************************
			 * Try lock
			 ********************************************************************************/
			PUMP_INLINE bool try_lock()
			{
				bool exp = false;
				return locked_.compare_exchange_strong(exp, true);
			}

			/*********************************************************************************
			 * Unlock
			 ********************************************************************************/
			PUMP_INLINE void unlock()
			{ locked_.store(false); }

			/*********************************************************************************
			 * Get locked status
			 ********************************************************************************/
			PUMP_INLINE bool is_locked() PUMP_CONST
			{ return locked_.load(); }

		private:
			int32 per_loop_;
			std::atomic_bool locked_;
		};

	}
}

#endif