#include <QCoreApplication>

#include "publisher.h"
#include "subscriber.h"

int main(int argc, char *argv[])
{
    QCoreApplication a{argc, argv};

    if(argc != 1){
        qDebug() << "Subscribe";
        Subscriber sub{&a};
    } else {
        qDebug() << "Publish";
        Publisher pub{&a};
    }
    return a.exec();
}

