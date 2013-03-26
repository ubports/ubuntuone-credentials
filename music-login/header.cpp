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
