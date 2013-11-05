#include "downloader.h"
#include <QUuid>
#include <QFile>
#include <QDebug>

Downloader::Downloader(QObject *parent) :
    QObject(parent)
{
    _nam = new QNetworkAccessManager(this);
    _request = new QNetworkRequest();
    createDownloadDir();

    QObject::connect(_nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(onReply(QNetworkReply*)));
}

void Downloader::onReply(QNetworkReply* reply)
{
    QVariant statusAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(!statusAttr.isValid()) {
        qDebug() << "No image resource available!";
        return;
    }

    QByteArray payload = reply->readAll();
    if(!payload.isEmpty()) {
        QUuid uuid = QUuid::createUuid();
        QString imagePath = _downloadDir->filePath(uuid.toString());
        QFile file(imagePath);
        file.open(QIODevice::WriteOnly);
        file.write(payload.data(), payload.size());
        file.close();

        emit fileDownloaded(imagePath);
    }
}

void Downloader::startDownload(const QString& url)
{
    if(!url.isEmpty()){
        _request->setUrl(url);
        _nam->get(*_request);
    }
}

void Downloader::createDownloadDir(){
    _downloadDir = new QDir(QDir::temp().filePath("u1credentials"));
    if(!_downloadDir->exists()){
        _downloadDir->mkpath(_downloadDir->absolutePath());
    }
}
