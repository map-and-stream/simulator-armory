#include "RadarClient.h"
#include <QDebug>

static const QByteArray MAGIC = QByteArray::fromHex("6E6E6E89");

RadarClient::RadarClient(QObject* parent)
    : QObject(parent)
{
    connect(&socket, &QTcpSocket::readyRead,
            this, &RadarClient::onReadyRead);
}

void RadarClient::connectToServer(const QString& host, int port) {
    socket.connectToHost(host, port);
}

void RadarClient::onReadyRead() {
    buffer.append(socket.readAll());

    while (true) {
        // Need at least 7 bytes for MAGIC(4) + LEN(2) + TYPE(1)
        if (buffer.size() < 7)
            return;

        // Sync to MAGIC
        if (!buffer.startsWith(MAGIC)) {
            buffer.remove(0, 1);
            continue;
        }

        // Now we have MAGIC + at least len + type?
        if (buffer.size() < 7)
            return;

        quint16 len = (quint8)buffer[4] << 8 | (quint8)buffer[5];
        int frameSize = 5 + len;  // 4 magic + 2 len - 1? NO: 4+2+1+len? Check:
        // Your simulator: frame = MAGIC(4) + len(2) + type(1) + payload(len)
        // That means total size = 4 + 2 + 1 + len = 7 + len
        frameSize = 7 + len;

        if (buffer.size() < frameSize)
            return;  // wait for more data

        QByteArray frame = buffer.left(frameSize);
        buffer.remove(0, frameSize);

        if (frame.size() < 7) {
            qWarning() << "[UI] Corrupt frame (too small), skipping";
            continue;
        }

        quint8 type = (quint8)frame[6];
        qDebug() << "[UI] Frame received. Type =" << type
                 << "size =" << frame.size()
                 << "payload =" << len;

        emit frameReceived(frame);
    }
}


