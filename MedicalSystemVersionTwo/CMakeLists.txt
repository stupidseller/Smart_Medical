cmake_minimum_required(VERSION 3.10)

# 项目名称
project(MedicalSystemVersionTwo)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-fexec-charset=UTF-8 -finput-charset=UTF-8)

# --- Qt 配置 - 开始 ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.12.8/5.12.8/mingw73_64")

# 寻找我们需要的 Qt 模块 (我们目前只需要 Widgets)
find_package(Qt5 COMPONENTS Widgets Network REQUIRED Core Svg)

# 添加可执行文件，并包含所有源文件
add_executable(${PROJECT_NAME}
        main.cpp
        src/ui/widgets/widget.h
        src/ui/widgets/widget.cpp
        src/ui/widgets/common/login_dialog.h
                src/ui/widgets/common/login_dialog.cpp
                src/ui/widgets/patient/patient_main_window.cpp
                src/ui/widgets/patient/patient_main_window.h
                src/ui/widgets/patient/profile_widget.cpp
                src/ui/widgets/patient/profile_widget.h
                src/ui/widgets/doctor/doctor_main_window.cpp
                src/ui/widgets/patient/appointment_dialog.h
                src/ui/widgets/patient/appointment_dialog.cpp
                src/ui/widgets/patient/appointment_booking_widget.h
                src/ui/widgets/patient/appointment_booking_widget.cpp
                src/ui/widgets/patient/doctor_info_widget.h
                src/ui/widgets/patient/doctor_info_widget.cpp
                src/ui/widgets/patient/communication_widget.h
                src/ui/widgets/patient/communication_widget.cpp
                src/ui/widgets/patient/health_assessment_widget.h
                src/ui/widgets/patient/health_assessment_widget.cpp
                src/ui/widgets/patient/medicine_search_widget.h
                src/ui/widgets/patient/medicine_search_widget.cpp
                src/ui/widgets/patient/payment_success_dialog.h
                src/ui/widgets/patient/payment_success_dialog.cpp
                src/ui/widgets/patient/online_payment_widget.h
                src/ui/widgets/patient/online_payment_widget.cpp

                src/ui/widgets/doctor/doctor_main_window.h
                src/ui/widgets/doctor/doctor_profile_widget.h
                src/ui/widgets/doctor/doctor_profile_widget.cpp
                src/ui/widgets/doctor/leave_record_item_widget.h
                src/ui/widgets/doctor/leave_record_item_widget.cpp
                src/ui/widgets/doctor/attendance_widget.h
                src/ui/widgets/doctor/attendance_widget.cpp
                src/ui/widgets/doctor/patient_management_widget.h
                src/ui/widgets/doctor/patient_management_widget.cpp
                src/ui/widgets/doctor/patient_list_item_widget.cpp
)

# 将我们的程序链接到 Qt 的 Widgets 模块
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network Qt5::Core Qt5::Svg)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/ui/widgets
)
