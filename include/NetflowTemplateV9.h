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

#ifndef NETFLOWENGINE_NETFLOWTEMPLATE_H
#define NETFLOWENGINE_NETFLOWTEMPLATE_H

#include <vector>
#include <memory>
#include "NetflowConst.h"

namespace nfc {
    class NetflowTemplateV9 {
    public:
        NetflowTemplateV9(uint8_t srcIpVersion, uint8_t nfVersion, uint16_t templateId, __int128_t sourceIp,
                        uint32_t sourceId);

        virtual ~NetflowTemplateV9();
        NetflowTemplateV9(const NetflowTemplateV9 &netflowTemplateV9);
        uint32_t getTemplateKey() const;
        static uint32_t makeTemplateKey(uint8_t srcIpVersion, uint8_t nfVersion, uint16_t templateId);
        void setTemplateItem(const nf9_template_item *templateItem, int size);

        nf9_template_item *getTemplateItem() const;
        int getTemplateSize() const;

        static const char *getFiledTypeV9Def(int type);
    private:
        uint8_t src_ip_version_ = 4;
        uint8_t nf_version_;
        uint16_t template_id_;

        __int128_t source_ip_;
        uint32_t source_id_;
        uint32_t epoch_;

        nf9_template_item *template_item_;
        int template_size_;

        static const char *filed_type_v9_def[80];
    };

} // nfc

#endif //NETFLOWENGINE_NETFLOWTEMPLATE_H
