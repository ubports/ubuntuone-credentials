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

#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "responses.h"

using namespace UbuntuOne;

namespace Ui {
class LoginForm;
}

#ifdef TESTS
class TestLoginForm;
#endif

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = 0);
    ~LoginForm();

    void setSessionState(bool value);
    void showErrorTips(const ErrorResponse& error);

signals:
    void newCustomerSelected(QString email, QString password);
    void loginCheckout(QString email, QString password);

private slots:
    void validateForm();
    void on_btnProceed_clicked();
    void on_lineEmail_returnPressed();
    void on_linePassword_returnPressed();
    void on_radioNewCustomer_clicked();
    void on_radioReturningCustomer_clicked();
    void showEmailWarning();
    void showPasswordWarning();

private:
    Ui::LoginForm *ui;
    bool _sessionActive;

    bool checkEmail();
    bool checkPassword();

#ifdef TESTS
    friend class TestLoginForm;
#endif
};

#endif // LOGINFORM_H
