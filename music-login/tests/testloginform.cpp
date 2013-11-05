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

#include "testloginform.h"
#include "ui_loginform.h"
#include <QtTest/QtTest>
#include <QDebug>
#include "responses.h"
#include "errormessages.h"

TestLoginForm::TestLoginForm(QObject *parent) :
    QObject(parent)
{
}

void TestLoginForm::init()
{
    loginForm._sessionActive = true;
    _newCustomerEmitted = false;
    _loginCheckoutEmitted = false;
    loginForm.ui->lineEmail->setProperty("error", false);
    loginForm.ui->linePassword->setProperty("error", false);
    loginForm.ui->lineEmail->setText("");
    loginForm.ui->linePassword->setText("");
    loginForm.ui->btnProceed->setEnabled(false);
    loginForm.ui->lblEmailError->setVisible(false);
    loginForm.ui->lblPasswordError->setVisible(false);
    loginForm.ui->btnProceed->setText(QString("Proceed to Checkout"));
}

void TestLoginForm::cleanup()
{
    loginForm.hide();
}

void TestLoginForm::testButtonText()
{
    QCOMPARE(loginForm.ui->btnProceed->text(), QString("Proceed to Checkout"));
    loginForm.ui->radioNewCustomer->clicked();
    QCOMPARE(loginForm.ui->btnProceed->text(), QString("Continue"));
    loginForm.ui->radioReturningCustomer->clicked();
    QCOMPARE(loginForm.ui->btnProceed->text(), QString("Proceed to Checkout"));
}

void TestLoginForm::testCheckEmail()
{
    loginForm.ui->lineEmail->setText("mail");
    QVERIFY(!loginForm.checkEmail());
    loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    QVERIFY(loginForm.checkEmail());
}

void TestLoginForm::testCheckPassword()
{
    loginForm.ui->linePassword->setText("pass");
    QVERIFY(!loginForm.checkPassword());
    loginForm.ui->linePassword->setText("password");
    QVERIFY(loginForm.checkPassword());
}

void TestLoginForm::testValidateFormWithPassword()
{
    QVERIFY(!loginForm.ui->btnProceed->isEnabled());
    loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    QVERIFY(!loginForm.ui->btnProceed->isEnabled());
    loginForm.ui->linePassword->setText("password");
    QVERIFY(loginForm.ui->btnProceed->isEnabled());
}

void TestLoginForm::testValidateFormWithoutPassword()
{
    QVERIFY(!loginForm.ui->btnProceed->isEnabled());
    loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    QVERIFY(!loginForm.ui->btnProceed->isEnabled());
    loginForm.ui->radioNewCustomer->click();
    QVERIFY(loginForm.ui->btnProceed->isEnabled());
}

void TestLoginForm::testCheckEmailWarnings()
{
    loginForm.show();
    QVERIFY(!loginForm.ui->lblEmailError->isVisible());
    loginForm.ui->lineEmail->setText("mail");
    loginForm.ui->lineEmail->editingFinished();
    QVERIFY(loginForm.ui->lblEmailError->isVisible());
    loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    loginForm.ui->lineEmail->editingFinished();
    QVERIFY(!loginForm.ui->lblEmailError->isVisible());
}

void TestLoginForm::testCheckPasswordWarningsReturningCustomer()
{
    loginForm.show();
    loginForm.ui->radioReturningCustomer->setChecked(true);
    QVERIFY(!loginForm.ui->lblPasswordError->isVisible());
    loginForm.ui->linePassword->setText("pass");
    loginForm.ui->linePassword->editingFinished();
    QVERIFY(loginForm.ui->lblPasswordError->isVisible());
    loginForm.ui->linePassword->setText("password");
    loginForm.ui->linePassword->editingFinished();
    QVERIFY(!loginForm.ui->lblPasswordError->isVisible());
}

void TestLoginForm::testCheckPasswordWarningsNewCustomer()
{
    loginForm.show();
    loginForm.ui->radioNewCustomer->setChecked(true);
    QVERIFY(!loginForm.ui->lblPasswordError->isVisible());
    loginForm.ui->linePassword->setText("pass");
    loginForm.ui->linePassword->editingFinished();
    QVERIFY(!loginForm.ui->lblPasswordError->isVisible());
    loginForm.ui->linePassword->setText("password");
    loginForm.ui->linePassword->editingFinished();
    QVERIFY(!loginForm.ui->lblPasswordError->isVisible());
}

void TestLoginForm::testButtonProceed()
{
    QVERIFY(!_newCustomerEmitted);
    QVERIFY(!_loginCheckoutEmitted);
    QObject::connect(&(loginForm), SIGNAL(newCustomerSelected(QString, QString)),
                     this, SLOT(receiveCustomerSignal()));
    QObject::connect(&(loginForm), SIGNAL(loginCheckout(QString, QString)),
                     this, SLOT(receiveLoginSignal()));
    loginForm.ui->btnProceed->setEnabled(true);
    loginForm.ui->radioNewCustomer->setChecked(true);
    loginForm.ui->btnProceed->clicked();
    QVERIFY(_newCustomerEmitted);
    QVERIFY(!_loginCheckoutEmitted);
    loginForm.ui->radioReturningCustomer->setChecked(true);
    loginForm.ui->btnProceed->clicked();
    QVERIFY(_newCustomerEmitted);
    QVERIFY(_loginCheckoutEmitted);
}

void TestLoginForm::testShowErrorTipsAlreadyRegistered()
{
    QVERIFY(!loginForm.ui->lineEmail->property("error").toBool());
    ErrorResponse error(0, "", ALREADY_REGISTERED, "");
    loginForm.showErrorTips(error);
    QVERIFY(loginForm.ui->lineEmail->property("error").toBool());
}

void TestLoginForm::testShowErrorTipsEmailInvalidated()
{
    QVERIFY(!loginForm.ui->lineEmail->property("error").toBool());
    ErrorResponse error(0, "", EMAIL_INVALIDATED, "");
    loginForm.showErrorTips(error);
    QVERIFY(loginForm.ui->lineEmail->property("error").toBool());
}

void TestLoginForm::testShowErrorTipsInvalidCredentials()
{
    QVERIFY(!loginForm.ui->lineEmail->property("error").toBool());
    QVERIFY(!loginForm.ui->linePassword->property("error").toBool());
    ErrorResponse error(0, "", INVALID_CREDENTIALS, "");
    loginForm.showErrorTips(error);
    QVERIFY(loginForm.ui->linePassword->property("error").toBool());
}

void TestLoginForm::receiveCustomerSignal()
{
    _newCustomerEmitted = true;
}

void TestLoginForm::receiveLoginSignal()
{
    _loginCheckoutEmitted = true;
}
