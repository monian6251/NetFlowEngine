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

#include <cstring>
#include "NetflowTemplateV9.h"

namespace nfc {
    const char * NetflowTemplateV9::filed_type_v9_def[80] = {
            "Undefined",
            "IN_BYTES",
            "IN_PKTS",
            "FLOWS",
            "PROTOCOL",
            "TOS",
            "TCP_FLAGS",
            "L4_SRC_PORT",
            "IPV4_SRC_ADDR",
            "SRC_MASK",
            "INPUT_SNMP",
            "L4_DST_PORT",
            "IPV4_DST_ADDR",
            "DST_MASK",
            "OUTPUT_SNMP",
            "IPV4_NEXT_HOP",
            "SRC_AS",
            "DST_AS",
            "BGP_IPV4_NEXT_HOP",
            "MUL_DST_PKTS",
            "MUL_DST_BYTES",
            "LAST_SWITCHED",
            "FIRST_SWITCHED",
            "OUT_BYTES",
            "OUT_PKTS",
            "Undefined",
            "Undefined",
            "IPV6_SRC_ADDR",
            "IPV6_DST_ADDR",
            "IPV6_SRC_MASK",
            "IPV6_DST_MASK",
            "IPV6_FLOW_LABEL",
            "ICMP_TYPE",
            "MUL_IGMP_TYPE",
            "SAMPLING_INTERVAL",
            "SAMPLING_ALGORITHM",
            "FLOW_ACTIVE_TIMEOUT",
            "FLOW_INACTIVE_TIMEOUT",
            "ENGINE_TYPE",
            "ENGINE_ID",
            "TOTAL_BYTES_EXP",
            "TOTAL_PKTS_EXP",
            "TOTAL_FLOWS_EXP",
            "Undefined",
            "Undefined",
            "Undefined",
            "MPLS_TOP_LABEL_TYPE",
            "MPLS_TOP_LABEL_IP_ADDR",
            "FLOW_SAMPLER_ID",
            "FLOW_SAMPLER_MODE",
            "FLOW_SAMPLER_RANDOM_INTERVAL",
            "Undefined",
            "Undefined",
            "Undefined",
            "Undefined",
            "DST_TOS",
            "SRC_MAC",
            "DST_MAC",
            "SRC_VLAN",
            "DST_VLAN",
            "IP_PROTOCOL_VERSION",
            "DIRECTION",
            "IPV6_NEXT_HOP",
            "BGP_IPV6_NEXT_HOP",
            "IPV6_OPTION_HEADERS",
            "Undefined",
            "Undefined",
            "Undefined",
            "Undefined",
            "Undefined",
            "MPLS_LABEL_1",
            "MPLS_LABEL_2",
            "MPLS_LABEL_3",
            "MPLS_LABEL_4",
            "MPLS_LABEL_5",
            "MPLS_LABEL_6",
            "MPLS_LABEL_7",
            "MPLS_LABEL_8",
            "MPLS_LABEL_9",
            "MPLS_LABEL_10"
    };

    NetflowTemplateV9::NetflowTemplateV9(uint8_t srcIpVersion, uint8_t nfVersion, uint16_t templateId, __int128_t sourceIp,
                                     uint32_t sourceId) : src_ip_version_(srcIpVersion), nf_version_(nfVersion),
                                                          template_id_(templateId), source_ip_(sourceIp),
                                                          source_id_(sourceId) {
        epoch_ = time(nullptr);
        template_item_ = nullptr;
    }

    uint32_t NetflowTemplateV9::getTemplateKey() const {
        return makeTemplateKey(src_ip_version_, nf_version_, template_id_);
    }

    uint32_t NetflowTemplateV9::makeTemplateKey(uint8_t srcIpVersion, uint8_t nfVersion, uint16_t templateId) {
        uint32_t key = srcIpVersion;
        key << 8;
        key += nfVersion;
        key << 8;
        key += templateId;
        return key;
    }

    void NetflowTemplateV9::setTemplateItem(const nf9_template_item *templateItem, int size) {
        template_size_ = size;
        if (template_item_ != nullptr) {
            free(template_item_);
        }
        template_item_ = (nf9_template_item *) malloc(size);
        memcpy(template_item_, templateItem, size);
    }

    nf9_template_item *NetflowTemplateV9::getTemplateItem() const {
        return template_item_;
    }

    int NetflowTemplateV9::getTemplateSize() const {
        return template_size_;
    }

    const char *NetflowTemplateV9::getFiledTypeV9Def(int type) {
        if (type < 80 && type >=0) {
            return filed_type_v9_def[type];
        } else {
            return "Undefined";
        }
    }

    NetflowTemplateV9::~NetflowTemplateV9() {
        if (template_item_ != nullptr) {
            free(template_item_);
        }
    }

    NetflowTemplateV9::NetflowTemplateV9(const NetflowTemplateV9 &netflowTemplateV9) {
        src_ip_version_ = netflowTemplateV9.src_ip_version_;
        nf_version_ = netflowTemplateV9.nf_version_;
        template_id_ = netflowTemplateV9.template_id_;

        source_ip_ = netflowTemplateV9.source_ip_;
        source_id_ = netflowTemplateV9.source_id_;
        epoch_ = netflowTemplateV9.epoch_;
        template_size_ = netflowTemplateV9.template_size_;

        template_item_ = (nf9_template_item *) malloc(template_size_);
        memcpy(template_item_, netflowTemplateV9.template_item_, template_size_);
    }

} // nfc