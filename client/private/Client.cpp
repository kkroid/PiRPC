//
// Created by Will Zhang on 2021/7/12.
//
#include "Client.h"
#include <utility>

namespace PiRPC {
    void Client::init(std::string address, std::string clientName, bool heartbeat_) {
        addr = std::move(address);
        name = std::move(clientName);
        loop = new evpp::EventLoop();
        client = new evpp::TCPClient(loop, addr, name);
        _heartbeat = heartbeat_;
        client->SetConnectionCallback([this](const evpp::TCPConnPtr &connPtr) {
            spdlog::info("{} connection status:{}", client->name(), connPtr->status());
            if (_onConnectionChanged) {
                _onConnectionChanged(connPtr->status());
            }
            if (_heartbeat && connPtr->IsConnected()) {
                loop->RunEvery(evpp::Duration(INTERVAL), [this]() {
                    heartbeat();
                });
            }
        });
        client->SetMessageCallback([this](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            Stream2Package::Do(_onNewMsgReceived, buffer);
        });
    }

    void Client::Send(const void *d, size_t dlen) {
        if (client && client->conn()->IsConnected()) {
            evpp::Buffer buffer;
            buffer.AppendInt32(dlen);
            buffer.Append(d, dlen);
            client->conn()->Send(&buffer);
            if (_heartbeat) {
                refreshHeartbeat();
            }
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
