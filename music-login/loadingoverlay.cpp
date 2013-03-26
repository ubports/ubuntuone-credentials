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
    this->counter = 0;
    this->timer = 0;
    this->orientation = false;
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
    if((this->ui != NULL) && (watched == this->ui->frm_box) && (event->type() == QEvent::Paint)){
        QPainter painter;
        painter.begin(this->ui->frm_box);
        painter.setRenderHint(QPainter::Antialiasing, true);
        int posx = this->ui->frm_box->width() / 3;
        int paddingx = posx / 5;
        int i;
        for(i = 0; i < 5; i++){
            QLinearGradient linearGradient(
                            posx + (paddingx * i),
                            this->ui->frm_box->height() / 2 + 10,
                            posx + (paddingx * i) + 15,
                            this->ui->frm_box->height() / 2 + 25);
            if(this->counter != i){
                linearGradient.setColorAt(0, QColor(205, 200, 198));
                linearGradient.setColorAt(1, QColor(237, 237, 237));
            }else{
                linearGradient.setColorAt(0, QColor(240, 67, 26));
                linearGradient.setColorAt(1, QColor(255, 122, 53));
            }
            painter.setBrush(QBrush(linearGradient));
            painter.drawEllipse(posx + (paddingx * i),
                                this->ui->frm_box->height() / 2 + 10, 15, 15);
        }
        painter.end();
    }

    return false;
}

void LoadingOverlay::showEvent(QShowEvent * event)
{
    QFrame::showEvent(event);
    this->ui->frm_box->installEventFilter(this);
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background, Qt::transparent);
    this->setPalette(palette);

    if(this->timer == 0){
        this->timer = this->startTimer(200);
    }
}

void LoadingOverlay::timerEvent(QTimerEvent *)
{
    if(this->counter == 0){
        this->orientation = true;
    }else if(this->counter == 4){
        this->orientation = false;
    }

    if(this->orientation){
        this->counter++;
    }else{
        this->counter--;
    }

    this->update();
}
