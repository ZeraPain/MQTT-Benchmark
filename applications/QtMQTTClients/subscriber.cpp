#include "subscriber.h"

#include <QDebug>

Subscriber::Subscriber(QObject *parent)
    : QObject{parent}
    , m_client{new QMqttClient{this}}
{
    m_client->setHostname("127.0.0.1");
    m_client->setPort(2222);

    connect(m_client.data(), &QMqttClient::stateChanged, this, &Subscriber::stateChanged);
    connect(m_client.data(), &QMqttClient::errorChanged, this, &Subscriber::errorChanged);
    connect(m_client.data(), &QMqttClient::messageReceived, this, &Subscriber::printMessage);
    connect(m_client.data(), &QMqttClient::pingResponseReceived, this, [this]() {
            const QString content = QDateTime::currentDateTime().toString()
                        + QLatin1String(" PingResponse")
                        + QLatin1Char('\n');
            qDebug() << content;
        });
    qDebug() << "Client Created";

    m_client->connectToHost();
}

void Subscriber::listenToTopic(const QString &topic)
{
    auto subscription = m_client->subscribe(topic);
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
        case QMqttClient::ClientState::Connected:
            qDebug() << "Client connected";
            listenToTopic("test/test");
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

void Subscriber::errorChanged(QMqttClient::ClientError error)
{
    qDebug() << "Error changed";
    switch(error){
    // Protocol states
    case QMqttClient::ClientError::NoError:
        qDebug() << "No error";
        break;
    case QMqttClient::ClientError::InvalidProtocolVersion:
        qDebug() << "Invalid protocol version";
        break;
    case QMqttClient::ClientError::IdRejected:
        qDebug() << "Id rejected";
        break;
    case QMqttClient::ClientError::ServerUnavailable:
        qDebug() << "Server unavailable";
        break;
    case QMqttClient::ClientError::BadUsernameOrPassword:
        qDebug() << "Bad username or password";
        break;
    case QMqttClient::ClientError::NotAuthorized:
        qDebug() << "Not authorized";
        break;
        // Qt states
    case QMqttClient::ClientError::TransportInvalid:
        qDebug() << "Transport invalid";
        break;
    case QMqttClient::ClientError::ProtocolViolation:
        qDebug() << "Protocol violation";
        break;
    case QMqttClient::ClientError::UnknownError:
        qDebug() << "Unknown error";
        break;
    }
}

