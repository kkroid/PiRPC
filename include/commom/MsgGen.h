//
// Created by Will Zhang on 2021/7/21.
//

#ifndef PITWINS_MESSAGEGENERATOR_H
#define PITWINS_MESSAGEGENERATOR_H

#include <utility>

#include "Snowflake.h"

#define TYPE_HEARTBEAT 0
#define TYPE_CAMERA_CTRL 1
#define TYPE_SERVO_CTRL 2
#define TYPE_MOTOR_CTRL 3

#define BASE_MSG_FORMAT R"({"tid":%llu,"type":%d,"payload":%s})"
#define BASE_SERVO_MOTOR_MSG_FORMAT R"({"servo":%d,"delta":%d})"
#define BASE_CAMERA_MSG_FORMAT R"({"cmd":%d,"param":%d})"

namespace PiRPC {
    class MsgGen {

    public:
        template<typename ... Args>
        static std::string format(const std::string &format, Args ... args) {
            int size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
            if (size <= 0) { throw std::runtime_error("Error during formatting."); }
            std::unique_ptr<char[]> buf(new char[size]);
            snprintf(buf.get(), size, format.c_str(), args ...);
            return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
        }

        static std::string msg(int type, const std::string &payload) {
            static Snowflake snowflake;
            return format(BASE_MSG_FORMAT, snowflake.nextId(), type, payload.c_str());
        }

        static std::string camera(int cmd, int param) {
            return msg(TYPE_CAMERA_CTRL, format(BASE_CAMERA_MSG_FORMAT, cmd, param));
        }

        static std::string servo(int servo, int delta) {
            return msg(TYPE_SERVO_CTRL, format(BASE_SERVO_MOTOR_MSG_FORMAT, servo, delta));
        }

        static std::string heartbeat() {
            return msg(TYPE_HEARTBEAT, "\"\"");
        }

        static std::string motor(int key) {
            return msg(TYPE_MOTOR_CTRL, format(BASE_SERVO_MOTOR_MSG_FORMAT, key));
        }
    };
}

#endif //PITWINS_MESSAGEGENERATOR_H
