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

#ifndef pump_transport_flow_tcp_h
#define pump_transport_flow_tcp_h

#include "pump/transport/flow/flow.h"

namespace pump {
	namespace transport {
		namespace flow {

			class flow_tcp: public flow_base
			{
			public:
				/*********************************************************************************
				 * Constructor
				 ********************************************************************************/
				flow_tcp();

				/*********************************************************************************
				 * Deconstructor
				 ********************************************************************************/
				virtual ~flow_tcp();

				/*********************************************************************************
				 * Init
				 * Return results:
				 *     FLOW_ERR_NO    => success
				 *     FLOW_ERR_ABORT => error
				 ********************************************************************************/
				int32 init(poll::channel_sptr &ch, int32 fd);

				/*********************************************************************************
				 * Want to recv
				 * If using iocp this post an iocp task for receiving, else do nothing.
				 * Return results:
				 *     FLOW_ERR_NO    => success
				 *     FLOW_ERR_ABORT => error
				 ********************************************************************************/
				int32 want_to_recv();

				/*********************************************************************************
				 * Recv
				 ********************************************************************************/
				c_block_ptr recv(net::iocp_task_ptr itask, int32_ptr size);

				/*********************************************************************************
				 * Want to send
				 * If using iocp this post an iocp task for sending, else this try sending data.
				 * Return results:
				 *     FLOW_ERR_NO    => success
				 *     FLOW_ERR_ABORT => error
				 ********************************************************************************/
				int32 want_to_send(buffer_ptr sb);

				/*********************************************************************************
				 * Send
				 * Return results:
				 *     FLOW_ERR_NO      => send completely
				 *     FLOW_ERR_AGAIN   => try to send again
				 *     FLOW_ERR_NO_DATA => no data to send
				 *     FLOW_ERR_ABORT   => error
				 ********************************************************************************/
				int32 send(net::iocp_task_ptr itask);

				/*********************************************************************************
				 * Check there are data to send or not
				 ********************************************************************************/
				bool has_data_to_send()
				{
					if (send_buffer_ == nullptr || 
						send_buffer_->data_size() == 0)
						return false;
					return true;
				}

			private:
				// Recv iocp task
				net::iocp_task_ptr recv_task_;
				// Send iocp task
				net::iocp_task_ptr send_task_;
				// Recv cache
				std::string recv_cache_;
				// Send buffer
				buffer_ptr send_buffer_;
			};
			DEFINE_ALL_POINTER_TYPE(flow_tcp);

		}
	}
}

#endif