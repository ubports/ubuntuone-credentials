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

#include "registerform.h"
#include "ui_registerform.h"
#include "errormessages.h"

RegisterForm::RegisterForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterForm)
{
    ui->setupUi(this);
    _sessionActive = false;
    ui->lblNameError->setVisible(false);
    ui->lblPasswordError->setVisible(false);
    ui->lblConfirmError->setVisible(false);
    ui->btnCheckout->setDefault(true);

    QObject::connect(ui->lineEmail, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(ui->lineName, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(ui->linePassword, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(ui->lineConfirm, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(ui->checkBox, SIGNAL(clicked()),
                     this, SLOT(validateForm()));
    QObject::connect(ui->lineName, SIGNAL(editingFinished()),
                     this, SLOT(showNameWarning()));
    QObject::connect(ui->linePassword, SIGNAL(editingFinished()),
                     this, SLOT(showPasswordWarning()));
    QObject::connect(ui->lineConfirm, SIGNAL(editingFinished()),
                     this, SLOT(showConfirmWarning()));
}

RegisterForm::~RegisterForm()
{
    delete ui;
}

void RegisterForm::showErrorTips(const ErrorResponse& error)
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

void RegisterForm::setEmail(QString email)
{
    ui->lineEmail->setText(email);
}

void RegisterForm::setPassword(QString password)
{
    ui->linePassword->setText(password);
}

void RegisterForm::on_btnBack_clicked()
{
    emit goBack();
}

void RegisterForm::validateForm()
{
    bool value = (checkEmail() && checkName() &&
                  checkPassword() && checkConfirmPassword() &&
                  ui->checkBox->isChecked() && _sessionActive);
    ui->btnCheckout->setEnabled(value);
}

bool RegisterForm::checkEmail()
{
    QRegExp mailRE(".+@.+");
    bool value = mailRE.exactMatch(ui->lineEmail->text());
    ui->lineEmail->setProperty("error", !value);
    style()->unpolish(ui->lineEmail);
    style()->polish(ui->lineEmail);
    return value;
}

bool RegisterForm::checkPassword()
{
    bool value = ui->linePassword->text().length() > 7;
    ui->linePassword->setProperty("error", !value);
    style()->unpolish(ui->linePassword);
    style()->polish(ui->linePassword);

    return value;
}

bool RegisterForm::checkName()
{
    return !ui->lineName->text().isEmpty();
}

bool RegisterForm::checkConfirmPassword()
{
    return ui->linePassword->text() == ui->lineConfirm->text();
}

void RegisterForm::on_btnCheckout_clicked()
{
    QString name = ui->lineName->text();
    QString email = ui->lineEmail->text();
    QString password = ui->linePassword->text();
    emit registerCheckout(email, password, name);
}

void RegisterForm::setSessionState(bool value)
{
    _sessionActive = value;
}

void RegisterForm::on_lineEmail_returnPressed()
{
    ui->lineName->setFocus();
}

void RegisterForm::on_lineName_returnPressed()
{
    ui->linePassword->setFocus();
}

void RegisterForm::on_linePassword_returnPressed()
{
    ui->lineConfirm->setFocus();
}

void RegisterForm::on_lineConfirm_returnPressed()
{
    ui->btnCheckout->click();
}

void RegisterForm::showNameWarning()
{
    ui->lblNameError->setVisible(!checkName());
}

void RegisterForm::showPasswordWarning()
{
    ui->lblPasswordError->setVisible(!checkPassword());
}

void RegisterForm::showConfirmWarning()
{
    ui->lblConfirmError->setVisible(!checkConfirmPassword());
}
