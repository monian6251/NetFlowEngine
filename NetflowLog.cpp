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

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/regex.hpp>
#include "NetflowLog.h"

std::string get_file_name() {
    std::string f = __FILE__;
    auto last_pos = f.find_last_of('/');
    if (last_pos != std::string::npos) {
        f = f.substr(last_pos + 1, f.size());
    }
    return f;
}

bool init_log_environment(const std::string &cfg) {
    namespace logging = boost::log;
    using namespace logging::trivial;

    if (!boost::filesystem::exists("./log/")) {
        boost::filesystem::create_directory("./log/");
    }
    logging::add_common_attributes();

    logging::register_simple_formatter_factory<severity_level, char>("Severity");
    logging::register_simple_filter_factory<severity_level, char>("Severity");

    std::ifstream file(cfg);
    try {
        logging::init_from_stream(file);
    }
    catch (const std::exception &e) {
        std::cout << "init_logger is fail, read log config file fail. curse: " << e.what() << std::endl;
        exit(-2);
    }
    return true;
}
