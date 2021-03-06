/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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
 *
 * Author: princejay.dai@rock-chips.com
 *   Date: 2019/11/28
 *   Task: print Rockit player version.
 */

#ifndef SRC_RT_PLAYER_RTVERSION_H_
#define SRC_RT_PLAYER_RTVERSION_H_

#include "rt_header.h"       // NOLINT
#include "rt_type.h"         // NOLINT

class RTVersion {
 public:
    static const char*    getVersionProject();
    static const char*    getVersionBuild();
    static void           versionInfo();
};

#endif  // SRC_RT_PLAYER_RTVERSION_H_
