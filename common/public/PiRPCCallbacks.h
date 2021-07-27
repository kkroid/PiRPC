//
// Created by Will Zhang on 2021/7/25.
//

#ifndef PITWINSCLIENT_PIRPCCALLBACKS_H
#define PITWINSCLIENT_PIRPCCALLBACKS_H

#endif //PITWINSCLIENT_PIRPCCALLBACKS_H

namespace PiRPC {
    typedef std::function<void(const char *, size_t)> OnNewMsgReceived;

    typedef std::function<void(const int)> OnConnectionChanged;
}