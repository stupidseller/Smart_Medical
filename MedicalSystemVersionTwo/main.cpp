#include "src/ui/widgets/common/login_dialog.h"
#include "src/ui/widgets/widget.h"
#include "src/ui/widgets/patient/patient_main_window.h"
#include "src/ui/widgets/patient/medicine_search_widget.h"
#include "src/ui/widgets/patient/online_payment_widget.h"
#include "src/ui/widgets/patient/appointment_dialog.h"
#include "src/ui/widgets/patient/doctor_info_widget.h"
#include <QApplication>
#include <QMessageBox>
#include <QPointer>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    // 关键：避免“瞬间没有可见窗口时”自动退出
    QApplication::setQuitOnLastWindowClosed(false);

    // 1) TCP 客户端（常驻）
    Widget api(&a);
    api.on_connectServerBtn_clicked();

    // 2) 登录框（常驻，用 show/hide 切换）
    LoginDialog loginDialog;
    loginDialog.show();

    // 3) Main 窗口指针用 QPointer 防悬挂（对象被删后会自动变 null）
    QPointer<PatientMainWindow> mainWin = nullptr;

    // UI → TCP
    QObject::connect(&loginDialog, &LoginDialog::loginRequested,
                     &api,        &Widget::sendLoginData);
    QObject::connect(&loginDialog, &LoginDialog::registerRequested,
                     &api,        &Widget::sendRegisterData);

    // TCP → UI：登录成功
    QObject::connect(&api, &Widget::loginSucceededDetail, &loginDialog,
                     [&](int id, const QString &name, const QString &role)
    {
        // 如已有旧主窗，先安全销毁
        if (mainWin) { mainWin->disconnect(); mainWin->close(); mainWin->deleteLater(); mainWin = nullptr; }

        if (role == "patient") {
            // 注意：不设父对象，完全由我们手动管理生命周期，避免父子双重删除
            mainWin = new PatientMainWindow(&api, id, name);
            // 退出登录 → 回到登录框
            QObject::connect(mainWin, &PatientMainWindow::logoutRequested, &loginDialog, [&](){
                // 防重入/多次触发
                if (mainWin) {
                    mainWin->disconnect();
                    mainWin->close();
                    mainWin->deleteLater();
                    mainWin = nullptr;
                }
                // 回到登录页
                loginDialog.onLoginTabClicked();
                // 可选清空输入：若你在 LoginDialog 加了 clearFields()
                // loginDialog.clearFields();
                loginDialog.show(); loginDialog.raise(); loginDialog.activateWindow();
            });

            loginDialog.hide();      // 不要 accept()
            mainWin->show();
        } else if (role == "doctor") {
            QMessageBox::information(nullptr, "医生端占位", "医生端主界面待接入。");
        } else {
            QMessageBox::critical(nullptr, "错误", "未知角色");
        }
    });

    // 登录失败/注册结果（保持你原有逻辑）
    QObject::connect(&api, &Widget::loginFailed, &loginDialog,
                     [&](const QString &msg){
        QMessageBox::warning(&loginDialog, "登录失败",
                             msg.isEmpty() ? "请检查账号或密码" : msg);
    });
    QObject::connect(&api, &Widget::registerSucceeded, &loginDialog,
                     [&](const QString &msg){
        QMessageBox::information(&loginDialog, "注册成功",
                                 msg.isEmpty() ? "账户已创建，请登录" : msg);
        loginDialog.onLoginTabClicked();
    });
    QObject::connect(&api, &Widget::registerFailed, &loginDialog,
                     [&](const QString &msg){
        QMessageBox::warning(&loginDialog, "注册失败",
                             msg.isEmpty() ? "请检查注册信息" : msg);
    });
    //shang mian bu yunxu xiugai, yijing ok
    MedicineSearchWidget medWidget;

    // 1) Widget 想加载 -> 让 Api 去发请求
    QObject::connect(&medWidget, &MedicineSearchWidget::requestLoadMedicineData,
                     &api,      &Api::loadMedicineData);

    // 2) Api 收到服务器的 ok -> 喂给 Widget
    QObject::connect(&api,      &Api::loadMedicineDataOk,
                     &medWidget,&MedicineSearchWidget::onLoadMedicineDataOk);

    // （可选）建立 socket 连接
    // api.connectToServer(QHostAddress::LocalHost, 12345);

    medWidget.show();

    // 随界面启动就加载一次（或在按钮点击等时机调用）
    medWidget.loadMedicineData();

    // ///////////////////////////////////
    // 1) Widget 发起 -> Api 发送请求
    QObject::connect(&widget, &OnlinePaymentWidget::requestLoadOrderDetails,
                     &api,    [&api](){ api.loadOrderDetails(); });

    // 2) Api 收到服务器响应 -> 喂给 Widget
    QObject::connect(&api,    &Api::loadOrderDetailsOk,
                     &widget, &OnlinePaymentWidget::onLoadOrderDetailsOk);
    widget.show();

    // 启动即加载（也可改成按钮触发）
    widget.loadOrderDetails();
    // ////////////////////////////////
    AppointmentBookingWidget widget;

    // 1) Widget 请求 -> Api 发送
    QObject::connect(&widget, &AppointmentBookingWidget::requestLoadAvailableDoctors,
                     &api,    &Api::loadAvailableDoctors);

    // 2) Api 收到响应 -> 喂给 Widget
    QObject::connect(&api,    &Api::loadAvailableDoctorsOk,
                     &widget, &AppointmentBookingWidget::onLoadAvailableDoctorsOk);

    // （可选）连接服务器
    // api.connectToServer(QHostAddress::LocalHost, 12345);

    widget.show();

    // 启动即加载（也可做成按钮）
    widget.loadAvailableDoctors();
    // //////////////////////////////////
    AppointmentDialog dlg;

        // 1) appointment dialog 发起 -> Api 发送
        QObject::connect(&dlg, &AppointmentDialog::submitAppointmentRequest,
                         &api, &Api::submitAppointmentRequest);

        // 2) Api 收到服务器响应 -> 喂回 appointment dialog
        QObject::connect(&api, &Api::submitAppointmentRequestOk,
                         &dlg, &AppointmentDialog::onSubmitAppointmentRequestOk);

        // （可选）连接到服务器
        // api.connectToServer(QHostAddress::LocalHost, 12345);

        dlg.show();
        // /////////////////////////////
        DoctorInfoWidget widget;

            // 1) widget 发起加载 -> Api 发送请求
            QObject::connect(&widget, &DoctorInfoWidget::requestLoadDoctorList,
                             &api,    &Api::loadDoctorList);

            // 2) Api 收到响应 -> 喂给 widget
            QObject::connect(&api,    &Api::loadDoctorListOk,
                             &widget, &DoctorInfoWidget::onLoadDoctorListOk);

            // （可选）连接服务器
            // api.connectToServer(QHostAddress::LocalHost, 12345);

            widget.show();

            // 启动即加载（或你用按钮触发）
            widget.loadDoctorList();
     // ////////////////////////////
            MedicineSearchWidget widget;

                // 1) medicine search widget 发起 -> Api 发送
                QObject::connect(&widget, &MedicineSearchWidget::onPurchaseClicked,
                                 &api,    &Api::onPurchaseClicked);

                // 2) Api 收到响应 -> 喂回 medicine search widget
                QObject::connect(&api,    &Api::onPurchaseClickedOk,
                                 &widget, &MedicineSearchWidget::onPurchaseClickedOk);

                // （可选）连接服务器
                // api.connectToServer(QHostAddress::LocalHost, 12345);

                widget.show();
    // ///////////////////////////////
                OnlinePaymentWidget widget;

                    // 1) online payment widget 发起 -> Api 发送
                    QObject::connect(&widget, &OnlinePaymentWidget::processPayment,
                                     &api,    &Api::processPayment);

                    // 2) Api 收到响应 -> 喂回 online payment widget
                    QObject::connect(&api,    &Api::processPaymentOk,
                                     &widget, &OnlinePaymentWidget::onProcessPaymentOk);

                    // （可选）连接服务器
                    // api.connectToServer(QHostAddress::LocalHost, 12345);

                    widget.show();
    return a.exec();
}
