//
// Created by Will Zhang on 2021/7/12.
//
#include "Client.h"

#include <utility>

namespace PiRPC {
    void Client::init(std::string address, std::string clientName) {
        addr = std::move(address);
        name = std::move(clientName);
        loop = new evpp::EventLoop();
        client = new evpp::TCPClient(loop, addr, name);
        client->SetConnectionCallback([this](const evpp::TCPConnPtr &connPtr) {
            spdlog::info("{} connection status:{}", client->name(), connPtr->status());
            if (_onConnectionChanged) {
                _onConnectionChanged(connPtr->status());
            }
        });
        client->SetMessageCallback([this](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            Stream2Package::Do(_onNewMsgReceived, buffer);
        });
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
