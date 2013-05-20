#include "downloader.h"
#include <QUuid>
#include <QFile>
#include <QDebug>

Downloader::Downloader(QObject *parent) :
    QObject(parent)
{
    this->_nam = new QNetworkAccessManager(this);
    this->_request = new QNetworkRequest();
    this->createDownloadDir();

    QObject::connect(this->_nam, SIGNAL(finished(QNetworkReply*)),
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
        QString imagePath = this->_downloadDir->filePath(uuid.toString());
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
        this->_request->setUrl(url);
        this->_nam->get(*this->_request);
    }
}

void Downloader::createDownloadDir(){
    this->_downloadDir = new QDir(QDir::temp().filePath("u1credentials"));
    if(!this->_downloadDir->exists()){
        this->_downloadDir->mkpath(this->_downloadDir->absolutePath());
    }
}
