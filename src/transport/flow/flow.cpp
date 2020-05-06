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

#include "pump/transport/flow/flow.h"

namespace pump {
	namespace transport {
		namespace flow {

			flow_base::flow_base() PUMP_NOEXCEPT :
				fd_(-1),
				ext_(nullptr)
			{
			}

			int32 flow_base::unbind_fd()
			{
				int32 fd = fd_; 
				fd_ = -1;
				return fd;
			}

			void flow_base::close()
			{
				if (ext_)
				{
					net::delete_net_extension(ext_);
					ext_ = nullptr;
				}
				
				if (fd_ > 0)
				{
					net::close(fd_);
					fd_ = -1;
				}
			}

		}
	}
}