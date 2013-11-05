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
#include "responses.h"
#include "errormessages.h"

TestRegisterForm::TestRegisterForm(QObject *parent) :
    QObject(parent)
{
}

void TestRegisterForm::init()
{
    _goBackEmitted = false;
    _registerCheckoutEmitted = false;
    registerForm._sessionActive = true;
    registerForm.ui->lineEmail->setProperty("error", false);
    registerForm.ui->linePassword->setProperty("error", false);
    registerForm.ui->lineEmail->setText("");
    registerForm.ui->linePassword->setText("");
    registerForm.ui->lineName->setText("");
    registerForm.ui->lineConfirm->setText("");
    registerForm.ui->btnCheckout->setEnabled(false);
    registerForm.ui->lblNameError->setVisible(false);
    registerForm.ui->lblPasswordError->setVisible(false);
    registerForm.ui->lblConfirmError->setVisible(false);
}

void TestRegisterForm::cleanup()
{
    registerForm.hide();
}

void TestRegisterForm::testSetEmail()
{
    QString email = "mail@ubuntu.com";
    registerForm.setEmail(email);
    QCOMPARE(registerForm.ui->lineEmail->text(), email);
}

void TestRegisterForm::testSetPassword()
{
    QString password = "password";
    registerForm.setPassword(password);
    QCOMPARE(registerForm.ui->linePassword->text(), password);
}

void TestRegisterForm::testCheckEmail()
{
    registerForm.ui->lineEmail->setText("mail");
    QVERIFY(!registerForm.checkEmail());
    registerForm.ui->lineEmail->setText("mail@ubuntu.com");
    QVERIFY(registerForm.checkEmail());
}

void TestRegisterForm::testCheckPassword()
{
    registerForm.ui->linePassword->setText("pass");
    QVERIFY(!registerForm.checkPassword());
    registerForm.ui->linePassword->setText("password");
    QVERIFY(registerForm.checkPassword());
}

void TestRegisterForm::testCheckConfirmPassword()
{
    registerForm.ui->linePassword->setText("password");
    registerForm.ui->lineConfirm->setText("");
    QVERIFY(!registerForm.checkConfirmPassword());
    registerForm.ui->lineConfirm->setText("password");
    QVERIFY(registerForm.checkConfirmPassword());
}

void TestRegisterForm::testCheckName()
{
    registerForm.ui->lineName->setText("");
    QVERIFY(!registerForm.checkName());
    registerForm.ui->lineName->setText("My Name");
    QVERIFY(registerForm.checkName());
}

void TestRegisterForm::testValidateForm()
{
    QVERIFY(!registerForm.ui->btnCheckout->isEnabled());
    registerForm.ui->lineEmail->setText("mail@ubuntu.com");
    QVERIFY(!registerForm.ui->btnCheckout->isEnabled());
    registerForm.ui->linePassword->setText("password");
    QVERIFY(!registerForm.ui->btnCheckout->isEnabled());
    registerForm.ui->lineName->setText("My Name");
    QVERIFY(!registerForm.ui->btnCheckout->isEnabled());
    registerForm.ui->checkBox->setChecked(true);
    registerForm.ui->lineConfirm->setText("password");
    QVERIFY(registerForm.ui->btnCheckout->isEnabled());
}

void TestRegisterForm::testBackPressed()
{
    QVERIFY(!_goBackEmitted);
    QObject::connect(&(registerForm), SIGNAL(goBack()), this, SLOT(receiveGoBack()));
    registerForm.ui->btnBack->clicked();
    QVERIFY(_goBackEmitted);
}

void TestRegisterForm::receiveGoBack()
{
    _goBackEmitted = true;
}

void TestRegisterForm::testCheckoutPressed()
{
    QVERIFY(!_registerCheckoutEmitted);
    QObject::connect(&(registerForm), SIGNAL(registerCheckout(QString,QString,QString)),
                     this, SLOT(receiveCheckout()));
    registerForm.ui->lineEmail->setText("mail@ubuntu.com");
    registerForm.ui->linePassword->setText("password");
    registerForm.ui->lineName->setText("My Name");
    registerForm.ui->lineConfirm->setText("password");
    registerForm.ui->checkBox->setChecked(true);
    registerForm.ui->btnCheckout->clicked();
    QVERIFY(_registerCheckoutEmitted);
}

void TestRegisterForm::testShowErrorTipsAlreadyRegistered()
{
    QVERIFY(!registerForm.ui->lineEmail->property("error").toBool());
    ErrorResponse error(0, "", ALREADY_REGISTERED, "");
    registerForm.showErrorTips(error);
    QVERIFY(registerForm.ui->lineEmail->property("error").toBool());
}

void TestRegisterForm::testShowErrorTipsEmailInvalidated()
{
    QVERIFY(!registerForm.ui->lineEmail->property("error").toBool());
    ErrorResponse error(0, "", EMAIL_INVALIDATED, "");
    registerForm.showErrorTips(error);
    QVERIFY(registerForm.ui->lineEmail->property("error").toBool());
}

void TestRegisterForm::testShowErrorTipsInvalidCredentials()
{
    QVERIFY(!registerForm.ui->lineEmail->property("error").toBool());
    QVERIFY(!registerForm.ui->linePassword->property("error").toBool());
    ErrorResponse error(0, "", INVALID_CREDENTIALS, "");
    registerForm.showErrorTips(error);
    QVERIFY(registerForm.ui->linePassword->property("error").toBool());
}

void TestRegisterForm::receiveCheckout()
{
    _registerCheckoutEmitted = true;
}

void TestRegisterForm::testLineConfirmPasswordReturnPressed()
{
    registerForm.show();
    registerForm.ui->btnCheckout->setEnabled(true);
    registerForm.ui->lineConfirm->setFocus();
    QVERIFY(!_registerCheckoutEmitted);
    registerForm.ui->lineConfirm->returnPressed();
    QVERIFY(_registerCheckoutEmitted);
}
