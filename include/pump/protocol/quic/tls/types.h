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
 
#ifndef pump_protocol_quic_tls_types_h
#define pump_protocol_quic_tls_types_h

#include <string>

#include "pump/types.h"

// TLS verison.
typedef uint16_t tls_version_type;
#define TLS_VERSION_10 0x0301
#define TLS_VSERVER_11 0x0302
#define TLS_VSERVER_12 0x0303
#define TLS_VSERVER_13 0x0304

// TLS handshake message types.
typedef uint8_t tls_message_type;
#define TLS_MSG_HELLO_REQUEST        0
#define TLS_MSG_CLIENT_HELLO         1
#define TLS_MSG_SERVER_HELLO         2
#define TLS_MSG_NEW_SESSION_TICKET   4
#define TLS_MSG_END_OF_EARLY_DATA    5
#define TLS_MSG_ENCRYPTED_EXTENSIONS 8
#define TLS_MSG_CERTIFICATE          11
#define TLS_MSG_SERVER_KEY_EXCHANGE  12
#define TLS_MSG_CERTIFICATE_REQUEST  13
#define TLS_MSG_SERVER_HELLO_DONE    14
#define TLS_MSG_CERIFICATE_VERIFY    15
#define TLS_MSG_CLIENT_KEY_EXCHANGE  16
#define TLS_MSG_FINISHED             20
#define TLS_MSG_CERTIFICATE_STATUS   22
#define TLS_MSG_KEY_UPDATE           24
#define TLS_NSG_NEXT_PROTOCOL        67  // Not IANA assigned
#define TLS_MSG_MESSAGE_HASH         254 // synthetic message

// TLS compression types.
typedef uint8_t tls_compression_method_type;
#define TLS_COMPRESSION_METHOD_NONE 0

// TLS Elliptic Curve Point Formats
// https://tools.ietf.org/html/rfc4492#section-5.1.2
typedef uint8_t tls_point_format_type;
#define TLS_POINT_FORMAT_UNCOMPRESSED 0

// TLS cipher suites
// A list of cipher suite IDs that are, or have been, implemented by this
// package.
// https://www.iana.org/assignments/tls-parameters/tls-parameters.xml
typedef uint16_t tls_cipher_suite_type;
// TLS 1.0 - 1.2 cipher suites.
#define TLS_RSA_WITH_RC4_128_SHA                       0x0005
#define TLS_RSA_WITH_3DES_EDE_CBC_SHA                  0x000a
#define TLS_RSA_WITH_AES_128_CBC_SHA                   0x002f
#define TLS_RSA_WITH_AES_256_CBC_SHA                   0x0035
#define TLS_RSA_WITH_AES_128_CBC_SHA256                0x003c
#define TLS_RSA_WITH_AES_128_GCM_SHA256                0x009c
#define TLS_RSA_WITH_AES_256_GCM_SHA384                0x009d
#define TLS_ECDHE_ECDSA_WITH_RC4_128_SHA               0xc007
#define TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA           0xc009
#define TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA           0xc00a
#define TLS_ECDHE_RSA_WITH_RC4_128_SHA                 0xc011
#define TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA            0xc012
#define TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA             0xc013
#define TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA             0xc014
#define TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256        0xc023
#define TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256          0xc027
#define TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256          0xc02f
#define TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256        0xc02b
#define TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384          0xc030
#define TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384        0xc02c
#define TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256    0xcca8
#define TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256  0xcca9
// TLS 1.3 cipher suites.
#define TLS_AES_128_GCM_SHA256                         0x1301
#define TLS_AES_256_GCM_SHA384                         0x1302
#define TLS_CHACHA20_POLY1305_SHA256                   0x1303
// TLS_FALLBACK_SCSV isn't a standard cipher suite but an indicator
// that the client is doing version fallback. See RFC 7507.
#define TLS_FALLBACK_SCSV                              0x5600
// Legacy names for the corresponding cipher suites with the correct _SHA256
// suffix, retained for backward compatibility.
#define TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305           TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256
#define TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305         TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256


// TLS Certificate Status Type
// https://tools.ietf.org/html/rfc3546
typedef uint8_t tls_certicate_status_type;
#define TLS_OCSP_STATUS 1

// TLS curve group types.
typedef uint16_t tls_group_type;
#define TLS_GROUP_P256   23
#define TLS_GROUP_P384   24
#define TLS_GROUP_P2521  25
#define TLS_GROUP_X25519 29

