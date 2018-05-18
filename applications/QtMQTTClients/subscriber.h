#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <QtMqtt>

class Subscriber : public QObject
{
    Q_OBJECT
public:
    explicit Subscriber(const QString &name, QObject *parent);

    virtual ~Subscriber();

    void listenToTopic(const QString &topic);

private slots:
    void printMessage(const QByteArray &message, const QMqttTopicName &topic);
    void stateChanged(QMqttClient::ClientState state);
    void errorChanged(QMqttClient::ClientError error);

private:
    QScopedPointer<QMqttClient> m_client;
    QString m_name;
};

#endif // SUBSCRIBER_H
