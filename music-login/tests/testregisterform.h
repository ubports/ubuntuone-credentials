#ifndef TESTREGISTERFORM_H
#define TESTREGISTERFORM_H

#include <QObject>
#include "registerform.h"

class TestRegisterForm : public QObject
{
    Q_OBJECT
public:
    explicit TestRegisterForm(QObject *parent = 0);

public slots:
    void receiveGoBack();
    void receiveCheckout();

private slots:
    void init();
    void testSetEmail();
    void testSetPassword();
    void testCheckEmail();
    void testCheckPassword();
    void testCheckName();
    void testCheckConfirmPassword();
    void testValidateForm();
    void testBackPressed();
    void testCheckoutPressed();
    void testLineConfirmPasswordReturnPressed();
    void cleanup();

private:
    RegisterForm registerForm;
    bool _goBackEmitted;
    bool _registerCheckoutEmitted;

};

#endif // TESTREGISTERFORM_H
