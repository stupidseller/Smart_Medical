#ifndef SHARED_DATA_TYPES_H
#define SHARED_DATA_TYPES_H
#pragma once

#include <QString>
#include <QList>

namespace SharedTypes {

// 聊天消息（供需要“公共模型”的模块使用）
// 不会与全局的 ChatMessage 冲突
struct ChatMessage {
    QString content;
    QString timestamp;
    bool    sentByDoctor; // true 医生发，false 患者发
};

// 医生公共信息（包含预约用到的时段）
struct DoctorInfo {
    struct AppointmentSlot {
        QString time;      // 如 "09:00-09:30"
        bool    isAvailable;
    };

    // 基础信息
    QString id;
    QString name;
    QString department; // 科室
    QString title;      // 职称
    bool    isOnline = true;

    // 预约信息
    QList<AppointmentSlot> availableSlots;
};

} // namespace SharedTypes

#endif // SHARED_DATA_TYPES_H
