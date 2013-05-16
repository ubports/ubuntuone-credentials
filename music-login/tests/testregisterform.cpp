/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include "testregisterform.h"
#include <QtTest/QtTest>
#include "ui_registerform.h"
#include <sso_api/responses.h>
#include "sso_api/errormessages.h"

TestRegisterForm::TestRegisterForm(QObject *parent) :
    QObject(parent)
{
}

void TestRegisterForm::init()
{
    this->_goBackEmitted = false;
    this->_registerCheckoutEmitted = false;
    this->registerForm._sessionActive = true;
    this->registerForm.ui->lineEmail->setProperty("error", false);
    this->registerForm.ui->linePassword->setProperty("error", false);
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
    QVERIFY(!this->registerForm.checkEmail());
    this->registerForm.ui->lineEmail->setText("mail@ubuntu.com");
    QVERIFY(this->registerForm.checkEmail());
}

void TestRegisterForm::testCheckPassword()
{
    this->registerForm.ui->linePassword->setText("pass");
    QVERIFY(!this->registerForm.checkPassword());
    this->registerForm.ui->linePassword->setText("password");
    QVERIFY(this->registerForm.checkPassword());
}

void TestRegisterForm::testCheckConfirmPassword()
{
    this->registerForm.ui->linePassword->setText("password");
    this->registerForm.ui->lineConfirm->setText("");
    QVERIFY(!this->registerForm.checkConfirmPassword());
    this->registerForm.ui->lineConfirm->setText("password");
    QVERIFY(this->registerForm.checkConfirmPassword());
}

void TestRegisterForm::testCheckName()
{
    this->registerForm.ui->lineName->setText("");
    QVERIFY(!this->registerForm.checkName());
    this->registerForm.ui->lineName->setText("My Name");
    QVERIFY(this->registerForm.checkName());
}

void TestRegisterForm::testValidateForm()
{
    QVERIFY(!this->registerForm.ui->btnCheckout->isEnabled());
    this->registerForm.ui->lineEmail->setText("mail@ubuntu.com");
    QVERIFY(!this->registerForm.ui->btnCheckout->isEnabled());
    this->registerForm.ui->linePassword->setText("password");
    QVERIFY(!this->registerForm.ui->btnCheckout->isEnabled());
    this->registerForm.ui->lineName->setText("My Name");
    QVERIFY(!this->registerForm.ui->btnCheckout->isEnabled());
    this->registerForm.ui->checkBox->setChecked(true);
    this->registerForm.ui->lineConfirm->setText("password");
    QVERIFY(this->registerForm.ui->btnCheckout->isEnabled());
}

void TestRegisterForm::testBackPressed()
{
    QVERIFY(!this->_goBackEmitted);
    QObject::connect(&(this->registerForm), SIGNAL(goBack()), this, SLOT(receiveGoBack()));
    this->registerForm.ui->btnBack->clicked();
    QVERIFY(this->_goBackEmitted);
}

void TestRegisterForm::receiveGoBack()
{
    this->_goBackEmitted = true;
}

void TestRegisterForm::testCheckoutPressed()
{
    QVERIFY(!this->_registerCheckoutEmitted);
    QObject::connect(&(this->registerForm), SIGNAL(registerCheckout(QString,QString,QString)),
                     this, SLOT(receiveCheckout()));
    this->registerForm.ui->lineEmail->setText("mail@ubuntu.com");
    this->registerForm.ui->linePassword->setText("password");
    this->registerForm.ui->lineName->setText("My Name");
    this->registerForm.ui->lineConfirm->setText("password");
    this->registerForm.ui->checkBox->setChecked(true);
    this->registerForm.ui->btnCheckout->clicked();
    QVERIFY(this->_registerCheckoutEmitted);
}

void TestRegisterForm::testShowErrorTipsAlreadyRegistered()
{
    QVERIFY(!this->registerForm.ui->lineEmail->property("error").toBool());
    ErrorResponse error(0, "", ALREADY_REGISTERED, "");
    this->registerForm.showErrorTips(error);
    QVERIFY(this->registerForm.ui->lineEmail->property("error").toBool());
}

void TestRegisterForm::testShowErrorTipsEmailInvalidated()
{
    QVERIFY(!this->registerForm.ui->lineEmail->property("error").toBool());
    ErrorResponse error(0, "", EMAIL_INVALIDATED, "");
    this->registerForm.showErrorTips(error);
    QVERIFY(this->registerForm.ui->lineEmail->property("error").toBool());
}

void TestRegisterForm::testShowErrorTipsInvalidCredentials()
{
    QVERIFY(!this->registerForm.ui->lineEmail->property("error").toBool());
    QVERIFY(!this->registerForm.ui->linePassword->property("error").toBool());
    ErrorResponse error(0, "", INVALID_CREDENTIALS, "");
    this->registerForm.showErrorTips(error);
    QVERIFY(this->registerForm.ui->linePassword->property("error").toBool());
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
    QVERIFY(!this->_registerCheckoutEmitted);
    this->registerForm.ui->lineConfirm->returnPressed();
    QVERIFY(this->_registerCheckoutEmitted);
}
