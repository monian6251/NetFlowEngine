/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2023 monian6251
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

//
// Created by ubuntu on 2/17/23.
//

#include "Device.h"

namespace nfc {
    int Device::getUseIp() const {
        return use_ip;
    }

    void Device::setUseIp(int useIp) {
        use_ip = useIp;
    }

    int Device::getIpVer() const {
        return ip_ver;
    }

    void Device::setIpVer(int ipVer) {
        ip_ver = ipVer;
    }

    int Device::getUseId() const {
        return use_id;
    }

    void Device::setUseId(int useId) {
        use_id = useId;
    }

    uint32_t Device::getId() const {
        return id;
    }

    void Device::setId(uint32_t id) {
        Device::id = id;
    }

    int Device::getSamplingRate() const {
        return sampling_rate;
    }

    void Device::setSamplingRate(int samplingRate) {
        sampling_rate = samplingRate;
    }

    const std::string &Device::getIp() const {
        return ip;
    }

    void Device::setIp(const std::string &ip) {
        Device::ip = ip;
    }

} // nfc