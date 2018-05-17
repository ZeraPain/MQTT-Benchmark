#include <QCoreApplication>
#include <QDebug>

#include "publisher.h"
#include "subscriber.h"

int main(int argc, char *argv[])
{
    QCoreApplication a{argc, argv};

    if(argc != 1){
        qDebug() << "Subscribe";
        Subscriber *sub = new Subscriber(&a);
        Q_UNUSED(sub)
    } else {
        qDebug() << "Publish";
        Publisher *pub = new Publisher{&a};
        Q_UNUSED(pub)
    }
    return a.exec();
}
