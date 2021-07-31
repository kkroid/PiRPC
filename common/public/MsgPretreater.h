//
// Created by Will Zhang on 2021/7/31.
//

#ifndef PITWINS_MSGPRETREATER_H
#define PITWINS_MSGPRETREATER_H

#include "PiRPCCallbacks.h"
#include "json.hpp"
#include "spdlog/spdlog.h"

namespace PiRPC {
    class MsgPretreater {
    private:
        const static int32_t HEADER_SIZE = sizeof(int32_t);
    public:
        MsgPretreater() = default;

        ~MsgPretreater() = default;

        MsgPretreater(const MsgPretreater &rhs) = delete;

        MsgPretreater &operator=(const MsgPretreater &rhs) = delete;

        static MsgPretreater &getInstance() {
            static MsgPretreater instance;
            return instance;
        }

        void pretreat(const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {

        }

        void stream2Package(evpp::Buffer *buffer, const OnPackageReceived &onPackageReceived) {
            if (buffer->size() < HEADER_SIZE) {
                spdlog::warn("收到的数据比头部还小，等待。。。");
                return;
            }
            // 拆包分包处理
            int32_t realDataSize = buffer->PeekInt32();
            int32_t headerSize = sizeof(realDataSize);
            int32_t currentDataSize = buffer->size() - headerSize;
            int32_t delta = realDataSize - currentDataSize;
            if (delta > 0) {
                spdlog::trace("数据不够,realDataSize:{}, currentDataSize:{}, peek:{}, delta:{}", realDataSize,
                              currentDataSize, buffer->PeekInt32(), realDataSize - currentDataSize);
                return;
            } else if (delta == 0) {
                spdlog::trace("数据充足,realDataSize:{}, currentDataSize:{}, headerSize:{}", realDataSize, currentDataSize,
                              headerSize);
                buffer->Skip(headerSize);
                if (onPackageReceived) {
                    onPackageReceived(buffer);
                }
                // TODO 调查为什么需要reset
                buffer->Reset();
            } else {
                int count = 0;
                while (true) {
                    if (realDataSize - currentDataSize < 0) {
                        spdlog::trace("数据多了{}，拆包，发送处理, realDataSize:{}, currentDataSize:{}, headerSize:{}", count++,
                                      realDataSize, currentDataSize, headerSize);
                        evpp::Buffer data;
                        buffer->Skip(headerSize);
                        data.Append(buffer->data(), realDataSize);
                        buffer->Skip(realDataSize);
                        if (onPackageReceived) {
                            onPackageReceived(&data);
                        }
                        realDataSize = buffer->PeekInt32();
                        currentDataSize = buffer->size() - headerSize;
                    } else {
                        break;
                    }
                }
            }
        }

        nlohmann::json parse(const std::string &msg) {
            if (msg.empty()) {
                return nullptr;
            }
            try {
                spdlog::warn("json parse msg:[{}]", msg);
                nlohmann::json obj = nlohmann::json::parse(msg);
                return obj;
            } catch (std::exception &e) {
                spdlog::warn("json parse error:{} for msg:[{}]", e.what(), msg);
                return nullptr;
            }
        }
    };
}

#endif //PITWINS_MSGPRETREATER_H