// TLS signature scheme types
typedef uint16_t tls_signature_scheme_type;
// RSASSA-PKCS1-v1_5 algorithms.
#define TLS_SIGN_SCHEME_PKCS1WITHSHA256        0x0401
#define TLS_SIGN_SCHEME_PKCS1WITHSHA384        0x0501
#define TLS_SIGN_SCHEME_PKCS1WITHSHA512        0x0601
// RSASSA-PSS algorithms with public key OID rsaEncryption.
#define TLS_SIGN_SCHEME_PSSWITHSHA256          0x0804
#define TLS_SIGN_SCHEME_PSSWITHSHA384          0x0805
#define TLS_SIGN_SCHEME_PSSWITHSHA512          0x0806
// ECDSA algorithms. Only constrained to a specific curve in TLS 1.3.
#define TLS_SIGN_SCHEME_ECDSAWITHP256AndSHA256 0x0403
#define TLS_SIGN_SCHEME_ECDSAWITHP384AndSHA384 0x0503
#define TLS_SIGN_SCHEME_ECDSAWITHP521AndSHA512 0x0603
// EdDSA algorithms.
#define TLS_SIGN_SCHEME_ED25519                0x0807
// Legacy signature and hash algorithms for TLS 1.2.
#define TLS_SIGN_SCHEME_PKCS1WITHSHA1          0x0201
#define TLS_SIGN_SCHEME_ECDSAWITHSHA1          0x0203

// TLS 1.3 PSK Key Exchange Modes. See RFC 8446, Section 4.2.9.
typedef uint8_t tls_psk_mode_type;
#define TLS_PSK_MODE_PLAIN 0
#define TLS_PSK_MODE_DHE   1

// TLS extension types.
typedef uint16_t tls_extension_type;
#define TLS_EXTENSION_SERVER_NAME               0  // https://tools.ietf.org/html/rfc6066#section-3
#define TLS_EXTENSION_MAX_FRAGMENT_LENGTH       1  // https://tools.ietf.org/html/rfc6066
#define TLS_EXTENSION_STATUS_REQUEST            5  // https://tools.ietf.org/html/rfc4366#section-3.6
#define TLS_EXTENSION_SUPPORTED_GROUPS          10 // https://tools.ietf.org/html/rfc4492#section-5.1.1 https://tools.ietf.org/html/rfc8446#section-4.2.7
#define TLS_EXTENSION_SUPPORTED_POINTS          11 // https://tools.ietf.org/html/rfc4492#section-5.1.2
#define TLS_EXTENSION_SIGNATURE_ALGORITHMS      13 // https://tools.ietf.org/html/rfc5246#section-7.4.1.4.1 https://tools.ietf.org/html/rfc8446#section-4.2.3
#define TLS_EXTENSION_ALPN                      16 // https://tools.ietf.org/html/rfc7301#section-3.1
#define TLS_EXTENSION_SCT                       18 // https://tools.ietf.org/html/rfc6962#section-3.3.1
#define TLS_EXTENSION_SESSION_TICKET            35 // https://tools.ietf.org/html/rfc5077#section-3.2
#define TLS_EXTENSION_PRE_SHARED_KEY            41 // https://tools.ietf.org/html/rfc8446#section-4.2.11
#define TLS_EXTENSION_EARLY_DATA                42 // https://tools.ietf.org/html/rfc8446#section-4.2.10
#define TLS_EXTENSION_SUPPORTED_VERSIONS        43 // https://tools.ietf.org/html/rfc8446#section-4.2.1
#define TLS_EXTENSION_COOKIE                    44 // https://tools.ietf.org/html/rfc8446#section-4.2.2
#define TLS_EXTENSION_PSK_MODES                 45 // https://tools.ietf.org/html/rfc8446#section-4.2.9
#define TLS_EXTENSION_CERTIFICATE_AUTHORITIES   47 // https://tools.ietf.org/html/rfc8446
#define TLS_EXTENSION_SIGNATURE_ALGORITHMS_CERT 50 // https://tools.ietf.org/html/rfc8446#section-4.2.3
#define TLS_EXTENSION_KEY_SHARE                 51 // https://tools.ietf.org/html/rfc8446#section-4.2.8
#define TLS_EXTENSION_RENEGOTIATION_INFO        0xff01 // https://tools.ietf.org/html/rfc5746#section-3.2
#define TLS_EXTENSION_QUIC                      0xffa5

struct key_share {
    tls_group_type group;
    std::string data;
};

struct psk_identity {
    std::string identity;
    uint32_t obfuscated_ticket_age;
 
};

struct extension {
    uint16_t type;
    std::string data;
};

#endif