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

#ifndef pump_protocol_http_utils_h
#define pump_protocol_http_utils_h

#include "pump/utils.h"
#include "pump/transport/address.h"

namespace pump {
	namespace protocol {
		namespace http {

			/*********************************************************************************
			 * Find http line end position
			 ********************************************************************************/
			LIB_PUMP c_block_ptr find_http_line_end(c_block_ptr src, int32 len);

			/*********************************************************************************
			 * Decode url string
			 ********************************************************************************/
			LIB_PUMP bool url_decode(const std::string &src, std::string &des);

			/*********************************************************************************
			 * Encode to url string
			 ********************************************************************************/
			LIB_PUMP bool url_encode(const std::string &src, std::string &des);

			/*********************************************************************************
			 * Host to address
			 ********************************************************************************/
			LIB_PUMP transport::address host_to_address(
				bool https,
				const std::string &host
			);

		}
	}
}

#endif