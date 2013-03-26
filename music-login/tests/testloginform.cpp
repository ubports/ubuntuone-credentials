#include "testloginform.h"
#include "ui_loginform.h"
#include <QtTest/QtTest>
#include <QDebug>

TestLoginForm::TestLoginForm(QObject *parent) :
    QObject(parent)
{
}

void TestLoginForm::init()
{
    this->loginForm._sessionActive = true;
    this->_newCustomerEmitted = false;
    this->_loginCheckoutEmitted = false;
    this->loginForm.ui->lineEmail->setText("");
    this->loginForm.ui->linePassword->setText("");
    this->loginForm.ui->btnProceed->setEnabled(false);
    this->loginForm.ui->lblEmailError->setVisible(false);
    this->loginForm.ui->lblPasswordError->setVisible(false);
}

void TestLoginForm::cleanup()
{
    this->loginForm.hide();
}

void TestLoginForm::testCheckEmail()
{
    this->loginForm.ui->lineEmail->setText("mail");
    QCOMPARE(this->loginForm.checkEmail(), false);
    this->loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    QCOMPARE(this->loginForm.checkEmail(), true);
}

void TestLoginForm::testCheckPassword()
{
    this->loginForm.ui->linePassword->setText("pass");
    QCOMPARE(this->loginForm.checkPassword(), false);
    this->loginForm.ui->linePassword->setText("password");
    QCOMPARE(this->loginForm.checkPassword(), true);
}

void TestLoginForm::testValidateForm()
{
    QCOMPARE(this->loginForm.ui->btnProceed->isEnabled(), false);
    this->loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    QCOMPARE(this->loginForm.ui->btnProceed->isEnabled(), false);
    this->loginForm.ui->linePassword->setText("password");
    QCOMPARE(this->loginForm.ui->btnProceed->isEnabled(), true);
}

void TestLoginForm::testCheckEmailWarnings()
{
    this->loginForm.show();
    QCOMPARE(this->loginForm.ui->lblEmailError->isVisible(), false);
    this->loginForm.ui->lineEmail->setText("mail");
    this->loginForm.ui->lineEmail->editingFinished();
    QCOMPARE(this->loginForm.ui->lblEmailError->isVisible(), true);
    this->loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    this->loginForm.ui->lineEmail->editingFinished();
    QCOMPARE(this->loginForm.ui->lblEmailError->isVisible(), false);
}

void TestLoginForm::testCheckPasswordWarnings()
{
    this->loginForm.show();
    QCOMPARE(this->loginForm.ui->lblPasswordError->isVisible(), false);
    this->loginForm.ui->linePassword->setText("pass");
    this->loginForm.ui->linePassword->editingFinished();
    QCOMPARE(this->loginForm.ui->lblPasswordError->isVisible(), true);
    this->loginForm.ui->linePassword->setText("password");
    this->loginForm.ui->linePassword->editingFinished();
    QCOMPARE(this->loginForm.ui->lblPasswordError->isVisible(), false);
}

void TestLoginForm::testButtonProceed()
{
    QCOMPARE(this->_newCustomerEmitted, false);
    QCOMPARE(this->_loginCheckoutEmitted, false);
    QObject::connect(&(this->loginForm), SIGNAL(newCustomerSelected(QString, QString)),
                     this, SLOT(receiveCustomerSignal()));
    QObject::connect(&(this->loginForm), SIGNAL(loginCheckout(QString, QString)),
                     this, SLOT(receiveLoginSignal()));
    this->loginForm.ui->btnProceed->setEnabled(true);
    this->loginForm.ui->radioNewCustomer->setChecked(true);
    this->loginForm.ui->btnProceed->clicked();
    QCOMPARE(this->_newCustomerEmitted, true);
    QCOMPARE(this->_loginCheckoutEmitted, false);
    this->loginForm.ui->radioReturningCustomer->setChecked(true);
    this->loginForm.ui->btnProceed->clicked();
    QCOMPARE(this->_newCustomerEmitted, true);
    QCOMPARE(this->_loginCheckoutEmitted, true);
}

void TestLoginForm::receiveCustomerSignal()
{
    this->_newCustomerEmitted = true;
}

void TestLoginForm::receiveLoginSignal()
{
    this->_loginCheckoutEmitted = true;
}
