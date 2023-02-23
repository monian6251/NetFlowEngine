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

#include <arpa/inet.h>

#include "NetflowConst.h"
#include "NetflowLog.h"
#include "NetflowPacket.h"

namespace nfc {
    NetflowPacket::NetflowPacket() {
        template_v9_ = std::make_shared<std::unordered_map<uint32_t, NetflowTemplateV9>>();
        flow_ = std::make_shared<std::unordered_map<int, std::pair<int, std::shared_ptr<char>>>>();
    }

    NetflowPacket::~NetflowPacket() = default;

    int NetflowPacket::process_packet(struct nf_packet_info *npi, size_t len) {
        uint16_t *version_ptr;
        int version;
        struct timespec tmsp{};
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

    int NetflowPacket::parse_netflow_v9(struct nf_packet_info *npi, size_t len) {
        struct nf9_header *header;
        int flowset_id, flowset_id_host, length, count;
        uint8_t *ptr;

        header = (struct nf9_header *) npi->rawpacket;
        npi->source_id = header->source_id;
        npi->epoch = header->unix_secs;

        // device_get_sampling_rate(npi->src_addr_ipv4);

        ptr = (uint8_t *) npi->rawpacket + sizeof(struct nf9_header);

        while (ptr < ((uint8_t *) npi->rawpacket + len)) {
            struct nf9_flowset_header *flowset_header;

            flowset_header = (struct nf9_flowset_header *) ptr;

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

    int NetflowPacket::parse_netflow_v9_template(struct nf_packet_info *npi, uint8_t **ptr, int length) {
        struct nf9_template_item *ptmpl;
        uint16_t template_id, field_count;
        int template_size;

        ptmpl = (struct nf9_template_item *) (*ptr);
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
        uint32_t tk = netflowTemplateV9.getTemplateKey();
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

    int NetflowPacket::parse_netflow_v9_flowset(struct nf_packet_info *npi, uint8_t **ptr, int flowset_id, int length,
                                                int count) {
        uint8_t *fptr;
        struct nf9_template_item *tmpl;
        struct template_key tkey;
        int template_field_count;

        auto tmp = template_v9_->find(NetflowTemplateV9::makeTemplateKey(4, 9, flowset_id));
        if (tmp == template_v9_->end()) {
            LOG(warning) << "Unknown flowset id " << ntohs(flowset_id) << " .";
            return 1;
        }
        LOG(info) << "found one template " << flowset_id;

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

    int NetflowPacket::parse_ipfix(struct nf_packet_info *npi, size_t len) {
        return 0;
    }

    void NetflowPacket::setDevices(const std::shared_ptr<std::vector<Device>> &devices) {
        devices_ = devices;
    }

    int NetflowPacket::device_get_sampling_rate(uint32_t src_addr_ipv4) {
        char ipstr[16];
        struct in_addr s;
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