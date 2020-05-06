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

#ifndef pump_transport_tcp_dialer_h
#define pump_transport_tcp_dialer_h

#include "pump/time/timer.h"
#include "pump/transport/base_dialer.h"
#include "pump/transport/tcp_transport.h"
#include "pump/transport/flow/flow_tcp_dialer.h"

namespace pump {
	namespace transport {

		class tcp_dialer;
		DEFINE_ALL_POINTER_TYPE(tcp_dialer);

		class LIB_PUMP tcp_dialer : 
			public base_dialer,
			public std::enable_shared_from_this<tcp_dialer>
		{
		public:
			/*********************************************************************************
			 * Create instance
			 ********************************************************************************/
			PUMP_INLINE PUMP_STATIC tcp_dialer_sptr create_instance(
				PUMP_CONST address &local_address,
				PUMP_CONST address &remote_address,
				int64 connect_timeout = 0
			) {
				return tcp_dialer_sptr(new tcp_dialer(
					local_address, 
					remote_address, 
					connect_timeout
				));
			}

			/*********************************************************************************
			 * Deconstructor
			 ********************************************************************************/
			virtual ~tcp_dialer() = default;

			/*********************************************************************************
			 * Start
			 ********************************************************************************/
			virtual bool start(service_ptr sv, PUMP_CONST dialer_callbacks &cbs) override;

			/*********************************************************************************
			 * Stop
			 ********************************************************************************/
			virtual void stop() override;

		protected:
			/*********************************************************************************
			 * Send event callback
			 ********************************************************************************/
			virtual void on_send_event(net::iocp_task_ptr itask) override;

		protected:
			/*********************************************************************************
			 * Timeout event callback
			 ********************************************************************************/
			PUMP_STATIC void on_timeout(tcp_dialer_wptr wptr);

		private:
			/*********************************************************************************
			 * Open flow
			 ********************************************************************************/
			bool __open_flow();

			/*********************************************************************************
			 * Close flow
			 ********************************************************************************/
			PUMP_INLINE void __close_flow()
			{ if (flow_) flow_->close(); }

		private:
			/*********************************************************************************
			 * Constructor
			 ********************************************************************************/
			tcp_dialer(
				PUMP_CONST address &local_address,
				PUMP_CONST address &remote_address,
				int64 connect_timeout
			) PUMP_NOEXCEPT;

		private:
			// Dialer flow
			flow::flow_tcp_dialer_sptr flow_;
		};

		class tcp_sync_dialer;
		DEFINE_ALL_POINTER_TYPE(tcp_sync_dialer);

		class LIB_PUMP tcp_sync_dialer : 
			public std::enable_shared_from_this<tcp_sync_dialer>
		{
		public:
			/*********************************************************************************
			 * Create instance
			 ********************************************************************************/
			PUMP_STATIC tcp_sync_dialer_sptr create_instance()
			{
				return tcp_sync_dialer_sptr(new tcp_sync_dialer);
			}

			/*********************************************************************************
			 * Deconstructor
			 ********************************************************************************/
			virtual ~tcp_sync_dialer() = default;

			/*********************************************************************************
			 * Dial by sync
			 ********************************************************************************/
			base_transport_sptr dial(
				service_ptr sv,
				PUMP_CONST address &local_address,
				PUMP_CONST address &remote_address,
				int64 connect_timeout = 0
			);

		protected:
			/*********************************************************************************
			 * Dialed event callback
			 ********************************************************************************/
			static void on_dialed_callback(
				tcp_sync_dialer_wptr wptr,
				base_transport_sptr transp,
				bool succ
			);

			/*********************************************************************************
			 * Dialed timeout event callback
			 ********************************************************************************/
			PUMP_STATIC void on_timeout_callback(tcp_sync_dialer_wptr wptr);

			/*********************************************************************************
			 * Stopped dial event callback
			 ********************************************************************************/
			PUMP_STATIC void on_stopped_callback();

		private:
			/*********************************************************************************
			 * Constructor
			 ********************************************************************************/
			tcp_sync_dialer() PUMP_NOEXCEPT
			{}

			/*********************************************************************************
			 * Reset sync dialer
			 ********************************************************************************/
			PUMP_INLINE void __reset()
			{ dialer_.reset(); }

		private:
			// Tcp dialer
			tcp_dialer_sptr dialer_;
			// Dial promise
			std::promise<base_transport_sptr> dial_promise_;
		};

	}
}

#endif
