#include "publisher.h"
#include "subscriber.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a{argc, argv};

    if(argc == 1){
        Subscriber *sub = new Subscriber("Sub", &a);
        Q_UNUSED(sub);
    } else if(argc == 3){
        qint32 messageSize = atoi(argv[1]);
        qint32 timeout = atoi(argv[2]);

        Publisher *pub = new Publisher("Pub", messageSize, timeout, &a);
        Q_UNUSED(pub);
    } else {
        qDebug() << "Wrong parameter number (msgsize, intervall)";
        return 0;
    }

    return a.exec();
}
