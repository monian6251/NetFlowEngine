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
// Created by mo_nian@live.com on 3/3/23.
//

#ifndef NETFLOWENGINE_NETFLOWTEMPLATEIPFIX_H
#define NETFLOWENGINE_NETFLOWTEMPLATEIPFIX_H

#include <vector>
#include <memory>
#include "NetflowConst.h"

#define FILED_MAX_IPFIX (80)

namespace nfc {

    class NetflowTemplateIpfix {
    public:
        NetflowTemplateIpfix(uint8_t srcIpVersion, uint8_t nfVersion, uint16_t templateId, __int128_t sourceIp,
                             uint32_t sourceId);

        virtual ~NetflowTemplateIpfix();

        NetflowTemplateIpfix(const NetflowTemplateIpfix &netflowTemplateIpfix);

        template_key getTemplateKey();

        static void makeTemplateKey(template_key *tkey, uint16_t template_id,
                                                             nf_packet_info *npi, uint8_t version);

        void setTemplateItem(const ipfix_stored_template *templateItem, int size);

        ipfix_stored_template *getTemplateItem() const;

        int getTemplateSize() const;

        static const char *getFiledTypeIpfixDef(int type);

        static void ipfixTemplateConvert(ipfix_stored_template *tmpl, uint8_t **ptr,
                                         unsigned int field_count);

    private:
        uint8_t src_ip_version_ = 4;
        uint8_t nf_version_;
        uint16_t template_id_;

        xe_ip source_ip_;
        uint32_t source_id_;
        uint32_t epoch_;

        ipfix_stored_template *template_item_;
        int template_size_;

        static const char *filed_type_ipfix_def[FILED_MAX_IPFIX];
    };

} // nfc

#endif //NETFLOWENGINE_NETFLOWTEMPLATEIPFIX_H
