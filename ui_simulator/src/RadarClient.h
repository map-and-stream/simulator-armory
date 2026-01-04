#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>

class RadarClient : public QObject {
    Q_OBJECT
public:
    explicit RadarClient(QObject* parent = nullptr);

    void connectToServer(const QString& host, int port);

signals:
    void frameReceived(const QByteArray& frame);

private slots:
    void onReadyRead();

private:
    QTcpSocket socket;
    QByteArray buffer;
};
