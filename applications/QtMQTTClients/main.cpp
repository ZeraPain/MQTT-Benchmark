#include <QCoreApplication>
#include <QDebug>

#include "publisher.h"
#include "subscriber.h"

int main(int argc, char *argv[])
{
    QCoreApplication a{argc, argv};

    Subscriber *sub1 = new Subscriber("Sub1", &a);
    Subscriber *sub2 = new Subscriber("Sub2", &a);
    Q_UNUSED(sub1)
    Q_UNUSED(sub2)

    Publisher *pub1 = new Publisher{"Pub1", &a};
    Publisher *pub2 = new Publisher{"Pub2", &a};
    Q_UNUSED(pub1)
    Q_UNUSED(pub2)

    return a.exec();
}
