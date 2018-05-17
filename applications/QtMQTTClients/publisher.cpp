#include "publisher.h"

#include <QDebug>

Publisher::Publisher(QObject *parent)
    : QObject{parent}
    , m_client(new QMqttClient{this})
    , m_publishTimer{}
    , m_messageSize{1}
    , m_publishTimeout{10}
{
    m_publishTimer.setSingleShot(true);
    m_client->setHostname("localhost");
    m_client->setPort(2222);

    connect(m_client.data(), &QMqttClient::stateChanged, this, &Publisher::stateChanged);
    connect(m_client.data(), &QMqttClient::errorChanged, this, &Publisher::errorChanged);
    connect(m_client.data(), &QMqttClient::pingResponseReceived, this, [this]() {
            const QString content = QDateTime::currentDateTime().toString()
                        + QLatin1String(" PingResponse")
                        + QLatin1Char('\n');
            qDebug() << content;
        });

    connect(&m_publishTimer, &QTimer::timeout, this, &Publisher::sendMessage);
    qDebug() << "Client Created";

    m_client->connectToHost();
}

void Publisher::sendMessage()
{
    m_client->publish(QString{"test/test"}, QByteArray{"test"});
}

void Publisher::stateChanged(QMqttClient::ClientState state)
{
    qDebug() << "Client state changed";
    switch (state) {
        case QMqttClient::ClientState::Connected:
            qDebug() << "Client connected";
            m_publishTimer.start(m_publishTimeout);
        break;
        case QMqttClient::ClientState::Connecting:
            qDebug() << "Client connecting";
        break;
        case QMqttClient::ClientState::Disconnected:
            qDebug() << "Client disconnected";
            m_publishTimer.stop();
        break;
        default:
            qDebug() << "Unhandelt state";
        return;
    }
}

void Publisher::errorChanged(QMqttClient::ClientError error)
{
    qDebug() << "Error changed";
    switch(error){
    // Protocol states
    case QMqttClient::ClientError::NoError:
        break;
    case QMqttClient::ClientError::InvalidProtocolVersion:
        break;
    case QMqttClient::ClientError::IdRejected:
        break;
    case QMqttClient::ClientError::ServerUnavailable:
        break;
    case QMqttClient::ClientError::BadUsernameOrPassword:
        break;
    case QMqttClient::ClientError::NotAuthorized:
        break;
        // Qt states
    case QMqttClient::ClientError::TransportInvalid:
        break;
    case QMqttClient::ClientError::ProtocolViolation:
        break;
    case QMqttClient::ClientError::UnknownError:
        break;
    }
}


