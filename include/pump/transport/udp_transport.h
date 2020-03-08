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

#ifndef pump_transport_udp_transport_h
#define pump_transport_udp_transport_h

#include "pump/transport/flow/flow_udp.h"
#include "pump/transport/transport_notifier.h"

namespace pump {
	namespace transport {

		class udp_transport;
		DEFINE_ALL_POINTER_TYPE(udp_transport);

		class LIB_EXPORT udp_transport :
			public transport_base,
			public std::enable_shared_from_this<udp_transport>
		{
		public:
			/*********************************************************************************
			 * Create instance
			 ********************************************************************************/
			static udp_transport_sptr create_instance()
			{
				udp_transport_sptr ins(new udp_transport);
				return ins;
			}

			/*********************************************************************************
			 * Deconstructor
			 ********************************************************************************/
			virtual ~udp_transport();

			/*********************************************************************************
			 * Start
			 ********************************************************************************/
			bool start(
				service_ptr sv,
				const address &bind_address,
				transport_io_notifier_sptr &io_notifier,
				transport_terminated_notifier_sptr &terminated_notifier
			);

			/*********************************************************************************
			 * Stop 
			 ********************************************************************************/
			virtual void stop();

			/*********************************************************************************
			 * Send
			 ********************************************************************************/
			bool send(c_block_ptr b, uint32 size, const address &remote_address);

		protected:
			/*********************************************************************************
			 * Read event callback
			 ********************************************************************************/
			virtual void on_read_event(net::iocp_task_ptr itask);

			/*********************************************************************************
			 * Tracker event callback
			 ********************************************************************************/
			virtual void on_tracker_event(bool on);

		private:
			/*********************************************************************************
			 * Constructor
			 ********************************************************************************/
			udp_transport();

			/*********************************************************************************
			 * Open flow
			 ********************************************************************************/
			bool __open_flow(const address &local_address);

			/*********************************************************************************
			 * Close flow
			 ********************************************************************************/
			void __close_flow();

			/*********************************************************************************
			 * Start tracker
			 ********************************************************************************/
			bool __start_tracker();

			/*********************************************************************************
			 * Stop tracker
			 ********************************************************************************/
			void __stop_tracker();

		private:
			// Bind address
			address bind_address_;

			// Channel tracker
			poll::channel_tracker_sptr tracker_;

			// Udp flow
			flow::flow_udp_sptr flow_;

			// Transport terminated notifier
			transport_terminated_notifier_wptr terminated_notifier_;
		};

	}
}

#endif