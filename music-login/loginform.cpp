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

#include "loginform.h"
#include "ui_loginform.h"
#include <QRegExp>
#include <QDebug>
#include "errormessages.h"

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    _sessionActive = false;
    ui->lblEmailError->setVisible(false);
    ui->lblPasswordError->setVisible(false);
    ui->btnProceed->setDefault(true);

    // Connect Signals
    QObject::connect(ui->lineEmail, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
    QObject::connect(ui->linePassword, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
    QObject::connect(ui->lineEmail, SIGNAL(editingFinished()), this, SLOT(showEmailWarning()));
    QObject::connect(ui->linePassword, SIGNAL(editingFinished()), this, SLOT(showPasswordWarning()));
}

void LoginForm::showErrorTips(const ErrorResponse& error)
{
    if(error.code() == ErrorCodes::CODE_ALREADY_REGISTERED ||
            error.code() == ErrorCodes::CODE_EMAIL_INVALIDATED){
        ui->lineEmail->setProperty("error", true);
    }else if(error.code() == ErrorCodes::CODE_INVALID_CREDENTIALS){
        ui->lineEmail->setProperty("error", true);
        ui->linePassword->setProperty("error", true);
    }

    style()->unpolish(this);
    style()->polish(this);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::validateForm()
{
    bool passwordCheck = ui->radioNewCustomer->isChecked() || checkPassword();
    bool value = checkEmail() && passwordCheck && _sessionActive;
    ui->btnProceed->setEnabled(value);
}

bool LoginForm::checkEmail()
{
    QRegExp mailRE(".+@.+");
    bool value = mailRE.exactMatch(ui->lineEmail->text());
    ui->lineEmail->setProperty("error", !value);
    style()->unpolish(ui->lineEmail);
    style()->polish(ui->lineEmail);
    return value;
}

bool LoginForm::checkPassword()
{
    bool value = true;
    if(ui->radioReturningCustomer->isChecked()){
        value = ui->linePassword->text().length() > 7;
    }
    ui->linePassword->setProperty("error", !value);
    style()->unpolish(ui->linePassword);
    style()->polish(ui->linePassword);

    return value;
}

void LoginForm::on_btnProceed_clicked()
{
    if(ui->radioNewCustomer->isChecked()){
        emit newCustomerSelected(ui->lineEmail->text(), ui->linePassword->text());
    }else{
        emit loginCheckout(ui->lineEmail->text(), ui->linePassword->text());
    }
}

void LoginForm::setSessionState(bool value)
{
    _sessionActive = value;
}

void LoginForm::on_lineEmail_returnPressed()
{
    ui->linePassword->setFocus();
}

void LoginForm::on_linePassword_returnPressed()
{
    ui->btnProceed->click();
}

void LoginForm::on_radioNewCustomer_clicked()
{
    ui->btnProceed->setText(tr("Continue"));
    validateForm();
    checkPassword();
    ui->lblPasswordError->setVisible(false);
}

void LoginForm::on_radioReturningCustomer_clicked()
{
    ui->btnProceed->setText(tr("Proceed to Checkout"));
    validateForm();
}

void LoginForm::showEmailWarning()
{
    ui->lblEmailError->setVisible(!checkEmail());
}

void LoginForm::showPasswordWarning()
{
    if(ui->radioReturningCustomer->isChecked()){
        ui->lblPasswordError->setVisible(!checkPassword());
    }
}
