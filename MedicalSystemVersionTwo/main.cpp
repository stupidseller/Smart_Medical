#include "src/ui/widgets/common/login_dialog.h"
#include "src/ui/widgets/widget.h"
#include "src/ui/widgets/patient/patient_main_window.h"
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

    return a.exec();
}
