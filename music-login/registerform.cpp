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
    this->_sessionActive = false;
    this->ui->lblNameError->setVisible(false);
    this->ui->lblPasswordError->setVisible(false);
    this->ui->lblConfirmError->setVisible(false);
    this->ui->btnCheckout->setDefault(true);

    QObject::connect(this->ui->lineEmail, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(this->ui->lineName, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(this->ui->linePassword, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(this->ui->lineConfirm, SIGNAL(textChanged(QString)),
                     this, SLOT(validateForm()));
    QObject::connect(this->ui->checkBox, SIGNAL(clicked()),
                     this, SLOT(validateForm()));
    QObject::connect(this->ui->lineName, SIGNAL(editingFinished()),
                     this, SLOT(showNameWarning()));
    QObject::connect(this->ui->linePassword, SIGNAL(editingFinished()),
                     this, SLOT(showPasswordWarning()));
    QObject::connect(this->ui->lineConfirm, SIGNAL(editingFinished()),
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
        this->ui->lineEmail->setProperty("error", true);
    }else if(error.code() == ErrorCodes::CODE_INVALID_CREDENTIALS){
        this->ui->lineEmail->setProperty("error", true);
        this->ui->linePassword->setProperty("error", true);
    }

    this->style()->unpolish(this);
    this->style()->polish(this);
}

void RegisterForm::setEmail(QString email)
{
    this->ui->lineEmail->setText(email);
}

void RegisterForm::setPassword(QString password)
{
    this->ui->linePassword->setText(password);
}

void RegisterForm::on_btnBack_clicked()
{
    emit this->goBack();
}

void RegisterForm::validateForm()
{
    bool value = (this->checkEmail() && this->checkName() &&
                  this->checkPassword() && this->checkConfirmPassword() &&
                  this->ui->checkBox->isChecked() && this->_sessionActive);
    this->ui->btnCheckout->setEnabled(value);
}

bool RegisterForm::checkEmail()
{
    QRegExp mailRE(".+@.+");
    bool value = mailRE.exactMatch(this->ui->lineEmail->text());
    this->ui->lineEmail->setProperty("error", !value);
    this->style()->unpolish(this->ui->lineEmail);
    this->style()->polish(this->ui->lineEmail);
    return value;
}

bool RegisterForm::checkPassword()
{
    bool value = this->ui->linePassword->text().length() > 7;
    this->ui->linePassword->setProperty("error", !value);
    this->style()->unpolish(this->ui->linePassword);
    this->style()->polish(this->ui->linePassword);

    return value;
}

bool RegisterForm::checkName()
{
    return !this->ui->lineName->text().isEmpty();
}

bool RegisterForm::checkConfirmPassword()
{
    return this->ui->linePassword->text() == this->ui->lineConfirm->text();
}

void RegisterForm::on_btnCheckout_clicked()
{
    QString name = this->ui->lineName->text();
    QString email = this->ui->lineEmail->text();
    QString password = this->ui->linePassword->text();
    emit this->registerCheckout(email, password, name);
}

void RegisterForm::setSessionState(bool value)
{
    this->_sessionActive = value;
}

void RegisterForm::on_lineEmail_returnPressed()
{
    this->ui->lineName->setFocus();
}

void RegisterForm::on_lineName_returnPressed()
{
    this->ui->linePassword->setFocus();
}

void RegisterForm::on_linePassword_returnPressed()
{
    this->ui->lineConfirm->setFocus();
}

void RegisterForm::on_lineConfirm_returnPressed()
{
    this->ui->btnCheckout->click();
}

void RegisterForm::showNameWarning()
{
    this->ui->lblNameError->setVisible(!this->checkName());
}

void RegisterForm::showPasswordWarning()
{
    this->ui->lblPasswordError->setVisible(!this->checkPassword());
}

void RegisterForm::showConfirmWarning()
{
    this->ui->lblConfirmError->setVisible(!this->checkConfirmPassword());
}
