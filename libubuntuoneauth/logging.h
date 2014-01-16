/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef _U1_AUTH_LOGGING_H_
#define _U1_AUTH_LOGGING_H_

#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QtGlobal>

namespace UbuntuOne {

    class AuthLogger : public QObject
    {
        Q_OBJECT
    public:
        QtMsgType _logLevel = QtWarningMsg;

        explicit AuthLogger(QObject *parent = 0);
        Q_DECL_DEPRECATED AuthLogger(const QString filename);

        void logMessage(QtMsgType type, const QMessageLogContext &context,
                        const QString &message);

        static void setupLogging();
        static void stopLogging();
        Q_DECL_DEPRECATED static void setupLogging(const QString filename);
        static bool setLogLevel(QtMsgType level);
        static const QString getMessageTypeString(QtMsgType type);
        Q_DECL_DEPRECATED static QString getLogDirectory();

    private:
        bool _initialized = false;
        QTextStream _logStream;

        const QString _datetimeFormat = "yyyy-MM-dd hh:mm:ss,zzz";
    };

} /* namespace UbuntuOne */

#endif /* _U1_AUTH_LOGGING_H_ */
