//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_MESSAGEDISPATCHER_H
#define PITWINS_MESSAGEDISPATCHER_H


#include "MessageProcessor.h"
#include <tcp_conn.h>

using namespace evpp;
using namespace PiRPC;

namespace PiRPC {
    class MessageDispatcher {
    protected:
        std::map<int, MessageProcessor *> processorMapping;
    public:
        virtual void dispatch(const TCPConnPtr &connPtr, nlohmann::json msgJson) = 0;
    };
}

#endif //PITWINS_MESSAGEDISPATCHER_H
