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

#ifndef ANDROID_VOLD_DISK_H
#define ANDROID_VOLD_DISK_H

#include "Utils.h"
#include "VolumeBase.h"

#include <utils/Errors.h>

#include <vector>

namespace android {
namespace vold {

class VolumeBase;

/*
 * Representation of detected physical media.
 *
 * Knows how to create volumes based on the partition tables found, and also
 * how to repartition itself.
 */
class Disk {
public:
    /* SPRD: modify for physical internal SD @{
     * @orig
    Disk(const std::string& eventPath, dev_t device, const std::string& nickname, int flags);
     */
    Disk(const std::string& eventPath, dev_t device, const std::string& nickname, const std::string& partname, int flags);
    /* @} */
    virtual ~Disk();

    enum Flags {
        /* Flag that disk is adoptable */
        kAdoptable = 1 << 0,
        /* Flag that disk is considered primary when the user hasn't
         * explicitly picked a primary storage location */
        kDefaultPrimary = 1 << 1,
        /* Flag that disk is SD card */
        kSd = 1 << 2,
        /* Flag that disk is USB disk */
        kUsb = 1 << 3,
        /* Flag that disk is EMMC internal */
        kEmmc = 1 << 4,
    };

    const std::string& getId() { return mId; }
    const std::string& getEventPath() { return mEventPath; }
    const std::string& getSysPath() { return mSysPath; }
    const std::string& getDevPath() { return mDevPath; }
    /* SPRD: add for UMS @{ */
    const std::string& getPartname() { return mPartname; }
    /* @} */
    dev_t getDevice() { return mDevice; }
    uint64_t getSize() { return mSize; }
    const std::string& getLabel() { return mLabel; }
    int getFlags() { return mFlags; }

    std::shared_ptr<VolumeBase> findVolume(const std::string& id);

    void listVolumes(VolumeBase::Type type, std::list<std::string>& list);

    status_t create();
    status_t destroy();

    status_t readMetadata();
    status_t readPartitions();

    status_t unmountAll();

    status_t partitionPublic();
    status_t partitionPrivate();
    status_t partitionMixed(int8_t ratio);

    void notifyEvent(int msg);
    void notifyEvent(int msg, const std::string& value);

private:
    /* ID that uniquely references this disk */
    std::string mId;
    /* Original event path */
    std::string mEventPath;
    /* Device path under sysfs */
    std::string mSysPath;
    /* Device path under dev */
    std::string mDevPath;
    /* Kernel device representing disk */
    dev_t mDevice;
    /* Size of disk, in bytes */
    uint64_t mSize;
    /* User-visible label, such as manufacturer */
    std::string mLabel;
    /* Current partitions on disk */
    std::vector<std::shared_ptr<VolumeBase>> mVolumes;
    /* Nickname for this disk */
    std::string mNickname;
    /* Flags applicable to this disk */
    int mFlags;
    /* Flag indicating object is created */
    bool mCreated;
    /* Flag that we just partitioned and should format all volumes */
    bool mJustPartitioned;

    void createPublicVolume(dev_t device);
    void createPrivateVolume(dev_t device, const std::string& partGuid);

    void destroyAllVolumes();

    int getMaxMinors();

    /* SPRD: add for physical internal SD @{ */
    /* we only mount one partition on this disk,
     * which partition name is mPartname
     */
    std::string mPartname;
    /* record the partVol used for formatting */
    std::shared_ptr<VolumeBase> mThePartVol;
    /* just used for format the part */
    status_t formatThePart();
    /* @} */

    /* SPRD: add for set link name @{ */
    status_t setVolLinkName(int partIndex, const std::shared_ptr<VolumeBase> vol);
    /* @} */

    /* SPRD: add for usb otg @{ */
    // disk check thread id
    pthread_t mDiskCheckThreadID;
    // check device node
    bool mCtlStopCheckThread;

    /* check thread run function */
    static void *diskCheck(void *arg);
    /* start check disk thread */
    void startDiskCheckThread();
    /* stop check disk thread */
    void stopDiskCheckThread();
    /* @} */
    DISALLOW_COPY_AND_ASSIGN(Disk);
};

}  // namespace vold
}  // namespace android

#endif
