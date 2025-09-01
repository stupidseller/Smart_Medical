#ifndef PATIENT_LIST_ITEM_WIDGET_H
#define PATIENT_LIST_ITEM_WIDGET_H

#include <QPushButton>
#include "patient_data_types.h"
// 定义一个简单的数据结构来存储患者信息
//struct PatientData {
//    int id; // 唯一ID
//    QString name;
//    QString patientId;
//    int age;
//    QString department;
//    // ... 可以在此添加更多详细信息 ...
//    bool isInpatient;
//    bool isMedicalInsurance;
//    bool isKeyFocus;
//    // ... 等等
//};


class PatientListItemWidget : public QPushButton
{
Q_OBJECT

public:
    explicit PatientListItemWidget(const PatientData &data, QWidget *parent = nullptr);
    int getId() const;

private:
    PatientData patientData;
};

#endif // PATIENT_LIST_ITEM_WIDGET_H