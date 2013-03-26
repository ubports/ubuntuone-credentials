#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QWidget>

namespace Ui {
class RegisterForm;
}

#ifdef TESTS
class TestRegisterForm;
#endif

class RegisterForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterForm(QWidget *parent = 0);
    ~RegisterForm();

    void setEmail(QString email);
    void setPassword(QString password);
    void setSessionState(bool value);

signals:
    void goBack();
    void registerCheckout(QString email, QString password, QString name);

private slots:
    void validateForm();
    void on_btnBack_clicked();
    void on_btnCheckout_clicked();
    void on_lineEmail_returnPressed();
    void on_lineName_returnPressed();
    void on_linePassword_returnPressed();
    void on_lineConfirm_returnPressed();
    void showNameWarning();
    void showPasswordWarning();
    void showConfirmWarning();

private:
    Ui::RegisterForm *ui;
    bool _sessionActive;

    bool checkEmail();
    bool checkName();
    bool checkPassword();
    bool checkConfirmPassword();

#ifdef TESTS
    friend class TestRegisterForm;
#endif
};

#endif // REGISTERFORM_H
