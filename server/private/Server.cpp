//
// Created by Will Zhang on 2021/7/10.
//

#include "Server.h"


namespace PiRPC {
    void Server::run() {
        if (nullptr != server) {
            spdlog::info("{} is running", name);
            server->Init();
            server->Start();
            loop->Run();
        }
    }

// FIXME 此处会crash
    void Server::release() {
        spdlog::info("[{}]:{} released", name);
        server->Stop([this]() {
            spdlog::info("[{}]:{} stopped", name);
        });
        loop->Stop();
        delete loop;
        loop = nullptr;
        delete server;
        server = nullptr;
    }
}
