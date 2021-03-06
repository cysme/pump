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

#include "pump/protocol/http/server.h"

namespace pump {
namespace protocol {
namespace http {

    server::server() noexcept
      : sv_(nullptr) {
    }

    server::~server() {
    }

    bool server::start(service_ptr sv,
                       const transport::address &listen_address,
                       const server_callbacks &cbs) {
        // Check acceptor
        if (acceptor_) {
            return false;
        }

        // Check service
        if (!sv) {
            return false;
        }
        sv_ = sv;

        // Check callbacks
        if (!cbs.request_cb || !cbs.stopped_cb) {
            return false;
        }
        cbs_ = cbs;

        transport::acceptor_callbacks acbs;
        server_wptr wptr = shared_from_this();
        acbs.stopped_cb = pump_bind(&server::on_stopped, wptr);
        acbs.accepted_cb = pump_bind(&server::on_accepted, wptr, _1);

        auto accepter = transport::tcp_acceptor::create(listen_address);
        if (accepter->start(sv, acbs) != transport::ERROR_OK) {
            return false;
        }
        acceptor_ = accepter;

        return true;
    }

    bool server::start(service_ptr sv,
                       const std::string &crtfile,
                       const std::string &keyfile,
                       const transport::address &listen_address,
                       const server_callbacks &cbs) {
        // Check acceptor
        if (acceptor_) {
            return false;
        }

        // Check service
        if (!sv) {
            return false;
        }
        sv_ = sv;

        // Check callbacks
        if (!cbs.request_cb || !cbs.stopped_cb) {
            return false;
        }
        cbs_ = cbs;

        transport::acceptor_callbacks acbs;
        server_wptr wptr = shared_from_this();
        acbs.stopped_cb = pump_bind(&server::on_stopped, wptr);
        acbs.accepted_cb = pump_bind(&server::on_accepted, wptr, _1);

        auto acceptor = 
            transport::tls_acceptor::create_with_file(
                            crtfile, keyfile, listen_address, 1000);
        if (acceptor->start(sv, acbs) != transport::ERROR_OK) {
            return false;
        }
        acceptor_ = acceptor;

        return true;
    }

    void server::stop() {
        if (acceptor_) {
            acceptor_->stop();
        }
    }

    void server::on_accepted(server_wptr wptr,
                             transport::base_transport_sptr &transp) {
        PUMP_LOCK_WPOINTER(svr, wptr);
        if (!svr) {
            return;
        }

        connection_sptr conn(new connection(true, transp));
        {
            std::unique_lock<std::mutex> lock(svr->conn_mx_);
            svr->conns_[conn.get()] = conn;
        }

        http_callbacks cbs;
        cbs.error_cb = pump_bind(&server::on_http_error, wptr, conn, _1);
        cbs.pocket_cb = pump_bind(&server::on_http_request, wptr, conn, _1);
        if (!conn->start(svr->sv_, cbs)) {
            std::unique_lock<std::mutex> lock(svr->conn_mx_);
            svr->conns_.erase(conn.get());
        }
        conn->read_next_pocket();
    }

    void server::on_stopped(server_wptr wptr) {
        PUMP_LOCK_WPOINTER(svr, wptr);
        if (!svr) {
            return;
        }

        std::unique_lock<std::mutex> lock(svr->conn_mx_);
        while (!svr->conns_.empty()) {
            auto beg = svr->conns_.begin();
            while (beg != svr->conns_.end()) {
                if (beg->second->is_valid()) {
                    (beg++)->second->stop();
                }
            }

            svr->conn_cond_.wait_for(lock, std::chrono::seconds(1));
        }

        svr->cbs_.stopped_cb();
    }

    void server::on_http_request(server_wptr wptr,
                                 connection_wptr wconn,
                                 pocket_sptr &&pk) {
        PUMP_LOCK_WPOINTER(svr, wptr);
        if (!svr) {
            return;
        }

        PUMP_LOCK_WPOINTER(conn, wconn);
        if (!conn) {
            return;
        }

        svr->cbs_.request_cb(wconn, std::static_pointer_cast<request>(pk));

        conn->read_next_pocket();
    }

    void server::on_http_error(server_wptr wptr,
                               connection_wptr wconn,
                               const std::string &msg) {
        PUMP_LOCK_WPOINTER(conn, wconn);
        PUMP_ASSERT(conn);

        PUMP_LOCK_WPOINTER(svr, wptr);
        if (!svr) {
            conn->stop();
            return;
        }

        std::unique_lock<std::mutex> w_lock(svr->conn_mx_);
        svr->conns_.erase(conn);

        if (!svr->acceptor_->is_started()) {
            svr->conn_cond_.notify_one();
        }
    }

}  // namespace http
}  // namespace protocol
}  // namespace pump