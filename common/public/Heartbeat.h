//
// Created by Will Zhang on 2021/7/28.
//

#ifndef PITWINSCLIENT_HEARTBEAT_H
#define PITWINSCLIENT_HEARTBEAT_H

#include <Client.h>
#include "Snowflak.h"
#include "spdlog/spdlog.h"

namespace PiRPC {

    using namespace std;

    class Heartbeat {
    private:
        UInt64 lastHeartbeatTime = 0UL;
    public:
        constexpr static const double INTERVAL = 5.0;

        Heartbeat() = default;

        ~Heartbeat() {
            spdlog::info("~Heartbeat");
        }

        static Heartbeat &getInstance() {
            static Heartbeat instance;
            return instance;
        }

        // 拒绝拷贝构造
        Heartbeat(const Heartbeat &rhs) = delete;

        // 拒绝拷贝赋值
        Heartbeat &operator=(const Heartbeat &rhs) = delete;

        void refresh() {
            lastHeartbeatTime = Snowflake::GetTimeStamp();
            spdlog::trace("Refresh current heartbeat");
        }

        void beat() {
            UInt64 currentTime = Snowflake::GetTimeStamp();
            UInt64 delta = currentTime - lastHeartbeatTime;
            spdlog::trace("Current heartbeat:{}", delta);
            if (delta < INTERVAL * 1000) {
                spdlog::trace("Ignore current heartbeat");
                return;
            }
            lastHeartbeatTime = currentTime;
            Client::getMsgClient().heartbeat();
        }
    };
}

#endif //PITWINSCLIENT_HEARTBEAT_H
