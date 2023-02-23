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
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <boost/property_tree/json_parser.hpp>

#include "NetflowConst.h"
#include "NetflowLog.h"
#include "NetflowController.h"

namespace nfc {

    void NetflowController::run() {
        int sock_fd;
        struct sockaddr_in server{};
        struct nf_packet_info nfpkt{};
        socklen_t clientlen = sizeof(struct sockaddr);

        // Create a UDP socket
        if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            LOG(error) << "Failed to create socket";
            return;
        }

        // Bind to port
        memset((char *) &server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(port_);
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock_fd, (struct sockaddr *) &server, sizeof(server)) == -1) {
            LOG(error) << "Failed to bind to port 2055";
            return;
        }

        // Wait for incoming packets
        while (run_flag_) {
            memset(&nfpkt.rawpacket, 0, MAX_NF_PACKET_SIZE);
            auto packet_len = recvfrom(sock_fd, nfpkt.rawpacket, MAX_NF_PACKET_SIZE, 0, &(nfpkt.src_addr), &clientlen);
            if (packet_len < 0) {
                LOG(error) << "Failed to receive packet";
            } else {
                // Pass the packet to the NetflowPacket class for processing
                if (nfpkt.src_addr.sa_family == AF_INET) {
                    struct sockaddr_in *addr;
                    addr = (struct sockaddr_in *) &nfpkt.src_addr;
                    /* we're supporting only IPv4 */
                    nfpkt.src_addr_ipv4 =
                            *((uint32_t *) &(addr->sin_addr));
                } else {
                    nfpkt.src_addr_ipv4 = 0;
                }

                netflow_packet_.process_packet(&nfpkt, packet_len);
            }
        }

        // Close the socket
        close(sock_fd);
    }

    bool NetflowController::isRunFlag() const {
        return run_flag_;
    }

    void NetflowController::setRunFlag(bool runFlag) {
        run_flag_ = runFlag;
    }

    NetflowController::NetflowController(const char *filename, int port) : port_(port) {
        devices_load(filename);
    }

    int NetflowController::devices_load(const char *filename) {
        std::shared_ptr<std::vector<Device>> devices = std::make_shared<std::vector<Device>>();

        boost::property_tree::ptree root;
        boost::property_tree::ptree items;
        boost::property_tree::read_json<boost::property_tree::ptree>(filename, root);
        for (const auto &d: root) {
            Device device;
            device.setIp(d.second.get<std::string>("ip", ""));
            device.setId(d.second.get<int>("id", 0));
            device.setSamplingRate(d.second.get<int>("sampling-rate", 1));
            devices->push_back(device);
        }

        netflow_packet_.setDevices(devices);
        return 0;
    }

    NetflowController::~NetflowController() = default;
} // nfc