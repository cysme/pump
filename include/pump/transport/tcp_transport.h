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

#ifndef pump_transport_tcp_transport_h
#define pump_transport_tcp_transport_h

#include "pump/toolkit/freelock.h"
#include "pump/transport/flow/flow_tcp.h"
#include "pump/transport/base_transport.h"

namespace pump {
namespace transport {

    class tcp_transport;
    DEFINE_ALL_POINTER_TYPE(tcp_transport);

    class LIB_PUMP tcp_transport : public base_transport,
                                   public std::enable_shared_from_this<tcp_transport> {
      public:
        /*********************************************************************************
         * Create instance
         ********************************************************************************/
        PUMP_INLINE static tcp_transport_sptr create_instance() {
            INLINE_OBJECT_CREATE(obj, tcp_transport, ());
            return tcp_transport_sptr(obj, object_delete<tcp_transport>);
        }

        /*********************************************************************************
         * Deconstructor
         ********************************************************************************/
        virtual ~tcp_transport();

        /*********************************************************************************
         * Init
         ********************************************************************************/
        void init(int32 fd, const address &local_address, const address &remote_address);

        /*********************************************************************************
         * Start
         ********************************************************************************/
        virtual transport_error start(service_ptr sv,
                                      int32 max_pending_send_size,
                                      const transport_callbacks &cbs) override;

        /*********************************************************************************
         * Stop
         ********************************************************************************/
        virtual void stop() override;

        /*********************************************************************************
         * Force stop
         ********************************************************************************/
        virtual void force_stop() override;

        /*********************************************************************************
         * Read for once
         ********************************************************************************/
        virtual transport_error read_for_once();

        /*********************************************************************************
         * Read for loop
         ********************************************************************************/
        virtual transport_error read_for_loop();

        /*********************************************************************************
         * Send
         ********************************************************************************/
        virtual transport_error send(c_block_ptr b, uint32 size) override;

      protected:
        /*********************************************************************************
         * Read event callback
         ********************************************************************************/
#if defined(PUMP_HAVE_IOCP)
        virtual void on_read_event(void_ptr iocp_task) override;
#else
        virtual void on_read_event() override;
#endif

        /*********************************************************************************
         * Send event callback
         ********************************************************************************/
#if defined(PUMP_HAVE_IOCP)
        virtual void on_send_event(void_ptr iocp_task) override;
#else
        virtual void on_send_event() override;
#endif

      private:
        /*********************************************************************************
         * Constructor
         ********************************************************************************/
        tcp_transport() noexcept;

        /*********************************************************************************
         * Open transport flow
         ********************************************************************************/
        bool __open_transport_flow();

        /*********************************************************************************
         * Shutdown transport flow
         ********************************************************************************/
        PUMP_INLINE void __shutdown_transport_flow() {
            if (flow_)
                flow_->shutdown();
        }

        /*********************************************************************************
         * Close transport flow
         ********************************************************************************/
        virtual void __close_transport_flow() override {
            if (flow_)
                flow_->close();
        }

        /*********************************************************************************
         * Async read
         ********************************************************************************/
        transport_error __async_read(uint32 state);

        /*********************************************************************************
         * Async send
         ********************************************************************************/
        bool __async_send(toolkit::io_buffer_ptr b);

        /*********************************************************************************
         * Send once
         ********************************************************************************/
        bool __send_once(flow::flow_tcp_ptr flow, bool resume);

        /*********************************************************************************
         * Try doing dissconnected process
         ********************************************************************************/
        void __try_doing_disconnected_process();

        /*********************************************************************************
         * Clear sendlist
         ********************************************************************************/
        void __clear_sendlist();

      private:
        // Transport flow
        flow::flow_tcp_sptr flow_;

        // Last send buffer
        volatile int32 last_send_iob_size_;
        volatile toolkit::io_buffer_ptr last_send_iob_;

        // When sending data, transport will append buffer to sendlist at first. On
        // triggering send event, transport will send buffer in the sendlist.
        //toolkit::freelock_list_queue<toolkit::io_buffer_ptr> sendlist_;
        toolkit::freelock_vector_queue<toolkit::io_buffer_ptr> sendlist_;

        // Who got next send chance, who can send next buffer.
        std::atomic_flag next_send_chance_;
    };

}  // namespace transport
}  // namespace pump

#endif
