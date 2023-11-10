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

#ifndef NETFLOWENGINE_NETFLOWCONST_H
#define NETFLOWENGINE_NETFLOWCONST_H

#include <netinet/in.h>
#include <sys/types.h>
#include <cstdint>
#include <cstring>

#define MAX_NF_PACKET_SIZE (64*1024)

#define MAX_FLOWS_PER_PACKET 100

#define MAX_FIELDS_PER_FLOW 100

#define MAX_FLOW_VAL_LEN 32

typedef __int128_t xe_ip;

struct nf_packet_info
{
    struct sockaddr src_addr;
    uint32_t src_addr_ipv4;

    uint32_t source_id;
    uint32_t epoch;
    uint64_t time_ns; /* nanoseconds */
    uint8_t rawpacket[MAX_NF_PACKET_SIZE];

    int sampling_rate;
};

struct nf9_header
{
    uint16_t version;
    uint16_t count;
    uint32_t sys_uptime;
    uint32_t unix_secs;

    uint32_t package_sequence;
    uint32_t source_id;
} __attribute__ ((__packed__));

struct nf9_fieldtype_and_len
{
    uint16_t type;
    uint16_t length;
} __attribute__ ((__packed__));

struct nf9_flowset_header
{
    uint16_t flowset_id;
    uint16_t length;
} __attribute__ ((__packed__));

struct nf9_template_item
{
    uint16_t template_id;
    uint16_t field_count;
    struct nf9_fieldtype_and_len typelen[1];
} __attribute__ ((__packed__));

struct ipfix_header
{
    uint16_t version;
    uint16_t length;
    uint32_t export_time;

    uint32_t sequence_number;
    uint32_t observation_domain;
} __attribute__ ((__packed__));

struct ipfix_template_header
{
    uint16_t template_id;
    uint16_t field_count;
} __attribute__ ((__packed__));

struct ipfix_inf_element_iana
{
    uint16_t id;
    uint16_t length;
} __attribute__ ((__packed__));

struct ipfix_inf_element_enterprise
{
    uint16_t id;
    uint16_t length;
    uint32_t number;
} __attribute__ ((__packed__));

struct ipfix_stored_template
{
    struct ipfix_template_header header;

    struct ipfix_inf_element_enterprise elements[1];
} __attribute__ ((__packed__));

struct ipfix_flowset_header
{
    uint16_t flowset_id;
    uint16_t length;
} __attribute__ ((__packed__));

struct template_key
{
    uint8_t  src_ip_version;
    uint8_t  nf_version;
    uint16_t template_id;

    xe_ip source_ip;

    uint32_t source_id;
    uint32_t epoch;

    int operator<(const template_key &t) const {
        return strncmp((const char *)this, (const char *)&t, sizeof(template_key) - 4);
    }
} __attribute__ ((__packed__));

#endif //NETFLOWENGINE_NETFLOWCONST_H
