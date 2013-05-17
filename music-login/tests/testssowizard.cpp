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

#include "testssowizard.h"
#include <QtTest/QtTest>
#include "ui_ssowizard.h"
#include <sso_api/responses.h>
#include "ssowizard.cpp"

TestSSOWizard::TestSSOWizard(QObject *parent) :
    QObject(parent)
{
}

void TestSSOWizard::cleanup()
{
    this->wizard.hide();
}

void TestSSOWizard::testCleanArgument()
{
    QString arg = "\"argument\"";
    QString cleanArg = this->wizard.cleanArgument(arg);
    QVERIFY(arg == cleanArg);
    QVERIFY(cleanArg != "\"argument\"");
    QCOMPARE(cleanArg, QString("argument"));
}

void TestSSOWizard::testShowError()
{
    ErrorResponse error(0, "", "LOGIN_FAILED", "");
    this->wizard.showError(error);
    QCOMPARE(this->wizard.ui->lblError->text(), QString(GENERAL_ERROR_MESSAGE));
    QCOMPARE(this->wizard.ui->lblError->isEnabled(), true);
}

void TestSSOWizard::testHideError()
{
    this->wizard.hideError();
    QCOMPARE(this->wizard.ui->lblError->text(), QString(""));
    QCOMPARE(this->wizard.ui->lblError->isEnabled(), false);
}

void TestSSOWizard::testShowRegisterPage()
{
    this->wizard.ui->pageLogin->newCustomerSelected("email", "password");
    QCOMPARE(this->wizard.ui->stackedWidget->currentIndex(), 1);
}

void TestSSOWizard::testLoginAndBuy()
{
    this->wizard.show();
    this->wizard.ui->pageLogin->loginCheckout("email", "password");
    QCOMPARE(this->wizard.ui->lblError->text(), QString(""));
    QCOMPARE(this->wizard.ui->lblError->isEnabled(), false);
    QCOMPARE(this->wizard._overlay->isVisible(), true);
}

void TestSSOWizard::testRegisterAndBuy()
{
    this->wizard.show();
    this->wizard.ui->pageRegister->registerCheckout("email", "password", "My Name");
    QCOMPARE(this->wizard.ui->lblError->text(), QString(""));
    QCOMPARE(this->wizard.ui->lblError->isEnabled(), false);
    QCOMPARE(this->wizard._overlay->isVisible(), true);
}

void TestSSOWizard::testShowPageLogin()
{
    this->wizard.ui->stackedWidget->setCurrentIndex(1);
    QCOMPARE(this->wizard.ui->stackedWidget->currentIndex(), 1);
    this->wizard.ui->pageRegister->goBack();
    QCOMPARE(this->wizard.ui->stackedWidget->currentIndex(), 0);
}

void TestSSOWizard::testAccountAuthenticated()
{
    this->wizard.show();
    QObject::connect(&(this->wizard), SIGNAL(aboutToClose()), this, SLOT(receiveAboutToClose()));
    this->_aboutToCloseEmitted = false;
    this->wizard.purchaseUrl = "";
    this->wizard._service.credentialsFound("", "", "", "", "");
    QCOMPARE(this->wizard._overlay->isVisible(), false);
    QCOMPARE(this->_aboutToCloseEmitted, true);
}

void TestSSOWizard::receiveAboutToClose()
{
    this->_aboutToCloseEmitted = true;
}

void TestSSOWizard::testServiceFailed()
{
    this->wizard.show();
    this->wizard._overlay->show();
    ErrorResponse error;
    error.message("service failded");
    this->wizard._service.requestFailed(error);
    QCOMPARE(this->wizard._overlay->isVisible(), false);
    QCOMPARE(this->wizard.ui->lblError->text(), QString(GENERAL_ERROR_MESSAGE));
    QCOMPARE(this->wizard.ui->lblError->isEnabled(), true);
}
