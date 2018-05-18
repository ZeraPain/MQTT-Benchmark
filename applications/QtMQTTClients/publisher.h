#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <QtMqtt/QtMqtt>

class Publisher : public QObject
{
    Q_OBJECT
public:
    explicit Publisher(const QString &name, QObject *parent);

    virtual ~Publisher();

    void sendMessage();

public slots:
    void stateChanged(QMqttClient::ClientState state);
    void errorChanged(QMqttClient::ClientError error);

private:
    bool calcPublish();

    QScopedPointer<QMqttClient> m_client;
    QTimer m_publishTimer;

    qint32 m_messageSize;
    qint32 m_publishTimeout;
    qint32 m_publishCounter;
    QString m_name;
};

#endif // PUBLISHER_H
