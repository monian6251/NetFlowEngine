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

#include <alloca.h>
#include <arpa/inet.h>

#include "NetflowConst.h"
#include "NetflowLog.h"
#include "NetflowPacket.h"

namespace nfc {
    NetflowPacket::NetflowPacket() {
        template_v9_ = std::make_shared<std::map<template_key, NetflowTemplateV9>>();
        template_ipfix_ = std::make_shared<std::map<template_key, NetflowTemplateIpfix>>();
        flow_ = std::make_shared<std::unordered_map<int, std::pair<int, std::shared_ptr<char>>>>();
    }

    NetflowPacket::~NetflowPacket() = default;

    int NetflowPacket::process_packet(nf_packet_info *npi, size_t len) {
        uint16_t *version_ptr;
        int version;
        timespec tmsp{};
        int ret = 0;

        /* get time for moving averages */
        if (clock_gettime(CLOCK_REALTIME_COARSE, &tmsp) < 0) {
            LOG(error) << "clock_gettime() failed: %s" << strerror(errno);
            return 0;
        }
        npi->time_ns = tmsp.tv_sec * 1e9 + tmsp.tv_nsec;

        version_ptr = (uint16_t *) npi->rawpacket;
        version = ntohs(*version_ptr);
        switch (version) {
            case 5:
                LOG(warning) << "Receive netflow v5 packet, len: " << len << (" .netflow v5 not supported yet");
                break;
            case 9:
                LOG(info) << "Receive netflow v9 packet, len: " << len;
                ret = parse_netflow_v9(npi, len);
                break;
            case 10:
                LOG(debug) << "Receive netflow ip fix packet, len: " << len;
                ret = parse_ipfix(npi, len);
                break;
            default:
                LOG(error) << "Unknown netflow version " << version;
                break;
        }
        return ret;
    }

    int NetflowPacket::parse_netflow_v9(nf_packet_info *npi, size_t len) {
        nf9_header *header;
        int flowset_id, flowset_id_host, length, count;
        uint8_t *ptr;

        header = (nf9_header *) npi->rawpacket;
        npi->source_id = header->source_id;
        npi->epoch = header->unix_secs;

        // device_get_sampling_rate(npi->src_addr_ipv4);

        ptr = (uint8_t *) npi->rawpacket + sizeof(nf9_header);

        while (ptr < ((uint8_t *) npi->rawpacket + len)) {
            nf9_flowset_header *flowset_header;

            flowset_header = (nf9_flowset_header *) ptr;

            flowset_id = flowset_header->flowset_id;
            flowset_id_host = ntohs(flowset_id);
            length = ntohs(flowset_header->length);
            count = ntohs(header->count);

            ptr += 4;

            if (flowset_id_host == 0) {
                if (!parse_netflow_v9_template(npi, &ptr, length)) {
                    /* something went wrong in template parser */
                    return 0;
                }
            } else if (flowset_id_host == 1) {
                LOG(info) << "options template";
                break;
            } else {
                if (!parse_netflow_v9_flowset(npi, &ptr, flowset_id, length, count)) {
                    break;
                }
            }
        }
        return 1;
    }

    int NetflowPacket::parse_netflow_v9_template(nf_packet_info *npi, uint8_t **ptr, int length) {
        nf9_template_item *ptmpl;
        uint16_t template_id, field_count;
        int template_size;

        ptmpl = (nf9_template_item *) (*ptr);
        template_id = ptmpl->template_id;
        field_count = ntohs(ptmpl->field_count);

        template_size = 4 + field_count * 4;
        if (template_size > length) {
            /* packet too short */
            LOG(error) << "Template is too short (size: " << template_size << ", packet length: " << length << ")";
            return 1;
        }

        LOG(info) << "Template id " << ntohs(template_id) << ", field count: " << field_count;

        /* search for template in map */
        NetflowTemplateV9 netflowTemplateV9(4, 9, template_id, npi->src_addr_ipv4, npi->source_id);
        template_key tk = netflowTemplateV9.getTemplateKey();
        if (template_v9_->find(tk) != template_v9_->end()) {
            NetflowTemplateV9 oldTemplate = template_v9_->at(tk);
            if (template_size != oldTemplate.getTemplateSize() ||
                memcmp(ptmpl, oldTemplate.getTemplateItem(), template_size) != 0) {
                oldTemplate.setTemplateItem(ptmpl, template_size);
            }
        } else {
            netflowTemplateV9.setTemplateItem(ptmpl, template_size);
            template_v9_->insert(std::make_pair(netflowTemplateV9.getTemplateKey(), netflowTemplateV9));
        }

        *ptr += template_size;

        return 0;
    }

