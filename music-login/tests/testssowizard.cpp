#include "testssowizard.h"
#include <QtTest/QtTest>
#include "ui_ssowizard.h"

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
    this->wizard.showError("test error");
    QCOMPARE(this->wizard.ui->lblError->text(), QString("test error"));
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
    QCOMPARE(this->wizard.ui->lblError->text(), QString("service failded"));
    QCOMPARE(this->wizard.ui->lblError->isEnabled(), true);
}

void TestSSOWizard::testLoginFailed()
{
    this->wizard.show();
    this->wizard._service.loginFailed("login failed");
    QCOMPARE(this->wizard._overlay->isVisible(), false);
    QCOMPARE(this->wizard.ui->lblError->text(), QString("login failed"));
    QCOMPARE(this->wizard.ui->lblError->isEnabled(), true);
}
