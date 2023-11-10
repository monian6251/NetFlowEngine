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

#include <cstring>
#include "NetflowTemplateIpfix.h"

namespace nfc {
    const char *NetflowTemplateIpfix::filed_type_ipfix_def[FILED_MAX_IPFIX] = {
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

    NetflowTemplateIpfix::NetflowTemplateIpfix(uint8_t srcIpVersion, uint8_t nfVersion, uint16_t templateId,
                                               xe_ip sourceIp, uint32_t sourceId) : src_ip_version_(srcIpVersion),
                                                                                    nf_version_(nfVersion),
                                                                                    template_id_(templateId),
                                                                                    source_ip_(sourceIp),
                                                                                    source_id_(sourceId) {
        epoch_ = time(nullptr);
        template_item_ = nullptr;
    }

    NetflowTemplateIpfix::~NetflowTemplateIpfix() {
        if (template_item_ != nullptr) {
            free(template_item_);
        }
    }

    template_key NetflowTemplateIpfix::getTemplateKey() {
        template_key tkey{};
        tkey.src_ip_version = 4;
        tkey.nf_version = nf_version_;
        tkey.template_id = template_id_;
        tkey.source_ip = source_ip_;
        tkey.source_id = source_id_;
        tkey.epoch = epoch_;

        return tkey;
    }

    void NetflowTemplateIpfix::makeTemplateKey(template_key *tkey, uint16_t template_id,
                                               nf_packet_info *npi, uint8_t version) {
        xe_ip addr;
        /* currently we support only IPv4 */
        tkey->src_ip_version = 4;

        tkey->nf_version = version;
        tkey->template_id = template_id;

        addr = npi->src_addr_ipv4;
        memcpy(&tkey->source_ip, &addr, sizeof(xe_ip));

        tkey->source_id = npi->source_id;
        /*tkey->epoch = npi->epoch;*/
        tkey->epoch = time(NULL);
    }

    void NetflowTemplateIpfix::setTemplateItem(const ipfix_stored_template *templateItem, int size) {
        template_size_ = size;
        if (template_item_ != nullptr) {
            free(template_item_);
        }
        template_item_ = (ipfix_stored_template *) malloc(size);
        memcpy(template_item_, templateItem, size);
    }

    ipfix_stored_template *NetflowTemplateIpfix::getTemplateItem() const {
        return template_item_;
    }


    int NetflowTemplateIpfix::getTemplateSize() const {
        return template_size_;
    }

    const char *NetflowTemplateIpfix::getFiledTypeIpfixDef(int type) {
        if (type < 80 && type >= 0) {
            return filed_type_ipfix_def[type];
        } else {
            return "Undefined";
        }
    }

    NetflowTemplateIpfix::NetflowTemplateIpfix(const NetflowTemplateIpfix &netflowTemplateIpfix) {
        src_ip_version_ = netflowTemplateIpfix.src_ip_version_;
        nf_version_ = netflowTemplateIpfix.nf_version_;
        template_id_ = netflowTemplateIpfix.template_id_;

        source_ip_ = netflowTemplateIpfix.source_ip_;
        source_id_ = netflowTemplateIpfix.source_id_;
        epoch_ = netflowTemplateIpfix.epoch_;
        template_size_ = netflowTemplateIpfix.template_size_;

        template_item_ = (ipfix_stored_template *) malloc(template_size_);
        memcpy(template_item_, netflowTemplateIpfix.template_item_, template_size_);
    }

    void NetflowTemplateIpfix::ipfixTemplateConvert(ipfix_stored_template *tmpl, uint8_t **ptr,
                                                    unsigned int field_count) {

        /* copy template header */
        memcpy(tmpl, *ptr, sizeof(ipfix_template_header));

        /* skip header, seek to data */
        *ptr += sizeof(ipfix_template_header);

        for (int i = 0; i < field_count; i++) {
            ipfix_inf_element_enterprise *ent;

            ent = (ipfix_inf_element_enterprise *) (*ptr);
            if ((ntohs(ent->id) >> 15) & 1) {
                /* enterprise */
                tmpl->elements[i].id = ent->id;
                tmpl->elements[i].length = ent->length;
                tmpl->elements[i].number = ent->number;
                *ptr += sizeof(ipfix_inf_element_enterprise);
            } else {
                /* iana */
                tmpl->elements[i].id = ent->id;
                tmpl->elements[i].length = ent->length;
                tmpl->elements[i].number = 0;
                *ptr += sizeof(ipfix_inf_element_iana);
            }
        }
    }
} // nfc