    int NetflowPacket::parse_netflow_v9_flowset(nf_packet_info *npi, uint8_t **ptr, int flowset_id, int length,
                                                int count) {
        uint8_t *fptr;
        nf9_template_item *tmpl;
        template_key tkey{};
        int template_field_count;

        NetflowTemplateV9::makeTemplateKey(&tkey,flowset_id, npi, 9);
        auto tmp = template_v9_->find(tkey);
        if (tmp == template_v9_->end()) {
            LOG(warning) << "Unknown flowset id " << ntohs(flowset_id) << " .";
            return 1;
        }
        LOG(info) << "found one template " << ntohs(flowset_id);

        tmpl = tmp->second.getTemplateItem();
        template_field_count = ntohs(tmpl->field_count);
        fptr = (*ptr);
        flow_->clear();
        for (int cnt = 0; cnt < count; cnt++) {
            uint8_t *tmpfptr;
            tmpfptr = fptr;

            for (int i = 0; i < template_field_count; i++) {
                int flength, ftype;

                flength = ntohs(tmpl->typelen[i].length);
                ftype = ntohs(tmpl->typelen[i].type);
                std::shared_ptr<char> value(new char[flength]);
                memcpy(value.get(), fptr, flength);
                flow_->insert(std::make_pair(ftype, std::make_pair(flength, value)));

                fptr += flength;

                if ((fptr - (*ptr)) >= length) {
                    break;
                }
            }
            /* print flow or insert db */
            print_one_flow();
        }

        (*ptr) += length;
        return 0;
    }

    int NetflowPacket::parse_ipfix(nf_packet_info *npi, size_t len) {
        ipfix_header *header;
        int flowset_id, flowset_id_host, length;
        uint8_t *ptr;

        header = (ipfix_header *) npi->rawpacket;
        npi->source_id = header->observation_domain;
        npi->epoch = header->export_time;

        //sampling_rate_init(npi);
        LOG(debug) << "ipfix, package sequence: " << ntohl(header->sequence_number) << ", source id "
                   << ntohl(npi->source_id)
                   << ", length " << len << ".";

        ptr = (uint8_t *) npi->rawpacket + sizeof(ipfix_header);

        while (ptr < ((uint8_t *) npi->rawpacket + len)) {
            ipfix_flowset_header *flowset_header;

            flowset_header = (ipfix_flowset_header *) ptr;

            flowset_id = flowset_header->flowset_id;
            flowset_id_host = ntohs(flowset_id);
            length = ntohs(flowset_header->length);

            ptr += sizeof(ipfix_flowset_header);

            LOG(debug) << "Flowset " << flowset_id_host << ", length " << length;

            if (flowset_id_host == 2) {
                if (!parse_ipfix_template(npi, &ptr, length)) {
                    /* something went wrong in template parser */
                    break;
                }
            } else if (flowset_id_host == 3) {
                LOG(warning) << "options template ipfix, skipping";
            } else if (flowset_id_host > 255) {
                if (!parse_ipfix_flowset(npi, &ptr, flowset_id, length)) {

                    break;
                }
            } else {
                LOG(warning) << "unknown flowset id " << flowset_id_host;
            }

            ptr += length - sizeof(ipfix_flowset_header);
        }
        return 1;
    }

