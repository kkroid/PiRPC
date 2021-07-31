//
// Created by Will Zhang on 2021/7/10.
//

#include "Server.h"
#include "json.hpp"

using namespace nlohmann;

namespace PiRPC {
    void Server::run() {
        if (nullptr != server) {
            spdlog::info("{} is running", name);
            server->Init();
            server->Start();
            loop->Run();
        }
    }

    void Server::onPackageReceived(const TCPConnPtr &connPtr, Buffer *buf) {
        spdlog::trace("{} server received a msg from:{}", name, connPtr->remote_addr());
        std::string msg = std::string(buf->data(), buf->size());
        json msgJson = MsgPretreater::getInstance().parse(msg);
        if (_msgDispatcher) {
            _msgDispatcher->dispatch(msgJson);
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
