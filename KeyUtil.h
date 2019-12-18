/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef ANDROID_VOLD_KEYUTIL_H
#define ANDROID_VOLD_KEYUTIL_H

#include "KeyBuffer.h"
#include "KeyStorage.h"

#include <memory>
#include <string>

namespace android {
namespace vold {

bool randomKey(KeyBuffer* key);

bool isFsKeyringSupported(void);

bool installKey(const KeyBuffer& key, const std::string& mountpoint, int policy_version,
                std::string* raw_ref);
bool evictKey(const std::string& mountpoint, const std::string& raw_ref, int policy_version);
bool retrieveAndInstallKey(bool create_if_absent, const KeyAuthentication& key_authentication,
                           const std::string& key_path, const std::string& tmp_path,
                           const std::string& volume_uuid, int policy_version,
                           std::string* key_ref);
bool retrieveKey(bool create_if_absent, const std::string& key_path, const std::string& tmp_path,
                 KeyBuffer* key, bool keepOld = true);

}  // namespace vold
}  // namespace android

#endif