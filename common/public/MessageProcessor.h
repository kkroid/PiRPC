//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_MESSAGEPROCESSOR_H
#define PITWINS_MESSAGEPROCESSOR_H

#include "json.hpp"
#include "spdlog/spdlog.h"
#include <buffer.h>
#include "MessageGenerator.h"

namespace PiRPC {
    class MessageProcessor {
    public:
        virtual void process(nlohmann::json msg) = 0;

        static void processUnknownMessage(const std::string& msg, const std::string &error) {
            spdlog::warn("processUnknownMessage error:{}, msg:{}", error, msg);
        }
    };
}
#endif //PITWINS_MESSAGEPROCESSOR_H
