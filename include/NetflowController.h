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
// Created by ubuntu on 2/16/23.
//

#ifndef NETFLOWENGINE_NETFLOWCONTROLLER_H
#define NETFLOWENGINE_NETFLOWCONTROLLER_H

#include "NetflowPacket.h"

namespace nfc {

    class NetflowController {
    public:
        explicit NetflowController(const char *filename, int port);

        virtual ~NetflowController();

        void run();

        bool isRunFlag() const;

        void setRunFlag(bool runFlag);

    private:
        int devices_load(const char *filename);

    private:
        int port_;
        bool run_flag_ = true;

        NetflowPacket netflow_packet_;
    };

} // nfc

#endif //NETFLOWENGINE_NETFLOWCONTROLLER_H
