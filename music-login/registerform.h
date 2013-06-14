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

#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QWidget>
#include "responses.h"

using namespace UbuntuOne;

namespace Ui {
class RegisterForm;
}

#ifdef TESTS
class TestRegisterForm;
#endif

class RegisterForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterForm(QWidget *parent = 0);
    ~RegisterForm();

    void setEmail(QString email);
    void setPassword(QString password);
    void setSessionState(bool value);
    void showErrorTips(const ErrorResponse& error);

signals:
    void goBack();
    void registerCheckout(QString email, QString password, QString name);

private slots:
    void validateForm();
    void on_btnBack_clicked();
    void on_btnCheckout_clicked();
    void on_lineEmail_returnPressed();
    void on_lineName_returnPressed();
    void on_linePassword_returnPressed();
    void on_lineConfirm_returnPressed();
    void showNameWarning();
    void showPasswordWarning();
    void showConfirmWarning();

private:
    Ui::RegisterForm *ui;
    bool _sessionActive;

    bool checkEmail();
    bool checkName();
    bool checkPassword();
    bool checkConfirmPassword();

#ifdef TESTS
    friend class TestRegisterForm;
#endif
};

#endif // REGISTERFORM_H
