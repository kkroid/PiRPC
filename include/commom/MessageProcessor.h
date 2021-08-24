//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_MESSAGEPROCESSOR_H
#define PITWINS_MESSAGEPROCESSOR_H

#include "json.hpp"
#include "spdlog/spdlog.h"
#include <evpp/buffer.h>
#include "commom/MsgGen.h"
#include "commom/Snowflake.h"


namespace PiRPC {
    class MessageProcessor {
    protected:
        std::map<uint64_t/*the id of the connection*/, UInt64> _connectionMap;
    public:
        virtual void process(nlohmann::json msg) = 0;

        void updateConnectionMap(uint64_t id) {
            _connectionMap[id] = Snowflake::GetTimeStamp();
        }

        static void processUnknownMessage(const std::string &msg, const std::string &error) {
            spdlog::warn("processUnknownMessage error:{}, msg:{}", error, msg);
        }
    };
}
#endif //PITWINS_MESSAGEPROCESSOR_H
