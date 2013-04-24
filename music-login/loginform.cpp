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

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    this->_sessionActive = false;
    this->ui->lblEmailError->setVisible(false);
    this->ui->lblPasswordError->setVisible(false);
    this->ui->btnProceed->setDefault(true);

    // Connect Signals
    QObject::connect(this->ui->lineEmail, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
    QObject::connect(this->ui->linePassword, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
    QObject::connect(this->ui->lineEmail, SIGNAL(editingFinished()), this, SLOT(showEmailWarning()));
    QObject::connect(this->ui->linePassword, SIGNAL(editingFinished()), this, SLOT(showPasswordWarning()));
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::validateForm()
{
    bool value = this->checkEmail() && this->checkPassword() && this->_sessionActive;
    this->ui->btnProceed->setEnabled(value);
}

bool LoginForm::checkEmail()
{
    return !this->ui->lineEmail->text().isEmpty() && this->ui->lineEmail->text().contains("@");
}

bool LoginForm::checkPassword()
{
    bool value = this->ui->linePassword->text().length() > 7;
//    value = value && this->ui->linePassword->text().contains(QRegExp("[A-Z]"));
//    value = value && this->ui->linePassword->text().contains(QRegExp("[0-9]"));

    return value;
}

void LoginForm::on_btnProceed_clicked()
{
    if(this->ui->radioNewCustomer->isChecked()){
        emit this->newCustomerSelected(this->ui->lineEmail->text(), this->ui->linePassword->text());
    }else{
        emit this->loginCheckout(this->ui->lineEmail->text(), this->ui->linePassword->text());
    }
}

void LoginForm::setSessionState(bool value)
{
    this->_sessionActive = value;
}

void LoginForm::on_lineEmail_returnPressed()
{
    this->ui->linePassword->setFocus();
}

void LoginForm::on_linePassword_returnPressed()
{
    this->ui->btnProceed->click();
}

void LoginForm::showEmailWarning()
{
    this->ui->lblEmailError->setVisible(!this->checkEmail());
}

void LoginForm::showPasswordWarning()
{
    this->ui->lblPasswordError->setVisible(!this->checkPassword());
}