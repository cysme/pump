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

#include "pump/transport/flow/flow_tcp_acceptor.h"

namespace pump {
namespace transport {
    namespace flow {

        flow_tcp_acceptor::flow_tcp_acceptor() noexcept
          : is_ipv6_(false), 
            iob_(nullptr) {
#if defined(PUMP_HAVE_IOCP)
            accept_task_ = nullptr;
#endif
        }

        flow_tcp_acceptor::~flow_tcp_acceptor() {
#if defined(PUMP_HAVE_IOCP)
            if (accept_task_) {
                accept_task_->sub_link();
            }
#endif
            if (iob_) {
                iob_->sub_ref();
            }
        }

        int32_t flow_tcp_acceptor::init(poll::channel_sptr &&ch,
                                        const address &listen_address) {
            PUMP_DEBUG_ASSIGN(ch, ch_, ch);

            is_ipv6_ = listen_address.is_ipv6();
            int32_t domain = is_ipv6_ ? AF_INET6 : AF_INET;

            iob_ = toolkit::io_buffer::create();
            iob_->init_with_size(ADDRESS_MAX_LEN * 3);

#if defined(PUMP_HAVE_IOCP)
            fd_ = net::create_iocp_socket(domain, SOCK_STREAM, net::get_iocp_handler());
            if (fd_ == -1) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: init failed for creating iocp socket failed");
                return FLOW_ERR_ABORT;
            }

            extra_fns_ = net::new_iocp_extra_function(fd_);
            if (!extra_fns_) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: init failed for newing iocp function failed");
                return FLOW_ERR_ABORT;
            }

            accept_task_ = net::new_iocp_task();
            accept_task_->set_fd(fd_);
            accept_task_->set_notifier(ch_);
            accept_task_->set_type(net::IOCP_TASK_ACCEPT);
            accept_task_->bind_io_buffer(iob_);
#else
            fd_ = net::create_socket(domain, SOCK_STREAM);
            if (fd_ == -1) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: init failed for creating socket failed");
                return FLOW_ERR_ABORT;
            }
#endif
            if (!net::set_reuse(fd_, 1)) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: init failed for setting socket reuse failed");
                return FLOW_ERR_ABORT;
            }
            if (!net::set_noblock(fd_, 1)) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: init failed for setting socket noblock failed");
                return FLOW_ERR_ABORT;
            }
            if (!net::set_nodelay(fd_, 1)) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: init failed for setting socket nodelay failed");
                return FLOW_ERR_ABORT;
            }
            if (!net::bind(fd_, (sockaddr*)listen_address.get(), listen_address.len())) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: init failed for binding socket address failed");
                return FLOW_ERR_ABORT;
            }
            if (!net::listen(fd_)) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: init failed for listening failed");
                return FLOW_ERR_ABORT;
            }

            return FLOW_ERR_NO;
        }

#if defined(PUMP_HAVE_IOCP)
        int32_t flow_tcp_acceptor::post_accept() {
            int32_t domain = is_ipv6_ ? AF_INET6 : AF_INET;
            int32_t client = net::create_iocp_socket(domain, SOCK_STREAM, net::get_iocp_handler());
            if (client == -1) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: post accept task failed for creating iocp socket failed");
                return FLOW_ERR_ABORT;
            }

            accept_task_->set_client_fd(client);
            if (!net::post_iocp_accept(extra_fns_, accept_task_)) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: post accept failed for posting iocp accept task failed");
                net::close(client);
                return FLOW_ERR_ABORT;
            }

            return FLOW_ERR_NO;
        }

        int32_t flow_tcp_acceptor::accept(net::iocp_task_ptr iocp_task,
                                          address_ptr local_address,
                                          address_ptr remote_address) {
            int32_t client_fd = iocp_task->get_client_fd();
            if (iocp_task->get_errcode() != 0 || client_fd == -1) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: accept failed %d", iocp_task->get_errcode());
                net::close(client_fd);
                return -1;
            }

            sockaddr *local = nullptr;
            sockaddr *remote = nullptr;
            int32_t llen = sizeof(sockaddr_in);
            int32_t rlen = sizeof(sockaddr_in);
            if (!net::get_iocp_client_address(extra_fns_, iocp_task, &local, &llen, &remote, &rlen)) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: accept failed for getting iocp accepted address falied");
                net::close(client_fd);
                return -1;
            }
            local_address->set(local, llen);
            remote_address->set(remote, rlen);

            iocp_task->set_client_fd(0);
            if (!net::set_noblock(client_fd, 1) || 
                !net::set_nodelay(client_fd, 1)) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: accept failed for setting socket noblock or nodelay fialed");
                net::close(client_fd);
                return -1;
            }

            return client_fd;
        }
#else
        int32_t flow_tcp_acceptor::accept(address_ptr local_address,
                                        address_ptr remote_address) {
            int32_t addrlen = ADDRESS_MAX_LEN;
            int32_t client_fd = net::accept(fd_, (struct sockaddr*)iob_->buffer(), &addrlen);
            if (client_fd == -1) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: accept failed");
                return -1;
            }
            
            remote_address->set((sockaddr*)iob_->buffer(), addrlen);

            addrlen = ADDRESS_MAX_LEN;
            net::local_address(client_fd, (sockaddr*)iob_->buffer(), &addrlen);
            local_address->set((sockaddr*)iob_->buffer(), addrlen);

            if (!net::set_noblock(client_fd, 1) || 
                !net::set_nodelay(client_fd, 1)) {
                PUMP_DEBUG_LOG("flow_tcp_acceptor: accept failed for setting socket noblock or nodelay fialed");
                net::close(client_fd);
                return -1;
            }

            return client_fd;
        }
#endif

    }  // namespace flow
}  // namespace transport
}  // namespace pump
