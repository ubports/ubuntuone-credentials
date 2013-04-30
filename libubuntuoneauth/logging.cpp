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
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "logging.h"

namespace UbuntuOne {

    static void *_logger;
    static void _realMessageHandler(QtMsgType type,
                                    const QMessageLogContext &context,
                                    const QString &message)
    {
        if (_logger == NULL)
            return;

        ((AuthLogger *)_logger)->logMessage(type, context, message);
    }

    AuthLogger::AuthLogger (const QString filename)
    {
        if (filename == "") {
            QString cachePath = getLogDirectory();
            _logFileName = cachePath + "/authentication.log";
        } else
            _logFileName = filename;

        _logFile.setFileName(_logFileName);
        if (_logFile.open(QFile::WriteOnly | QFile::Truncate)) {
            _logStream.setDevice(&_logFile);
            _logStream.seek(0);
            _logStream.flush();
        }

        qInstallMessageHandler(_realMessageHandler);

        _initialized = true;
    }

    void AuthLogger::setupLogging(const QString filename)
    {
        if (_logger == NULL)
            _logger = new AuthLogger(filename);
    }

    void AuthLogger::setLogLevel(QtMsgType level)
    {
        ((AuthLogger *)_logger)->_logLevel = level;
    }

    const QString AuthLogger::getMessageTypeString(QtMsgType type)
    {
        switch (type) {
        case QtDebugMsg:
            return "DEBUG";
            break;
        case QtWarningMsg:
            return "WARNING";
            break;
        case QtCriticalMsg:
            return "CRITICAL";
            break;
        case QtFatalMsg:
            return "FATAL";
            break;
        }
        return "UNKNOWN";
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

        _logStream << QDateTime::currentDateTime().toString(_datetimeFormat).toUtf8().data() << " - " << getMessageTypeString(type).toUtf8().data() << " - " << message.toUtf8().data() << "\n";
        _logStream.flush();

        if (type == QtFatalMsg)
            abort();
    }

} // namespace UbuntuOne
