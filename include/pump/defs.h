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

#ifndef pump_defs_h
#define pump_defs_h

#if defined(WIN32) && defined(pump_EXPORTS)
#	define LIB_EXPORT __declspec(dllexport)
#elif defined(WIN32)
#	define LIB_EXPORT __declspec(dllimport)
#else
#	define LIB_EXPORT 
#endif

#ifdef WIN32
#	define LIB_FORCEINLINE __forceinline
#else
#	define LIB_FORCEINLINE __inline__ __attribute__((always_inline))
#endif

#if 1
#	define PUMP_ASSERT(x) assert(x)
#else
#	define PUMP_ASSERT(x) (void)0
#endif

#define PUMP_ASSERT_EXPR(x, expr) PUMP_ASSERT(x); expr

#ifdef WIN32
#	define snprintf sprintf_s
#	define strncpy strcpy_s
#	define strncasecmp _strnicmp
#endif

#endif