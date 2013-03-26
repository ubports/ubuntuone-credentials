#include "testregisterform.h"
#include <QtTest/QtTest>
#include "ui_registerform.h"

TestRegisterForm::TestRegisterForm(QObject *parent) :
    QObject(parent)
{
}

void TestRegisterForm::init()
{
    this->_goBackEmitted = false;
    this->_registerCheckoutEmitted = false;
    this->registerForm._sessionActive = true;
    this->registerForm.ui->lineEmail->setText("");
    this->registerForm.ui->linePassword->setText("");
    this->registerForm.ui->lineName->setText("");
    this->registerForm.ui->lineConfirm->setText("");
    this->registerForm.ui->btnCheckout->setEnabled(false);
    this->registerForm.ui->lblNameError->setVisible(false);
    this->registerForm.ui->lblPasswordError->setVisible(false);
    this->registerForm.ui->lblConfirmError->setVisible(false);
}

void TestRegisterForm::cleanup()
{
    this->registerForm.hide();
}

void TestRegisterForm::testSetEmail()
{
    QString email = "mail@ubuntu.com";
    this->registerForm.setEmail(email);
    QCOMPARE(this->registerForm.ui->lineEmail->text(), email);
}

void TestRegisterForm::testSetPassword()
{
    QString password = "password";
    this->registerForm.setPassword(password);
    QCOMPARE(this->registerForm.ui->linePassword->text(), password);
}

void TestRegisterForm::testCheckEmail()
{
    this->registerForm.ui->lineEmail->setText("mail");
    QCOMPARE(this->registerForm.checkEmail(), false);
    this->registerForm.ui->lineEmail->setText("mail@ubuntu.com");
    QCOMPARE(this->registerForm.checkEmail(), true);
}

void TestRegisterForm::testCheckPassword()
{
    this->registerForm.ui->linePassword->setText("pass");
    QCOMPARE(this->registerForm.checkPassword(), false);
    this->registerForm.ui->linePassword->setText("password");
    QCOMPARE(this->registerForm.checkPassword(), true);
}

void TestRegisterForm::testCheckConfirmPassword()
{
    this->registerForm.ui->linePassword->setText("password");
    this->registerForm.ui->lineConfirm->setText("");
    QCOMPARE(this->registerForm.checkConfirmPassword(), false);
    this->registerForm.ui->lineConfirm->setText("password");
    QCOMPARE(this->registerForm.checkConfirmPassword(), true);
}

void TestRegisterForm::testCheckName()
{
    this->registerForm.ui->lineName->setText("");
    QCOMPARE(this->registerForm.checkName(), false);
    this->registerForm.ui->lineName->setText("My Name");
    QCOMPARE(this->registerForm.checkName(), true);
}

void TestRegisterForm::testValidateForm()
{
    QCOMPARE(this->registerForm.ui->btnCheckout->isEnabled(), false);
    this->registerForm.ui->lineEmail->setText("mail@ubuntu.com");
    QCOMPARE(this->registerForm.ui->btnCheckout->isEnabled(), false);
    this->registerForm.ui->linePassword->setText("password");
    QCOMPARE(this->registerForm.ui->btnCheckout->isEnabled(), false);
    this->registerForm.ui->lineName->setText("My Name");
    QCOMPARE(this->registerForm.ui->btnCheckout->isEnabled(), false);
    this->registerForm.ui->checkBox->setChecked(true);
    this->registerForm.ui->lineConfirm->setText("password");
    QCOMPARE(this->registerForm.ui->btnCheckout->isEnabled(), true);
}

void TestRegisterForm::testBackPressed()
{
    QCOMPARE(this->_goBackEmitted, false);
    QObject::connect(&(this->registerForm), SIGNAL(goBack()), this, SLOT(receiveGoBack()));
    this->registerForm.ui->btnBack->clicked();
    QCOMPARE(this->_goBackEmitted, true);
}

void TestRegisterForm::receiveGoBack()
{
    this->_goBackEmitted = true;
}

void TestRegisterForm::testCheckoutPressed()
{
    QCOMPARE(this->_registerCheckoutEmitted, false);
    QObject::connect(&(this->registerForm), SIGNAL(registerCheckout(QString,QString,QString)),
                     this, SLOT(receiveCheckout()));
    this->registerForm.ui->lineEmail->setText("mail@ubuntu.com");
    this->registerForm.ui->linePassword->setText("password");
    this->registerForm.ui->lineName->setText("My Name");
    this->registerForm.ui->lineConfirm->setText("password");
    this->registerForm.ui->checkBox->setChecked(true);
    this->registerForm.ui->btnCheckout->clicked();
    QCOMPARE(this->_registerCheckoutEmitted, true);
}

void TestRegisterForm::receiveCheckout()
{
    this->_registerCheckoutEmitted = true;
}

void TestRegisterForm::testLineConfirmPasswordReturnPressed()
{
    this->registerForm.show();
    this->registerForm.ui->btnCheckout->setEnabled(true);
    this->registerForm.ui->lineConfirm->setFocus();
    QCOMPARE(this->_registerCheckoutEmitted, false);
    this->registerForm.ui->lineConfirm->returnPressed();
    QCOMPARE(this->_registerCheckoutEmitted, true);
}
