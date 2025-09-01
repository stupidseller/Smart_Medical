#ifndef PAYMENT_SUCCESS_DIALOG_H
#define PAYMENT_SUCCESS_DIALOG_H

#include <QDialog>
#include <QString>

class PaymentSuccessDialog : public QDialog
{
Q_OBJECT

public:
    explicit PaymentSuccessDialog(double paymentAmount, QWidget *parent = nullptr);
    ~PaymentSuccessDialog();

signals:
    void viewOrderClicked(); // 查看订单按钮被点击
    void backToDashboardClicked(); // 返回首页按钮被点击

private slots:
    void onViewOrder();
    void onBackToDashboard();

private:
    void initUI(double paymentAmount);
    void initStyleSheets();
};

#endif // PAYMENT_SUCCESS_DIALOG_H