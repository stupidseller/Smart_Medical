#ifndef SHARED_DATA_TYPES_H
#define SHARED_DATA_TYPES_H

#include <QString>
#include <QList> // 为了使用 QList<AppointmentSlot>，需要包含此头文件

// 聊天消息结构体
struct ChatMessage {
    QString content;
    QString timestamp;
    bool sentByDoctor; // true是医生发的，false是患者发的
};

// 【已整合】医生基本信息结构体
// 这个版本同时包含了聊天和预约模块所需的所有信息
struct DoctorInfo {
    // 嵌套结构体，用于表示一个可预约的时间段
    struct AppointmentSlot {
        QString time;      // 例如 "09:00-09:30"
        bool isAvailable;  // 是否可预约
    };

    // 基础信息
    QString id;
    QString name;
    QString department; // 科室
    QString title;      // 职称，例如 “主任医师”
    bool isOnline = true;

    // 预约模块所需信息
    QList<AppointmentSlot> availableSlots; // 医生可预约的时间段列表
};


#endif // SHARED_DATA_TYPES_H