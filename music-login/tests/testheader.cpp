#include "testheader.h"
#include <QtTest/QtTest>
#include <QDebug>
#include "ui_header.h"


TestHeader::TestHeader(QObject *parent) :
    QObject(parent)
{
}

void TestHeader::testSetAlbum()
{
    QString album = "album name";
    this->header.setAlbum(album);

    QCOMPARE(this->header.ui->lblAlbum->text(), album);
}

void TestHeader::testSetArtist()
{
    QString artist = "Artist Name";
    this->header.setArtist(artist);

    QCOMPARE(this->header.ui->lblArtist->text(), artist);
}

void TestHeader::testSetPrice()
{
    QString price = "$7.99";
    this->header.setPrice(price);

    QCOMPARE(this->header.ui->lblPrice->text(), price);
}

void TestHeader::testSetPicture()
{
    QVERIFY(this->header.ui->lblImage->pixmap() == NULL);
    QString picture = "/home/path/picture.png";
    this->header.setPicture(picture);

    QVERIFY(this->header.ui->lblImage->pixmap() != NULL);
}

