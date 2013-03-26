#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>

namespace Ui {
class LoginForm;
}

#ifdef TESTS
class TestLoginForm;
#endif

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = 0);
    ~LoginForm();

    void setSessionState(bool value);

signals:
    void newCustomerSelected(QString email, QString password);
    void loginCheckout(QString email, QString password);

private slots:
    void validateForm();
    void on_btnProceed_clicked();
    void on_lineEmail_returnPressed();
    void on_linePassword_returnPressed();
    void showEmailWarning();
    void showPasswordWarning();

private:
    Ui::LoginForm *ui;
    bool _sessionActive;

    bool checkEmail();
    bool checkPassword();

#ifdef TESTS
    friend class TestLoginForm;
#endif
};

#endif // LOGINFORM_H
