#ifndef TESTOGINFORM_H
#define TESTOGINFORM_H

#include <QObject>
#include "loginform.h"

class TestLoginForm : public QObject
{
    Q_OBJECT
public:
    explicit TestLoginForm(QObject *parent = 0);

public slots:
    void receiveCustomerSignal();
    void receiveLoginSignal();

private slots:
    void init();
    void testCheckEmail();
    void testCheckPassword();
    void testValidateForm();
    void testCheckEmailWarnings();
    void testCheckPasswordWarnings();
    void testButtonProceed();
    void cleanup();

private:
    LoginForm loginForm;
    bool _newCustomerEmitted;
    bool _loginCheckoutEmitted;
};

#endif // TESTOGINFORM_H
