#include "src/ui/widgets/common/login_dialog.h"
#include "src/ui/widgets/widget.h"
#include "src/ui/widgets/patient/patient_main_window.h"
#include "src/ui/widgets/doctor/doctor_main_window.h"
// main 里不要再把每个页面都 new 出来；页面在 PatientMainWindow 里按需创建
#include <QApplication>
#include <QMessageBox>
#include <QPointer>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    QApplication::setQuitOnLastWindowClosed(false);

    // 1) TCP 客户端（常驻）
    Widget api(&a);
    api.on_connectServerBtn_clicked();

    // 2) 登录框（常驻，用 show/hide 切换）
    LoginDialog loginDialog;
    loginDialog.show();

    // 3) Main 窗口指针用 QPointer 防悬挂
    QPointer<QWidget> mainWin = nullptr;

    // UI → TCP
    QObject::connect(&loginDialog, &LoginDialog::loginRequested,
                     &api,        &Widget::sendLoginData);
    QObject::connect(&loginDialog, &LoginDialog::registerRequested,
                     &api,        &Widget::sendRegisterData);

    // TCP → UI：登录成功
    QObject::connect(&api, &Widget::loginSucceededDetail, &loginDialog,
                     [&](int id, const QString &name, const QString &role)
    {
        if (mainWin) { mainWin->disconnect(); mainWin->close(); mainWin->deleteLater(); mainWin = nullptr; }

        if (role == "patient") {
            auto *win = new PatientMainWindow(&api, id, name);
            mainWin = win;
            QObject::connect(win, &PatientMainWindow::logoutRequested, &loginDialog, [&](){
                if (mainWin) { mainWin->disconnect(); mainWin->close(); mainWin->deleteLater(); mainWin = nullptr; }
                loginDialog.onLoginTabClicked(); loginDialog.show(); loginDialog.raise(); loginDialog.activateWindow();
            });
            loginDialog.hide(); win->show();

        } else if (role == "doctor") {
            auto *win = new DoctorMainWindow(&api, id, name);
            mainWin = win;
            QObject::connect(win, &DoctorMainWindow::logoutRequested, &loginDialog, [&](){
                if (mainWin) { mainWin->disconnect(); mainWin->close(); mainWin->deleteLater(); mainWin = nullptr; }
                loginDialog.onLoginTabClicked(); loginDialog.show(); loginDialog.raise(); loginDialog.activateWindow();
            });
            loginDialog.hide(); win->show();

            } else {
                QMessageBox::critical(nullptr, "错误", "未知角色");
            }
            });


    // 登录失败/注册结果
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
