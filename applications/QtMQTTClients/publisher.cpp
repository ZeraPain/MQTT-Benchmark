#include "publisher.h"

#include <QDebug>

Publisher::Publisher(const QString &name, QObject *parent)
    : QObject{parent}
    , m_client(new QMqttClient{this})
    , m_publishTimer{}
    , m_messageSize{0}
    , m_publishTimeout{10}
    , m_publishCounter{0}
    , m_name{name}
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
    calcPublish();

    QByteArray data;
    data.resize(m_messageSize);
    data.fill('c');

    m_client->publish(QString{"test"}, data);
    m_publishTimer.start(m_publishTimeout);
}

void Publisher::stateChanged(QMqttClient::ClientState state)
{
    qDebug() << "Client state changed";
    switch (state) {
    case QMqttClient::ClientState::Connected:
        qDebug() << m_name <<  "connected";
        m_publishTimer.start(m_publishTimeout);
        break;
    case QMqttClient::ClientState::Connecting:
        qDebug() << m_name <<  "connecting";
        break;
    case QMqttClient::ClientState::Disconnected:
        qDebug() << m_name <<  "disconnected";
        m_publishTimer.stop();
        break;
    default:
        qDebug() << m_name <<  "Unhandelt state";
        return;
    }
}

void Publisher::errorChanged(QMqttClient::ClientError error)
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

void Publisher::calcPublish()
{
    if(m_publishCounter == 10){
        m_publishCounter = 0;

        // If message size is 10^7 reset to 0 and reduce publish intervall
        if(m_messageSize == 10000000){
            m_publishTimeout *= 10;
            m_messageSize = 0;
            qDebug() << m_name << "Publish timeout set to:" << m_publishTimeout;
        }
        else if(m_messageSize == 0)
            m_messageSize = 1;
        else if(m_messageSize == 1)
            m_messageSize = 2;
        else if(m_messageSize == 2)
            m_messageSize = 10;
        else if(m_messageSize > 2)
            m_messageSize *= 10;

        // Quit application if publishTimeout is higher than 1000
        if(m_publishTimeout > 1000){
            m_publishTimer.stop();
            qDebug() << m_name << "stopped";
        }

        qDebug() << m_name << "Message size set to" << m_messageSize;
    }
    m_publishCounter++;
}


