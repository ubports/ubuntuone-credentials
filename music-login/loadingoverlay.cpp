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

#include "loadingoverlay.h"
#include "ui_loadingoverlay.h"
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QPen>
#include <QPaintEvent>
#include <QEvent>
#include <QShowEvent>
#include <QTimerEvent>
#include <QPalette>
#include <QLinearGradient>
#include <Qt>
#include <QDebug>

LoadingOverlay::LoadingOverlay(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LoadingOverlay)
{
    ui->setupUi(this);
    counter = 0;
    timer = 0;
    orientation = false;
}

LoadingOverlay::~LoadingOverlay()
{
    delete ui;
}

void LoadingOverlay::paintEvent(QPaintEvent * event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(event->rect(), QBrush(QColor(255, 255, 255, 135)));
    painter.setPen(QPen(Qt::NoPen));
    painter.end();
    QFrame::paintEvent(event);
}

bool LoadingOverlay::eventFilter(QObject * watched, QEvent * event)
{
    if((ui != NULL) && (watched == ui->frm_box) && (event->type() == QEvent::Paint)){
        QPainter painter;
        painter.begin(ui->frm_box);
        painter.setRenderHint(QPainter::Antialiasing, true);
        int posx = ui->frm_box->width() / 3;
        int paddingx = posx / 5;
        int i;
        for(i = 0; i < 5; i++){
            QLinearGradient linearGradient(
                            posx + (paddingx * i),
                            ui->frm_box->height() / 2 + 10,
                            posx + (paddingx * i) + 15,
                            ui->frm_box->height() / 2 + 25);
            if(counter != i){
                linearGradient.setColorAt(0, QColor(205, 200, 198));
                linearGradient.setColorAt(1, QColor(237, 237, 237));
            }else{
                linearGradient.setColorAt(0, QColor(240, 67, 26));
                linearGradient.setColorAt(1, QColor(255, 122, 53));
            }
            painter.setBrush(QBrush(linearGradient));
            painter.drawEllipse(posx + (paddingx * i),
                                ui->frm_box->height() / 2 + 10, 15, 15);
        }
        painter.end();
    }

    return false;
}

void LoadingOverlay::showEvent(QShowEvent * event)
{
    QFrame::showEvent(event);
    ui->frm_box->installEventFilter(this);
    QPalette transparent_palette(palette());
    transparent_palette.setColor(QPalette::Background, Qt::transparent);
    setPalette(transparent_palette);

    if(timer == 0){
        timer = startTimer(200);
    }
}

void LoadingOverlay::timerEvent(QTimerEvent *)
{
    if(counter == 0){
        orientation = true;
    }else if(counter == 4){
        orientation = false;
    }

    if(orientation){
        counter++;
    }else{
        counter--;
    }

    update();
}
