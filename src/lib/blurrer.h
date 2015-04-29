#ifndef BLURRER_H
#define BLURRER_H

class QImage;

class Blurrer
{
public:
    static void blur(QImage &source, QImage &dest);
};

#endif // BLURRER_H
