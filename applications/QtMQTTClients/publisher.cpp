#include "publisher.h"

#include <QDebug>

Publisher::Publisher(QObject *parent)
    : QObject{parent}
    , m_client(new QMqttClient{})
{
    m_client->setHostname("127.0.0.1");
    m_client->setPort(12345);

    qDebug() << "Client Id" << m_client->clientId();

    connect(m_client.data(), &QMqttClient::stateChanged, this, &Publisher::stateChanged);
    connect(m_client.data(), &QMqttClient::errorChanged, this, &Publisher::errorChanged);
    connect(m_client.data(), &QMqttClient::pingResponseReceived, this, [this]() {
            const QString content = QDateTime::currentDateTime().toString()
                        + QLatin1String(" PingResponse")
                        + QLatin1Char('\n');
            qDebug() << content;
        });
    m_client->requestPing();
    qDebug() << "Ping requested";

    m_client->connectToHost();
    qDebug() << "Client Created";
}

void Publisher::sendMessage(int bytes)
{
    qDebug() << "messge send";
    m_client->publish(QString{"test/test"}, QByteArray{"test"});
}

void Publisher::stateChanged(QMqttClient::ClientState state)
{
    qDebug() << "Client state changed";
    switch (state) {
        case QMqttClient::ClientState::Connected:
            qDebug() << "Client connected";
            sendMessage(1);
        break;
        case QMqttClient::ClientState::Connecting:
            qDebug() << "Client connecting";
        break;
        case QMqttClient::ClientState::Disconnected:
            qDebug() << "Client disconnected";
        break;
        default:
            qDebug() << "Unhandelt state";
        return;
    }
}

void Publisher::errorChanged(QMqttClient::ClientError error)
{
    qDebug() << "Error changed";
}