    int NetflowPacket::parse_ipfix_template(nf_packet_info *npi, uint8_t **ptr, int length) {
        ipfix_template_header *tmpl_header;
        ipfix_stored_template *tmpl_db, *tmpl;

        uint16_t template_id, field_count;
        template_key tkey;
        int template_size;

        tmpl_header = (ipfix_template_header *) (*ptr);
        template_id = tmpl_header->template_id;
        field_count = ntohs(tmpl_header->field_count);

        LOG(info) << "ipfix: template id " << ntohs(template_id) << ", field count: " << field_count << ".";

        /* search for template in database */
        if (field_count < 1) {
            LOG(warning) << "ipfix: incorrect field count " << field_count << ".";
            return 0;
        }
        template_size = sizeof(ipfix_template_header)
                        + sizeof(ipfix_inf_element_enterprise) * field_count;
        tmpl = (ipfix_stored_template *) alloca(template_size);

        NetflowTemplateIpfix::ipfixTemplateConvert(tmpl, ptr, field_count);

        NetflowTemplateIpfix netflowTemplateIpfix(4, 9, template_id, npi->src_addr_ipv4, npi->source_id);
        NetflowTemplateIpfix::makeTemplateKey(&tkey, template_id, npi, 10);
        if (template_ipfix_->find(tkey) != template_ipfix_->end()) {
            NetflowTemplateIpfix oldTemplate = template_ipfix_->at(tkey);
            if (template_size != oldTemplate.getTemplateSize() ||
                memcmp(tmpl, oldTemplate.getTemplateItem(), template_size) != 0) {
                oldTemplate.setTemplateItem(tmpl, template_size);
            }
        } else {
            netflowTemplateIpfix.setTemplateItem(tmpl, template_size);
            template_ipfix_->insert(std::make_pair(netflowTemplateIpfix.getTemplateKey(), netflowTemplateIpfix));
        }

        return 1;
    }

    int NetflowPacket::parse_ipfix_flowset(nf_packet_info *npi, uint8_t **ptr, int flowset_id, int length) {
        uint8_t *fptr;
        int i;
        ipfix_stored_template *tmpl;
        template_key tkey{};
        int template_field_count;
        int stop = 0;
        size_t t_id;

        LOG(debug) << "ipfix data, flowset: " << ntohs(flowset_id) << ", length = " << length;

        NetflowTemplateIpfix::makeTemplateKey(&tkey, flowset_id, npi, 10);
        auto temp = template_ipfix_->find(tkey);

        if (temp == template_ipfix_->end()) {
            LOG(warning) << "Unknown flowset id " << ntohs(flowset_id);
            return 0;
        }
        tmpl = temp->second.getTemplateItem();
        template_field_count = ntohs(tmpl->header.field_count);

        fptr = (*ptr);
        while (!stop) {
            uint8_t *tmpfptr;

            if ((length - (fptr - (*ptr))) < template_field_count) {
                break;
            }

            tmpfptr = fptr;

            for (i = 0; i < template_field_count; i++) {
                int flength, ftype;

                flength = ntohs(tmpl->elements[i].length);
                ftype = ntohs(tmpl->elements[i].id);
                std::shared_ptr<char> value(new char[flength]);
                memcpy(value.get(), fptr, flength);
                flow_->insert(std::make_pair(ftype, std::make_pair(flength, value)));

                fptr += flength;
                if ((fptr - (*ptr)) >= length) {
                    stop = 1;
                    break;
                }
            }
            /* print flow or insert db */
            print_one_flow();
        }
        return 1;
    }

    void NetflowPacket::setDevices(const std::shared_ptr<std::vector<Device>> &devices) {
        devices_ = devices;
    }

    int NetflowPacket::device_get_sampling_rate(uint32_t src_addr_ipv4) {
        char ipstr[16];
        in_addr s;
        s.s_addr = htonl(src_addr_ipv4);
        inet_ntop(AF_INET, (void *) &s, ipstr, (socklen_t) sizeof(ipstr));

        for (auto d: *devices_) {
            if (d.getIp() == std::string(ipstr)) {
                return d.getSamplingRate();
            }
        }

        LOG(warning) << "device not found " << ipstr;
        return 1000;
    }

    void NetflowPacket::print_one_flow() {
        LOG(debug) << "-----------------------------------------------";
        for (auto item: *flow_) {
            std::string p = NetflowTemplateV9::getFiledTypeV9Def(item.first);
            p.append(": ");
            for (int i = 0; i < item.second.first; i++) {
                char c[8];
                sprintf(c, "%02x", (unsigned char) item.second.second.get()[i]);
                p.append(c);
            }
            LOG(debug) << p;
        }
    }

} // nfc