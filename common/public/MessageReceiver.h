//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_MESSAGERECEIVER_H
#define PITWINS_MESSAGERECEIVER_H


#include "MessageProcessor.h"

using namespace evpp;

namespace PiRPC {
    class MessageReceiver {
    protected:
        std::map<int, MessageProcessor *> processorMapping;
    public:
        virtual void onNewMsgReceived(const std::string &msg) = 0;
    };
}

#endif //PITWINS_MESSAGERECEIVER_H
