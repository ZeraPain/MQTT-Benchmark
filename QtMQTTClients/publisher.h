#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <QtMqtt/QtMqtt>

class Publisher : public QObject
{
    Q_OBJECT
public:
    explicit Publisher(QObject *parent);

    void sendMessage(int bytes);
    void stateChanged(QMqttClient::ClientState state);
    void errorChanged(QMqttClient::ClientError error);

private:
    QScopedPointer<QMqttClient> m_client;
};

#endif // PUBLISHER_H
