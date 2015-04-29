#include "blurrer.h"

#include <QImage>
#include <QGraphicsBlurEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QPixmap>


void Blurrer::blur(QImage &source, QImage &dest)
{
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
    effect->setBlurRadius(0.1 * source.width());

    item.setPixmap(QPixmap::fromImage(source));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);

    dest = QImage(source.size(), QImage::Format_ARGB32);
    dest.fill(Qt::transparent);

    QPainter painter(&dest);
    scene.render(&painter);

    delete effect;
}
