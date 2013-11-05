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

#include "ssowizard.h"
#include "ui_ssowizard.h"
#include <QStringList>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QResizeEvent>
#include "errormessages.h"

#define GENERAL_ERROR_MESSAGE "We apologize, but we cannot process your request right now. Please try again later."

SSOWizard::SSOWizard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SSOWizard)
{
    ui->setupUi(this);
    _overlay = new LoadingOverlay(this);
    _overlay->hide();
    ui->lblError->setEnabled(false);

    QStringList cmdline_args = QCoreApplication::arguments();

    QString item;
    QString album;
    QString artist;
    QString price;
    QString picture;
    foreach(item, cmdline_args){
        if(item.startsWith("--album=")){
            item.remove(0, 8);
            album = cleanArgument(item);
        }else if(item.startsWith("--artist=")){
            item.remove(0, 9);
            artist = cleanArgument(item);
        }else if(item.startsWith("--price=")){
            item.remove(0, 8);
            price = cleanArgument(item);
        }else if(item.startsWith("--picture=")){
            item.remove(0, 10);
            picture = cleanArgument(item);
        }else if(item.startsWith("--url=")){
            item.remove(0, 6);
            purchaseUrl = cleanArgument(item);
        }
    }

    setHeader(artist, album, price);
    ui->pageLogin->setSessionState(true);
    ui->pageRegister->setSessionState(true);

    // Connect signals
    QObject::connect(&downloader, SIGNAL(fileDownloaded(QString&)),
                     this, SLOT(imageDownloaded(QString&)));

    QObject::connect(&_service, SIGNAL(credentialsStored()),
                     this, SLOT(accountAuthenticated()));
    QObject::connect(&_service, SIGNAL(credentialsFound(const Token&)),
                     this, SLOT(openUrlAndFinish(Token)));

    QObject::connect(&_service, SIGNAL(twoFactorAuthRequired()),
                     this, SLOT(handleTwoFactorAuthRequired()));

    QObject::connect(&_service, SIGNAL(requestFailed(const ErrorResponse&)),
                     this, SLOT(serviceFailed(const ErrorResponse&)));
    QObject::connect(ui->pageLogin, SIGNAL(newCustomerSelected(QString,QString)),
                     this, SLOT(showRegisterPage(QString,QString)));
    QObject::connect(ui->pageLogin, SIGNAL(loginCheckout(QString,QString)),
                     this, SLOT(loginAndBuy(QString,QString)));
    QObject::connect(ui->pageRegister, SIGNAL(goBack()),
                     this, SLOT(showPageLogin()));
    QObject::connect(ui->pageRegister, SIGNAL(registerCheckout(QString,QString,QString)),
                     this, SLOT(registerAndBuy(QString, QString, QString)));

    downloader.startDownload(picture);

    // Set the error messages depending the code.
    _codeMessages[ErrorCodes::CODE_CAPTCHA_REQUIRED] = "A captcha challenge is required to complete the request.";
    _codeMessages[ErrorCodes::CODE_INVALID_CREDENTIALS] = "That's not your password. Please try again.";
    _codeMessages[ErrorCodes::CODE_TWOFACTOR_REQUIRED] = "2-factor authentication required.";
    _codeMessages[ErrorCodes::CODE_ACCOUNT_SUSPENDED] = "Your account has been suspended. Please contact login support to re-enable it.";
    _codeMessages[ErrorCodes::CODE_ACCOUNT_DEACTIVATED] = "Your account has been deactivated. To reactivate it, please reset your password.";
    _codeMessages[ErrorCodes::CODE_EMAIL_INVALIDATED] = "This email address has been invalidated. Please contact login support.";
    _codeMessages[ErrorCodes::CODE_CAN_NOT_RESET_PASSWORD] = "Can not reset password. Please contact login support.";
    _codeMessages[ErrorCodes::CODE_ALREADY_REGISTERED] = "The email address is already registered.";
}

QString SSOWizard::cleanArgument(QString& arg)
{
    if(arg.startsWith("\"")){
        arg.remove(0, 1);
    }
    if(arg.endsWith("\"")){
        arg.chop(1);
    }

    return arg;
}

SSOWizard::~SSOWizard()
{
    delete ui;
}

void SSOWizard::showRegisterPage(QString email, QString password)
{
    ui->pageRegister->setEmail(email);
    ui->pageRegister->setPassword(password);

    ui->stackedWidget->setCurrentIndex(1);
}

void SSOWizard::loginAndBuy(QString email, QString password)
{
    hideError();
    _overlay->show();
    _service.login(email, password);
}

void SSOWizard::accountAuthenticated()
{
    _service.getCredentials();
}

void SSOWizard::openUrlAndFinish(Token token)
{
    if (purchaseUrl.startsWith("http")) {
        static const QString apiUrl = QStringLiteral("https://one.ubuntu.com/api/1.0/from_oauth/");
        QString urlToSign = apiUrl + "?next=" + purchaseUrl;
        QString params = token.signUrl(urlToSign, QStringLiteral("GET"), true);
        QString realUrl = apiUrl + "?" + params + "&next=" + purchaseUrl;
        QDesktopServices::openUrl(realUrl);
    }
    _overlay->hide();
    emit aboutToClose();
}

void SSOWizard::registerAndBuy(QString email, QString password, QString name)
{
    hideError();
    _overlay->show();
    _service.registerUser(email, password, name);
}

void SSOWizard::showPageLogin()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void SSOWizard::handleTwoFactorAuthRequired()
{
    _overlay->hide();
    showError(ErrorResponse(401, QString(), TWOFACTOR_REQUIRED, QString()));
}

void SSOWizard::serviceFailed(const ErrorResponse& error)
{
    _overlay->hide();
    showError(error);
}

void SSOWizard::showError(const ErrorResponse& error)
{
    ui->lblError->setText(_codeMessages.value(error.code(), GENERAL_ERROR_MESSAGE));
    ui->lblError->setEnabled(true);

    ui->pageLogin->showErrorTips(error);
    ui->pageRegister->showErrorTips(error);
}

void SSOWizard::hideError()
{
    ui->lblError->setText("");
    ui->lblError->setEnabled(false);
}

void SSOWizard::setHeader(QString artist, QString album, QString price)
{
    ui->header->setAlbum(album);
    ui->header->setArtist(artist);
    ui->header->setPrice(price);
}

void SSOWizard::imageDownloaded(QString& path)
{
    ui->header->setPicture(path);
}

void SSOWizard::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    _overlay->resize(event->size());
}
