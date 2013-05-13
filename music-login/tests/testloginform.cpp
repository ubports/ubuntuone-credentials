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
#include <sso_api/responses.h>
#include "error_messages.h"

TestLoginForm::TestLoginForm(QObject *parent) :
    QObject(parent)
{
}

void TestLoginForm::init()
{
    this->loginForm._sessionActive = true;
    this->_newCustomerEmitted = false;
    this->_loginCheckoutEmitted = false;
    this->loginForm.ui->lineEmail->setProperty("error", false);
    this->loginForm.ui->linePassword->setProperty("error", false);
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

void TestLoginForm::testButtonText()
{
    QCOMPARE(this->loginForm.ui->btnProceed->text(), QString("Proceed to Checkout"));
    this->loginForm.ui->radioNewCustomer->clicked();
    QCOMPARE(this->loginForm.ui->btnProceed->text(), QString("Continue"));
    this->loginForm.ui->radioReturningCustomer->clicked();
    QCOMPARE(this->loginForm.ui->btnProceed->text(), QString("Proceed to Checkout"));
}

void TestLoginForm::testCheckEmail()
{
    this->loginForm.ui->lineEmail->setText("mail");
    QVERIFY(!this->loginForm.checkEmail());
    this->loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    QVERIFY(this->loginForm.checkEmail());
}

void TestLoginForm::testCheckPassword()
{
    this->loginForm.ui->linePassword->setText("pass");
    QVERIFY(!this->loginForm.checkPassword());
    this->loginForm.ui->linePassword->setText("password");
    QVERIFY(this->loginForm.checkPassword());
}

void TestLoginForm::testValidateForm()
{
    QVERIFY(!this->loginForm.ui->btnProceed->isEnabled());
    this->loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    QVERIFY(!this->loginForm.ui->btnProceed->isEnabled());
    this->loginForm.ui->linePassword->setText("password");
    QVERIFY(this->loginForm.ui->btnProceed->isEnabled());
}

void TestLoginForm::testCheckEmailWarnings()
{
    this->loginForm.show();
    QVERIFY(!this->loginForm.ui->lblEmailError->isVisible());
    this->loginForm.ui->lineEmail->setText("mail");
    this->loginForm.ui->lineEmail->editingFinished();
    QVERIFY(this->loginForm.ui->lblEmailError->isVisible());
    this->loginForm.ui->lineEmail->setText("mail@ubuntu.com");
    this->loginForm.ui->lineEmail->editingFinished();
    QVERIFY(!this->loginForm.ui->lblEmailError->isVisible());
}

void TestLoginForm::testCheckPasswordWarnings()
{
    this->loginForm.show();
    QVERIFY(!this->loginForm.ui->lblPasswordError->isVisible());
    this->loginForm.ui->linePassword->setText("pass");
    this->loginForm.ui->linePassword->editingFinished();
    QVERIFY(this->loginForm.ui->lblPasswordError->isVisible());
    this->loginForm.ui->linePassword->setText("password");
    this->loginForm.ui->linePassword->editingFinished();
    QVERIFY(!this->loginForm.ui->lblPasswordError->isVisible());
}

void TestLoginForm::testButtonProceed()
{
    QVERIFY(!this->_newCustomerEmitted);
    QVERIFY(!this->_loginCheckoutEmitted);
    QObject::connect(&(this->loginForm), SIGNAL(newCustomerSelected(QString, QString)),
                     this, SLOT(receiveCustomerSignal()));
    QObject::connect(&(this->loginForm), SIGNAL(loginCheckout(QString, QString)),
                     this, SLOT(receiveLoginSignal()));
    this->loginForm.ui->btnProceed->setEnabled(true);
    this->loginForm.ui->radioNewCustomer->setChecked(true);
    this->loginForm.ui->btnProceed->clicked();
    QVERIFY(this->_newCustomerEmitted);
    QVERIFY(!this->_loginCheckoutEmitted);
    this->loginForm.ui->radioReturningCustomer->setChecked(true);
    this->loginForm.ui->btnProceed->clicked();
    QVERIFY(this->_newCustomerEmitted);
    QVERIFY(this->_loginCheckoutEmitted);
}

void TestLoginForm::testShowErrorTipsAlreadyRegistered()
{
    QVERIFY(!this->loginForm.ui->lineEmail->property("error").toBool());
    ErrorResponse error(0, "", CODE_ALREADY_REGISTERED, "");
    this->loginForm.showErrorTips(error);
    QVERIFY(this->loginForm.ui->lineEmail->property("error").toBool());
}

void TestLoginForm::testShowErrorTipsEmailInvalidated()
{
    QVERIFY(!this->loginForm.ui->lineEmail->property("error").toBool());
    ErrorResponse error(0, "", CODE_EMAIL_INVALIDATED, "");
    this->loginForm.showErrorTips(error);
    QVERIFY(this->loginForm.ui->lineEmail->property("error").toBool());
}

void TestLoginForm::testShowErrorTipsInvalidCredentials()
{
    QVERIFY(!this->loginForm.ui->lineEmail->property("error").toBool());
    QVERIFY(!this->loginForm.ui->linePassword->property("error").toBool());
    ErrorResponse error(0, "", CODE_INVALID_CREDENTIALS, "");
    this->loginForm.showErrorTips(error);
    QVERIFY(this->loginForm.ui->linePassword->property("error").toBool());
}

void TestLoginForm::receiveCustomerSignal()
{
    this->_newCustomerEmitted = true;
}

void TestLoginForm::receiveLoginSignal()
{
    this->_loginCheckoutEmitted = true;
}
