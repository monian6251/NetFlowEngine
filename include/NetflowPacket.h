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

#ifndef NETFLOWENGINE_NETFLOWPACKET_H
#define NETFLOWENGINE_NETFLOWPACKET_H

#include <cstring>
#include <vector>
#include <unordered_map>
#include <map>
#include "Device.h"
#include "NetflowTemplateV9.h"
#include "NetflowTemplateIpfix.h"

namespace nfc {

    class NetflowPacket {
    public:
        NetflowPacket();

        virtual ~NetflowPacket();

        int process_packet(nf_packet_info *nfPacketInfo, size_t len);

        void setDevices(const std::shared_ptr<std::vector<Device>> &devices);

    private:
        int parse_netflow_v9(nf_packet_info *npi, size_t len);

        int parse_netflow_v9_template(nf_packet_info *npi, uint8_t **ptr, int length);

        int parse_netflow_v9_flowset(nf_packet_info *npi, uint8_t **ptr,
                                     int flowset_id, int length, int count);

        int parse_ipfix(nf_packet_info *npi, size_t len);

        int parse_ipfix_template(nf_packet_info *npi, uint8_t **ptr, int length);

        int parse_ipfix_flowset(nf_packet_info *npi, uint8_t **ptr,
                                int flowset_id, int length);

        int device_get_sampling_rate(uint32_t src_addr_ipv4);

        void print_one_flow();

    private:
        std::shared_ptr<std::vector<Device>> devices_;
        std::shared_ptr<std::map<template_key, NetflowTemplateV9>> template_v9_;
        std::shared_ptr<std::map<template_key, NetflowTemplateIpfix>> template_ipfix_;
        std::shared_ptr<std::unordered_map<int, std::pair<int, std::shared_ptr<char>>>> flow_;

    };

} // nfc

#endif //NETFLOWENGINE_NETFLOWPACKET_H
