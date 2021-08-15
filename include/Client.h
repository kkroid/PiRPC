//
// Created by Will Zhang on 2021/7/12.
//

#ifndef PITWINSCLIENT_CLIENT_H
#define PITWINSCLIENT_CLIENT_H

#include <spdlog/spdlog.h>
#include <evpp/tcp_client.h>
#include <evpp/buffer.h>
#include <evpp/event_loop.h>
#include <evpp/tcp_conn.h>
#include <utility>
#include "commom/PiRPCCallbacks.h"
#include "commom/Snowflake.h"
#include "commom/MsgGen.h"
#include "commom/MsgPretreater.h"

using namespace evpp;

namespace PiRPC {
    class Client {
    private:
        constexpr static const double INTERVAL = 5.0;
        std::string addr = "";
        std::string name = "Client";
        evpp::TCPClient *client = nullptr;
        evpp::EventLoop *loop = nullptr;
        PiRPC::OnNewMsgReceived _onNewMsgReceived = nullptr;
        PiRPC::OnConnectionChanged _onConnectionChanged = nullptr;
        UInt64 lastHeartbeatTime = 0UL;
    public:

        Client() = default;

        ~Client() {
            spdlog::info("~Client:{}", name);
            delete loop;
            delete client;
        }

        static Client &getVideoClient() {
            static Client videoClient;
            return videoClient;
        }

        static Client &getMsgClient() {
            static Client msgClient;
            return msgClient;
        }

        // 拒绝拷贝构造
        Client(const Client &rhs) = delete;

        // 拒绝拷贝赋值
        Client &operator=(const Client &rhs) = delete;

        void init(std::string address, std::string clientName);

        void setOnConnectionChangedCallback(const PiRPC::OnConnectionChanged &onConnectionChanged) {
            _onConnectionChanged = onConnectionChanged;
        }

        void setOnNewMsgReceivedCallback(const PiRPC::OnNewMsgReceived &onNewMsgReceived) {
            _onNewMsgReceived = onNewMsgReceived;
        }

        void onPackageReceived(const TCPConnPtr &connPtr, Buffer *buf);

        void connect();

        void disconnect();

        void Send(const void *d, size_t dlen) {
            if (client && client->conn()->IsConnected()) {
                evpp::Buffer buffer;
                buffer.AppendInt32(dlen);
                buffer.Append(d, dlen);
                client->conn()->Send(&buffer);
                refreshHeartbeat();
            }
        }

        void Send(const char *s) {
            Send(s, strlen(s));
        }

        void Send(const std::string &msg) {
            // spdlog::info("send msg:{}", msg);
            Send(msg.data(), msg.length());
        }

    private:
        void refreshHeartbeat() {
            lastHeartbeatTime = Snowflake::GetTimeStamp();
            spdlog::trace("Refresh current heartbeat");
        }

        void heartbeat() {
            UInt64 currentTime = Snowflake::GetTimeStamp();
            UInt64 delta = currentTime - lastHeartbeatTime;
            spdlog::trace("Current heartbeat:{}", delta);
            if (delta < INTERVAL * 1000) {
                spdlog::trace("Ignore current heartbeat");
                return;
            }
            Send(MsgGen::heartbeat());
            spdlog::info("发送心跳包");
        }
    };
}


#endif //PITWINSCLIENT_CLIENT_H
