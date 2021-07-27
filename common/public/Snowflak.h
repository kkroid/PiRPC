/**
 * 文件名称：Snowflake.h
 * 文件标识：
 * 摘    要：通过SnowFlake算法生成一个64位大小的分布式自增长id
 */

#ifndef __SNOWFLAKE_H__
#define __SNOWFLAKE_H__

#include <mutex>
#include <atomic>
#include <iostream>

//#define SNOWFLAKE_ID_WORKER_NO_LOCK
typedef unsigned int UInt;
typedef long long unsigned int UInt64;

#ifdef SNOWFLAKE_ID_WORKER_NO_LOCK
typedef std::atomic<UInt> AtomicUInt;
typedef std::atomic<UInt64> AtomicUInt64;
#else
typedef UInt AtomicUInt;
typedef UInt64 AtomicUInt64;
#endif

namespace PiRPC {
    class Snowflake {
    public:
        Snowflake() {
            m_HostId = 1;
            m_WorkerId = 1;
            sequence = 0;
            lastTimestamp = 0;
        }

        ~Snowflake() = default;

        void setHostId(UInt HostId) {
            m_HostId = HostId;
        }

        void setWorkerId(UInt workerId) {
            m_WorkerId = workerId;
        }

        UInt64 nextId() {
            return GetDistributedId();
        }

    private:
        static UInt64 GetTimeStamp() {
            auto t = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now());
            return t.time_since_epoch().count();
        }

        static UInt64 tilNextMillis(UInt64 lastTimestamp) {
            UInt64 timestamp = GetTimeStamp();
            while (timestamp <= lastTimestamp) {
                timestamp = GetTimeStamp();
            }
            return timestamp;
        }

        UInt64 GetDistributedId() {
#ifndef SNOWFLAKE_ID_WORKER_NO_LOCK
            std::unique_lock<std::mutex> lock{mutex};
            AtomicUInt64 timestamp{0};
#else
            static AtomicUInt64 timestamp{ 0 };
#endif

            timestamp = GetTimeStamp();
            // 如果当前时间小于上一次ID生成的时间戳,说明系统时钟回退过这个时候应当抛出异常
            if (timestamp < lastTimestamp) {
                std::cout << "clock moved backwards.  Refusing to generate id for " << lastTimestamp - timestamp
                          << " milliseconds";
                return 0;
            }

            if (lastTimestamp == timestamp) {
                // 如果是同一时间生成的，则进行毫秒内序列
                sequence = (sequence + 1) & sequenceMask;
                if (0 == sequence) {
                    // 毫秒内序列溢出, 阻塞到下一个毫秒,获得新的时间戳
                    timestamp = tilNextMillis(lastTimestamp);
                }
            } else {
                sequence = 0;
            }

#ifndef SNOWFLAKE_ID_WORKER_NO_LOCK
            lastTimestamp = timestamp;
#else
            lastTimestamp = timestamp.load();
#endif

            // 移位并通过或运算拼到一起组成64位的ID
            return ((timestamp - twepoch) << timestampLeftShift)
                   | (m_HostId << hostIdShift)
                   | (m_WorkerId << workerIdShift)
                   | sequence;

        }

    private:

#ifndef SNOWFLAKE_ID_WORKER_NO_LOCK
        std::mutex mutex;
#endif

        /**
        * 开始时间截 (2019-09-30 00:00:00.000)
        */
        const UInt64 twepoch = 1569772800000UL;

        /**
        * worker进程映射id所占的位数
        */
        const UInt workerIdBits = 5;

        /**
        * 服务器id所占的位数
        */
        const UInt hostIdBits = 5;

        /**
        * 序列所占的位数
        */
        const UInt sequenceBits = 12;

        /**
        * worker进程映射ID向左移12位
        */
        const UInt workerIdShift = sequenceBits;

        /**
        * 服务器id向左移17位
        */
        const UInt hostIdShift = workerIdShift + workerIdBits;

        /**
        * 时间截向左移22位
        */
        const UInt timestampLeftShift = hostIdShift + hostIdBits;

        /**
        * 支持的worker进程映射id，结果是31
        */
        const UInt maxWorkerId = -1 ^(-1 << workerIdBits);

        /**
        * 支持的服务器id，结果是31
        */
        const UInt maxHostId = -1 ^(-1 << hostIdBits);

        /**
        * 生成序列的掩码，这里为4095
        */
        const UInt sequenceMask = -1 ^(-1 << sequenceBits);

        /**
        * worker进程映射id(0~31)
        */
        UInt m_WorkerId;

        /**
        * 服务器id(0~31)
        */
        UInt m_HostId;

        /**
        * 毫秒内序列(0~4095)
        */
        AtomicUInt sequence{0};

        /**
        * 上次生成ID的时间截
        */
        AtomicUInt64 lastTimestamp{0};
    };
}

#endif