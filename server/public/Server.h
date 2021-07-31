//
// Created by Will Zhang on 2021/7/10.
//

#ifndef PITWINS_SERVER_H
#define PITWINS_SERVER_H

#include <spdlog/spdlog.h>
#include <tcp_server.h>
#include <buffer.h>
#include <tcp_conn.h>
#include <event_loop.h>

#include <utility>
#include <PiRPCCallbacks.h>
#include "buffer.h"
#include "MsgPretreater.h"
#include "MessageDispatcher.h"
#include "Config.h"

#define ADDR_MSG_SERVER "0.0.0.0:5556"
#define ADDR_VIDEO_SERVER "0.0.0.0:5555"
#define NAME_MSG_SERVER "MsgServer"
#define NAME_VIDEO_SERVER "VideoServer"
#define THREAD_NUM 2

using namespace evpp;

namespace PiRPC {
    class Server {
    private:
        int threadNum = THREAD_NUM;
        std::string addr = ADDR_MSG_SERVER;
        std::string name = NAME_MSG_SERVER;
        TCPServer *server = nullptr;
        EventLoop *loop = nullptr;
        std::shared_ptr<TCPConn> tcpConnPtr = nullptr;
        std::map<uint64_t/*the id of the connection*/, UInt64> _connectionMap;
        OnConnectionChanged _onConnectionChanged = nullptr;
        MessageDispatcher *_msgDispatcher = nullptr;
        OnHeartbeatTimeout _onHeartbeatTimeout;
    public:

        Server() = default;

        ~Server() {
            spdlog::info("~Server");
            if (tcpConnPtr) {
                tcpConnPtr->Close();
            }
            delete loop;
            delete server;
        }

        static Server &getVideoInstance() {
            static Server videoInstance;
            return videoInstance;
        }

        static Server &getMsgInstance() {
            static Server msgInstance;
            return msgInstance;
        }

        void init(std::string address, std::string serverName) {
            addr = std::move(address);
            name = std::move(serverName);
            loop = new EventLoop();
            server = new TCPServer(loop, addr, name, threadNum);

            server->SetConnectionCallback([this](const TCPConnPtr &connPtr) {
                if (connPtr->IsConnected()) {
                    spdlog::info("[{}]:Client {} Connected", name, connPtr->remote_addr());
                    // TODO 这里是新连接覆盖旧连接，将来改成支持多连接
                    tcpConnPtr = connPtr;
                    _connectionMap[connPtr->id()] = Snowflake::GetTimeStamp();
                } else if (connPtr->IsDisconnected()) {
                    spdlog::info("[{}]:Client {} Disconnected", name, connPtr->remote_addr());
                    tcpConnPtr = nullptr;
                }
                if (_onConnectionChanged) {
                    _onConnectionChanged(connPtr->status());
                }
            });

            server->SetMessageCallback([this](const TCPConnPtr &connPtr, Buffer *buffer) {
                MsgPretreater::getInstance().stream2Package(buffer, [this, connPtr](Buffer *buf) {
                    onPackageReceived(connPtr, buf);
                });
            });
            loop->RunEvery(evpp::Duration(INTERVAL / 1000.0), [this]() {
                checkHeartbeat();
            });
        }

        void setOnConnectionChangedCallback(const PiRPC::OnConnectionChanged &onConnectionChanged) {
            _onConnectionChanged = onConnectionChanged;
        }

        void setMessageDispatcher(MessageDispatcher *messageDispatcher) {
            _msgDispatcher = messageDispatcher;
        }

        void run();

        bool isRunning() {
            return server != nullptr && server->IsRunning();
        }

        void send(const void *data, size_t len) {
            if (tcpConnPtr && tcpConnPtr->IsConnected()) {
                Buffer buffer;
                buffer.AppendInt32(len);
                buffer.Append(data, len);
                tcpConnPtr->Send(&buffer);
                // spdlog::info("{} send a msg to {}", name, tcpConnPtr->remote_addr());
            }
        }

        void send(const std::string &d) {
            send(d.c_str(), d.length());
        }

        void onPackageReceived(const TCPConnPtr &connPtr, Buffer *buf);

        void checkHeartbeat();

        void release();

        // 拒绝拷贝构造
        Server(const Server &rhs) = delete;

        // 拒绝拷贝赋值
        Server &operator=(const Server &rhs) = delete;
    };
}

#endif //PITWINS_SERVER_H
