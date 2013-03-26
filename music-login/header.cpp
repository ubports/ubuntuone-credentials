#include "header.h"
#include "ui_header.h"
#include <QPixmap>
#include <Qt>

Header::Header(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Header)
{

    ui->setupUi(this);
}

Header::~Header()
{
    delete ui;
}

void Header::setAlbum(QString album)
{
    this->ui->lblAlbum->setText(album);
}

void Header::setArtist(QString artist)
{
    this->ui->lblArtist->setText(artist);
}

void Header::setPrice(QString price)
{
    this->ui->lblPrice->setText(price);
}

void Header::setPicture(QString picture_path)
{
    QPixmap pixmap(picture_path);
    pixmap = pixmap.scaledToWidth(75, Qt::SmoothTransformation);
    this->ui->lblImage->setPixmap(pixmap);
}
