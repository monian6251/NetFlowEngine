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

#ifndef NETFLOWENGINE_DEVICE_H
#define NETFLOWENGINE_DEVICE_H

#include <cstdint>
#include <string>

namespace nfc {

    class Device {
    private:
        int use_ip;
        int ip_ver;
        std::string ip;

        int use_id;
        uint32_t id;

        int sampling_rate;
    public:
        int getUseIp() const;

        void setUseIp(int useIp);

        int getIpVer() const;

        void setIpVer(int ipVer);

        int getUseId() const;

        void setUseId(int useId);

        uint32_t getId() const;

        void setId(uint32_t id);

        int getSamplingRate() const;

        void setSamplingRate(int samplingRate);

        const std::string &getIp() const;

        void setIp(const std::string &ip);
    };

} // nfc

#endif //NETFLOWENGINE_DEVICE_H
