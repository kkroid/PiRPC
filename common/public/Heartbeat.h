//
// Created by Will Zhang on 2021/7/28.
//

#ifndef PITWINSCLIENT_HEARTBEAT_H
#define PITWINSCLIENT_HEARTBEAT_H

#include <blockingconcurrentqueue.h>
#include "Snowflak.h"
#include "spdlog/spdlog.h"
#include "ctpl_stl.h"

namespace PiRPC {

    using namespace std;
    using namespace std::chrono;

    class Heartbeat {
    private:
        UInt64 lastHeartbeatTime = 0UL;
        UInt64 INTERVAL = 5000UL;
        ctpl::thread_pool *threadPool = nullptr;
        moodycamel::BlockingConcurrentQueue<bool> heartbeatQueue;
    public:
        Heartbeat() = default;

        ~Heartbeat() {
            spdlog::info("~Heartbeat");
            if (threadPool) {
                threadPool->clear_queue();
                delete threadPool;
                threadPool = nullptr;
            }
        }

        static Heartbeat &getInstance() {
            static Heartbeat instance;
            return instance;
        }

        // 拒绝拷贝构造
        Heartbeat(const Heartbeat &rhs) = delete;

        // 拒绝拷贝赋值
        Heartbeat &operator=(const Heartbeat &rhs) = delete;

        void init() {
            threadPool = new ctpl::thread_pool(1);
            threadPool->push([this](int id) {
                while (true) {
                    bool heartbeat = true;
                    if (heartbeatQueue.wait_dequeue_timed(heartbeat, milliseconds(INTERVAL))) {
                        if (!heartbeat) {
                            return;
                        }
                        // 其他地方已经发送了数据包，忽略本次心跳
                        continue;
                    } else {
                        Client::getMsgClient().heartbeat();
                    }
                }
            });
        }

        void beat() {
            heartbeatQueue.enqueue(true);
        }

        void release() {
            heartbeatQueue.enqueue(false);
        }
    };
}

#endif //PITWINSCLIENT_HEARTBEAT_H
