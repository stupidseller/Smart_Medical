#include "src/ui/widgets/common/login_dialog.h"
#include "src/ui/widgets/widget.h"
#include "src/ui/widgets/patient/patient_main_window.h"
#include <QApplication>
#include <QMessageBox>
#include <QCoreApplication>

int main(int argc, char *argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    Widget api(&a);
    api.on_connectServerBtn_clicked(); // 可选：开机连

    // 2) 创建登录对话框
    LoginDialog loginDialog;

    // 3)  UI → TCP | 连接信号 → 槽（这一步就是让 emit “调用东西”）
    QObject::connect(&loginDialog, &LoginDialog::loginRequested,
                     &api, &Widget::sendLoginData);
    QObject::connect(&loginDialog, &LoginDialog::registerRequested,
                     &api, &Widget::sendRegisterData);

    // 登录成功信息缓存（供 exec() 返回后使用）
    int loggedId = -1;
    QString loggedName, loggedRole;

    // TCP → UI（占位处理：弹框 + 可选 accept()）
    QObject::connect(&api, &Widget::loginSucceededDetail, &loginDialog,
                     [&](int id, const QString &name, const QString &role){
        loggedId = id; loggedName = name; loggedRole = role;
        QMessageBox::information(&loginDialog, "登录成功",
                                 name.isEmpty() ? "欢迎回来！" : ("欢迎回来，" + name));
        loginDialog.accept(); // 关闭对话框
    });


    QObject::connect(&api, &Widget::loginFailed, &loginDialog,
                     [&](const QString &msg){
        QMessageBox::warning(&loginDialog, "登录失败",
                             msg.isEmpty() ? "请检查账号或密码" : msg);
    });

    QObject::connect(&api, &Widget::registerSucceeded, &loginDialog,
                     [&](const QString &msg){
        QMessageBox::information(&loginDialog, "注册成功",
                                 msg.isEmpty() ? "账户已创建，请登录" : msg);
        // 注册成功 → 回到登录页，但不关闭对话框
        loginDialog.onLoginTabClicked();
    });
    QObject::connect(&api, &Widget::registerFailed, &loginDialog,
                     [&](const QString &msg){
        QMessageBox::warning(&loginDialog, "注册失败",
                             msg.isEmpty() ? "请检查注册信息" : msg);
    });


    // 进入登录框（模态）
    if (loginDialog.exec() == QDialog::Accepted) {
        if (loggedRole == "patient") {
            auto *mainWin = new PatientMainWindow(&api, loggedId, loggedName);
            QObject::connect(mainWin, &PatientMainWindow::logoutRequested, [&](){
                mainWin->close();
                loggedId = -1; loggedName.clear(); loggedRole.clear();
                loginDialog.show();
                loginDialog.raise();
                loginDialog.activateWindow();
            });
            mainWin->show();
            return a.exec();
        } else if (loggedRole == "doctor") {
            // DoctorMainWindow 同理
            // auto *w = new DoctorMainWindow(&api, loggedId, loggedName);
            // ...
            // return a.exec();
            QMessageBox::information(nullptr, "医生端占位", "医生端主界面待接入。");
            return 0;
        } else {
            QMessageBox::critical(nullptr, "错误", "未知角色");
            return 0;
        }
    }
    return 0;
}
