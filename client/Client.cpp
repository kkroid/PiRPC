//
// Created by Will Zhang on 2021/7/12.
//
#include "Client.h"
#include <utility>
#include "commom/json.hpp"

using namespace nlohmann;
using namespace evpp;

namespace PiRPC {
    void Client::init(std::string address, std::string clientName) {
        addr = std::move(address);
        name = std::move(clientName);
        loop = new EventLoop();
        client = new TCPClient(loop, addr, name);
        client->SetConnectionCallback([this](const TCPConnPtr &connPtr) {
            spdlog::info("{} connection status:{}", client->name(), connPtr->status());
            if (_onConnectionChanged) {
                _onConnectionChanged(connPtr->status());
            }
            loop->RunEvery(Duration(INTERVAL), [this]() {
                heartbeat();
            });
        });
        client->SetMessageCallback([this](const TCPConnPtr &connPtr, Buffer *buffer) {
            MsgPretreater::getInstance().stream2Package(buffer, [this, connPtr](Buffer *buf) {
                onPackageReceived(connPtr, buf);
            });
        });
    }

    void Client::onPackageReceived(const evpp::TCPConnPtr &connPtr, evpp::Buffer *buf) {
        if (_onNewMsgReceived) {
            _onNewMsgReceived(buf->data(), buf->size());
        }
    }

    void Client::connect() {
        client->Connect();
        client->set_auto_reconnect(true);
        loop->Run();
    }

    void Client::disconnect() {
        if (client) {
            client->Disconnect();
            loop->Stop();
        }
    }
}
