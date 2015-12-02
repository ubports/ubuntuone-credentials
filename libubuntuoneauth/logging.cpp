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
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>

#include "logging.h"

namespace UbuntuOne {

    static void *_logger = NULL;
    static void _realMessageHandler(QtMsgType type,
                                    const QMessageLogContext &context,
                                    const QString &message)
    {
        if (_logger == NULL)
            return;

        ((AuthLogger *)_logger)->logMessage(type, context, message);
    }

    AuthLogger::AuthLogger (QObject *parent)
        : QObject(parent)
    {
        qInstallMessageHandler(_realMessageHandler);

        _initialized = true;
    }

    AuthLogger::AuthLogger (const QString filename)
    {
        qInstallMessageHandler(_realMessageHandler);

        _initialized = true;
    }

    void AuthLogger::setupLogging()
    {
        if (_logger == NULL)
            _logger = new AuthLogger();
    }

    void AuthLogger::setupLogging(const QString filename)
    {
        setupLogging();
    }

    void AuthLogger::stopLogging() {
        if (_logger != NULL) {
            delete (AuthLogger *)_logger;
            _logger = NULL;
        }
    }

    bool AuthLogger::setLogLevel(QtMsgType level)
    {
        if (_logger != NULL) {
            ((AuthLogger *)_logger)->_logLevel = level;
            return true;
        }

        return false;
    }

    const QString AuthLogger::getMessageTypeString(QtMsgType type)
    {
        switch (type) {
        case QtDebugMsg:
            return QStringLiteral("DEBUG");
            break;
        case QtWarningMsg:
            return QStringLiteral("WARNING");
            break;
        case QtCriticalMsg:
            return QStringLiteral("CRITICAL");
            break;
        case QtFatalMsg:
            return QStringLiteral("FATAL");
            break;
#if QT_VERSION >= 0x505000
        case QtInfoMsg:
            return QStringLiteral("INFO");
            break;
#endif
        }
        return QStringLiteral("UNKNOWN");
    }

    QString AuthLogger::getLogDirectory()
    {
        QString cachePath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
        QString logPath = cachePath + "/ubuntuone/log";
        QDir cacheDir = QDir(logPath);
        if (!cacheDir.exists(logPath))
            cacheDir.mkpath(logPath);

        return logPath;
    }

    void AuthLogger::logMessage(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &message)
    {
        if (type < _logLevel)
            return;

        QString logMessage;
        QTextStream _logMessage(&logMessage);
        _logMessage << QDateTime::currentDateTime().toString(_datetimeFormat).toUtf8().data() << " - " << getMessageTypeString(type).toUtf8().data() << " - " << message.toUtf8().data() << "\n";

        QTextStream _stdErr(stderr, QIODevice::WriteOnly);
        _stdErr << logMessage;
        _stdErr.device()->close();
    }

} // namespace UbuntuOne
