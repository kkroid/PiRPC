//
// Created by Will Zhang on 2021/7/25.
//

#ifndef PITWINSCLIENT_STREAM2PACKAGE_H
#define PITWINSCLIENT_STREAM2PACKAGE_H

#include "PiRPCCallbacks.h"

namespace PiRPC {
    class Stream2Package {
    public:
        static void Do(const OnNewMsgReceived &onNewMsgReceived, evpp::Buffer *buffer) {
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
                if (onNewMsgReceived) {
                    onNewMsgReceived(buffer->data(), buffer->size());
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
                        if (onNewMsgReceived) {
                            onNewMsgReceived(data.data(), data.size());
                        }
                        realDataSize = buffer->PeekInt32();
                        currentDataSize = buffer->size() - headerSize;
                    } else {
                        break;
                    }
                }
            }
        }


    };
}

#endif //PITWINSCLIENT_STREAM2PACKAGE_H
