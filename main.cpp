#include <thread>
#include <iostream>
#include "NetflowLog.h"
#include "NetflowController.h"

int main() {
    char cmd;

    init_log_environment("log.ini");

    nfc::NetflowController netflowController("devices.conf", 8888);
    std::thread main_work([&netflowController]() { netflowController.run(); });
    main_work.detach();
    LOG(info) << "main work start.";

    while (true) {
        std::cout << "press Q to exit. " << std::endl;
        std::cin >> cmd;
        if (cmd == 'q' || cmd == 'Q') {
            netflowController.setRunFlag(false);
            break;
        }
    }

    if (main_work.joinable()) {
        main_work.join();
    }

    return 0;
}
