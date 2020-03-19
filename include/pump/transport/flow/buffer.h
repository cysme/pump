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

#ifndef pump_transport_flow_buffer_h
#define pump_transport_flow_buffer_h

#include "pump/deps.h"

namespace pump {
	namespace transport {
		namespace flow {

			#define MAX_FLOW_BUFFER_SIZE 4096

			class LIB_EXPORT buffer
			{
			public:
				/*********************************************************************************
				 * Constructor
				 ********************************************************************************/
				buffer(): rpos_(0)
				{}

				/*********************************************************************************
				 * Append
				 ********************************************************************************/
				bool append(c_block_ptr b, uint32 size)
				{
					if (!b || size == 0)
						return false;

					if (rpos_ != 0 && rpos_ == (uint32)raw_.size())
						reset();

					raw_.append(b, size);

					return true;
				}

				/*********************************************************************************
				 * Reset
				 ********************************************************************************/
				void reset()
				{
					rpos_ = 0;
					raw_.clear();
				}

				/*********************************************************************************
				 * Shift
				 ********************************************************************************/
				bool shift(uint32 size)
				{
					if (size == 0 || raw_.size() < rpos_ + size)
						return false;
					rpos_ += size;
					return true;
				}

				/*********************************************************************************
				 * Get buffer raw ptr
				 ********************************************************************************/
				c_block_ptr raw() const
				{
					if (raw_.empty())
						return nullptr;
					return (c_block_ptr)raw_.data();
				}

				/*********************************************************************************
				 * Get buffer raw size
				 ********************************************************************************/
				uint32 raw_size() const 
				{ 
					return (uint32)raw_.size(); 
				}

				/*********************************************************************************
				 * Get data ptr
				 ********************************************************************************/
				c_block_ptr data() const 
				{
					if (raw_.empty())
						return nullptr;
					return raw_.data() + rpos_;
				}

				/*********************************************************************************
				 * Get data size
				 ********************************************************************************/
				uint32 data_size() const
				{
					return (uint32)raw_.size() - rpos_;
				}

			private:
				std::string raw_;

				uint32 rpos_;
			};
			DEFINE_ALL_POINTER_TYPE(buffer);

		}
	}
}

#endif