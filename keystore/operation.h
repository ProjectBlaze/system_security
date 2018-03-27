/*
 * Copyright (C) 2015 The Android Open Source Project
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

#ifndef KEYSTORE_OPERATION_H_
#define KEYSTORE_OPERATION_H_

#include <map>
#include <vector>

#include <binder/Binder.h>
#include <binder/IBinder.h>
#include <keymasterV4_0/Keymaster.h>
#include <utils/StrongPointer.h>

#include <keystore/keymaster_types.h>

namespace keystore {

using ::android::IBinder;
using ::android::sp;
using keymaster::support::Keymaster;

/**
 * OperationMap handles the translation of uint64_t's and keymaster2_device_t's to opaque binder
 * tokens that can be used to reference that operation at a later time by applications. It also does
 * LRU tracking for operation pruning and keeps a mapping of clients to operations to allow for
 * graceful handling of application death.
 */

class OperationMap {
  public:
    struct Operation {
        Operation() = default;
        Operation(uint64_t handle, uint64_t keyid, KeyPurpose purpose, const sp<Keymaster>& device,
                  KeyCharacteristics&& characteristics, sp<IBinder> appToken);
        Operation(Operation&&) = default;
        Operation(const Operation&) = delete;

        bool hasAuthToken() const { return authToken.mac.size() != 0; }

        uint64_t handle;
        uint64_t keyid;
        KeyPurpose purpose;
        sp<Keymaster> device;
        KeyCharacteristics characteristics;
        sp<IBinder> appToken;
        HardwareAuthToken authToken;
    };

    explicit OperationMap(IBinder::DeathRecipient* deathRecipient);
    sp<IBinder> addOperation(uint64_t handle, uint64_t keyid, KeyPurpose purpose,
                             const sp<Keymaster>& dev, const sp<IBinder>& appToken,
                             KeyCharacteristics&& characteristics, bool pruneable);
    NullOr<const Operation&> getOperation(const sp<IBinder>& token);
    NullOr<Operation> removeOperation(const sp<IBinder>& token);
    bool hasPruneableOperation() const;
    size_t getOperationCount() const { return mMap.size(); }
    size_t getPruneableOperationCount() const;
    bool setOperationAuthToken(const sp<IBinder>& token, HardwareAuthToken authToken);
    sp<IBinder> getOldestPruneableOperation();
    std::vector<sp<IBinder>> getOperationsForToken(const sp<IBinder>& appToken);

  private:
    void updateLru(const sp<IBinder>& token);
    void removeOperationTracking(const sp<IBinder>& token, const sp<IBinder>& appToken);
    std::map<sp<IBinder>, Operation> mMap;
    std::vector<sp<IBinder>> mLru;
    std::map<sp<IBinder>, std::vector<sp<IBinder>>> mAppTokenMap;
    IBinder::DeathRecipient* mDeathRecipient;
};

}  // namespace keystore

#endif