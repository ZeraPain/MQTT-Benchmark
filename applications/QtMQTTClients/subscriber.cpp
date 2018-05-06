#include "subscriber.h"

#include <QDebug>

Subscriber::Subscriber(QObject *parent)
    : QMqttClient{parent}
{
    //setHostname("fe80::83ca:204d:3c0a:5a58");
    setHostname("192.168.56.1");
    setPort(2222);

    qDebug() << "Client Id" << clientId();

    connect(this, &QMqttClient::stateChanged, this, &Subscriber::stateChanged);
    connect(this, &QMqttClient::errorChanged, this, &Subscriber::errorChanged);
    connect(this, &QMqttClient::messageReceived, this, &Subscriber::printMessage);
    connect(this, &QMqttClient::pingResponseReceived, this, [this]() {
            const QString content = QDateTime::currentDateTime().toString()
                        + QLatin1String(" PingResponse")
                        + QLatin1Char('\n');
            qDebug() << content;
        });
    requestPing();
    qDebug() << "Ping requested";

    connectToHost();
    qDebug() << "Client Created";
}

void Subscriber::listenToTopic(const QString &topic)
{
    auto subscription = subscribe(topic);
    if (!subscription) {
        qDebug() << Q_FUNC_INFO << "Subscription Fehlgeschlagen";
        return;
    }
    qDebug() << "Subscription submitted";
}

void Subscriber::printMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" Received Topic: ")
                + topic.name()
                + QLatin1String(" Message: ")
                + message
                + QLatin1Char('\n');
    qDebug() << content;
}

void Subscriber::stateChanged(QMqttClient::ClientState state)
{
    qDebug() << "Client state changed";
    switch (state) {
        case ClientState::Connected:
            qDebug() << "Client connected";
            listenToTopic("test/test");
        break;
        case ClientState::Connecting:
            qDebug() << "Client connecting";
        break;
        case ClientState::Disconnected:
            qDebug() << "Client disconnected";
        break;
        default:
            qDebug() << "Unhandelt state";
        return;
    }
}

void Subscriber::errorChanged(QMqttClient::ClientError error)
{
    qDebug() << "Error changed";
}

