#pragma once

#include "platform_config.h"

#ifdef __cplusplus

#include <iostream>

#define EVPP_LOG_ENABLE false

struct XLOG {
    ~XLOG() { ::std::cout << ::std::endl; }
};

template<typename T>
XLOG &&operator<<(XLOG &&wrap, T const &whatever) {
    ::std::cout << whatever;
    return ::std::move(wrap);
}

#define LOG_TRACE if (EVPP_LOG_ENABLE) XLOG() << __FILE__ << ":" << __LINE__ << " "
#define LOG_DEBUG if (EVPP_LOG_ENABLE) XLOG() << __FILE__ << ":" << __LINE__ << " "
#define LOG_INFO XLOG() << __FILE__ << ":" << __LINE__ << " "
#define LOG_WARN XLOG() << __FILE__ << ":" << __LINE__ << " "
#define LOG_ERROR XLOG() << __FILE__ << ":" << __LINE__ << " "
#define LOG_FATAL XLOG() << __FILE__ << ":" << __LINE__ << " "
#define DLOG_TRACE if (EVPP_LOG_ENABLE) XLOG() << __PRETTY_FUNCTION__ << " this=" << this << " "
#define DLOG_WARN XLOG() << __PRETTY_FUNCTION__ << " this=" << this << " "
#define CHECK_NOTnullptr(val) LOG_ERROR << "'" #val "' Must be non nullptr";
#endif // end of define __cplusplus
