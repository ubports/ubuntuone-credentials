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
