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

#include "pump/protocol/http/request.h"
#include "pump/protocol/http/response.h"
#include "pump/protocol/http/connection.h"

namespace pump {
namespace protocol {
namespace http {

    connection::connection(bool server, transport::base_transport_sptr &transp) noexcept
      : incoming_pocket_(nullptr),
        transp_(transp) {
        if (server) {
            create_incoming_pocket_ = []() {
                return object_create<request>();
            };
        } else {
            create_incoming_pocket_ = []() {
                return object_create<response>();
            };
        }
    }

    connection::~connection() {
        if (transp_) {
            transp_->force_stop();
        }
    }

    bool connection::start(service_ptr sv, const http_callbacks &cbs) {
        if (!transp_) {
            return false;
        }

        if (!cbs.pocket_cb || !cbs.error_cb) {
            return false;
        }
        http_cbs_ = cbs;

        transport::transport_callbacks tcbs;
        connection_wptr wptr = shared_from_this();
        tcbs.read_cb = pump_bind(&connection::on_read, wptr, _1, _2);
        tcbs.stopped_cb = pump_bind(&connection::on_stopped, wptr);
        tcbs.disconnected_cb = pump_bind(&connection::on_disconnected, wptr);
        if (transp_->start(sv, tcbs) != transport::ERROR_OK) {
            return false;
        }

        return true;
    }

    void connection::stop() {
        __stop_transport();
    }

    bool connection::read_next_pocket() {
        PUMP_LOCK_SPOINTER(transp, transp_);
        if (!transp) {
            return false;
        }

        if (transp->read_for_once() != transport::ERROR_OK) {
            return false;
        }

        return true;
    }

    bool connection::send(c_pocket_ptr pk) {
        std::string data;
        pk->serialize(data);
        return transp_->send(data.c_str(), (int32_t)data.size()) == transport::ERROR_OK;
    }

    bool connection::send(c_body_ptr b) {
        std::string data;
        b->serialize(data);
        return transp_->send(data.c_str(), (int32_t)data.size()) == transport::ERROR_OK;
    }

    void connection::on_read(connection_wptr wptr, const block_t *b, int32_t size) {
        PUMP_LOCK_WPOINTER(conn, wptr);
        if (conn) {
            conn->__handle_http_data(b, size);
        }
    }

    void connection::on_disconnected(connection_wptr wptr) {
        PUMP_LOCK_WPOINTER(conn, wptr);
        if (conn) {
            conn->http_cbs_.error_cb("http connection disconnected");
        }
    }

    void connection::on_stopped(connection_wptr wptr) {
        PUMP_LOCK_WPOINTER(conn, wptr);
        if (conn) {
            conn->http_cbs_.error_cb("http connection stopped");
        }
    }

    void connection::__handle_http_data(const block_t *b, int32_t size) {
        auto pk = incoming_pocket_.get();
        if (!pk) {
            pk = create_incoming_pocket_();
            incoming_pocket_.reset(pk, object_delete<pocket>);
        }

        int32_t parse_size = -1;
        if (read_cache_.empty()) {
            parse_size = pk->parse(b, size);
            if (parse_size >= 0 && parse_size < size) {
                read_cache_.append(b + parse_size, uint32_t(size - parse_size));
            }
        } else {
            read_cache_.append(b, size);
            parse_size = pk->parse(read_cache_.data(), (int32_t)read_cache_.size());
            if (parse_size > 0)
                read_cache_ = read_cache_.substr(parse_size);
        }

        if (parse_size == -1) {
            __stop_transport();
            return;
        }

        if (pk->is_parse_finished()) {
            http_cbs_.pocket_cb(std::move(incoming_pocket_));
        } else {
            transp_->read_for_once();
        }
    }

}  // namespace http
}  // namespace protocol
}  // namespace pump
