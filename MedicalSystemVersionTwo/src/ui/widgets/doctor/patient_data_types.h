#ifndef PATIENT_DATA_TYPES_H
#define PATIENT_DATA_TYPES_H

#include <QString>

// 患者数据结构 - 统一定义，避免重复
struct PatientData {
    int id;
    QString name;
    QString patientId;
    int age;
    QString department;
    bool isInpatient;
    bool isMedicalInsurance;
    bool isKeyFocus;

    // 构造函数
    PatientData() : id(0), age(0), isInpatient(false), isMedicalInsurance(false), isKeyFocus(false) {}

    PatientData(int id, const QString& name, const QString& patientId, int age,
                const QString& department, bool isInpatient, bool isMedicalInsurance, bool isKeyFocus)
            : id(id), name(name), patientId(patientId), age(age), department(department),
              isInpatient(isInpatient), isMedicalInsurance(isMedicalInsurance), isKeyFocus(isKeyFocus) {}
};

#endif // PATIENT_DATA_TYPES_H