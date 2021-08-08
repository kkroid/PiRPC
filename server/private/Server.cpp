//
// Created by Will Zhang on 2021/7/10.
//

#include "Server.h"
#include "json.hpp"

using namespace nlohmann;

#define TIMEOUT INTERVAL * 2UL

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
        _connectionMap[connPtr->id()] = Snowflake::GetTimeStamp();
        spdlog::trace("{} server received a msg from:{}", name, connPtr->remote_addr());
        std::string msg = std::string(buf->data(), buf->size());
        json msgJson = MsgPretreater::getInstance().parse(msg);
        if (_msgDispatcher) {
            _msgDispatcher->dispatch(connPtr, msgJson);
        }
    }

    void Server::checkHeartbeat() {
        UInt64 currentTime = Snowflake::GetTimeStamp();
        for (auto & it : _connectionMap) {
            UInt64 lastMsgTime = it.second;
            UInt64 delta = currentTime - lastMsgTime;
            if (delta > TIMEOUT) {
                TCPConnPtr connPtr = server->GetConnPtrById(it.first);
                if (connPtr) {
                    if (_onHeartbeatTimeout) {
                        _onHeartbeatTimeout(connPtr);
                    } else {
                        // TODO 释放死链接
                        // connPtr->loop()->RunInLoop([this, connPtr]() {
                        //     spdlog::info("[checkHeartbeat]:{} timeout, close", connPtr->remote_addr());
                        //     if (connPtr->IsConnected()) {
                        //         connPtr->Close();
                        //     }
                        // });
                        spdlog::info("[checkHeartbeat]:{} timeout, close", connPtr->remote_addr());
                        // if (tcpConnPtr && tcpConnPtr->IsConnected()) {
                        //     if (connPtr->id() == tcpConnPtr->id()) {
                        //         tcpConnPtr = nullptr;
                        //     }
                        // }
                        // connPtr = nullptr;
                        // _connectionMap.erase(it->first);
                    }
                }
            }
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
