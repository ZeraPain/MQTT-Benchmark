#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <QtMqtt>

class Subscriber : public QMqttClient
{
    Q_OBJECT
public:
    explicit Subscriber(QObject *parent);

    void listenToTopic(const QString &topic);

private slots:
    void printMessage(const QByteArray &message, const QMqttTopicName &topic);
    void stateChanged(ClientState state);
    void errorChanged(ClientError error);
};

#endif // SUBSCRIBER_H
