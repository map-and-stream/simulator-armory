#pragma once

#include <QWidget>
#include <QVector>

class RadarWidget : public QWidget {
    Q_OBJECT
public:
    explicit RadarWidget(QWidget* parent = nullptr);

public slots:
    void updateFromFrame(const QByteArray& frame);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    // === CAT decoders ===
    void decodeCAT101(const QByteArray& payload);
    void decodeCAT102(const QByteArray& payload);
    void decodeCAT103(const QByteArray& payload);
    void decodeCAT104(const QByteArray& payload);
    void decodeCAT105(const QByteArray& payload);
    void decodeCAT106(const QByteArray& payload);
    void decodeCAT107(const QByteArray& payload);
    void decodeCAT108(const QByteArray& payload);
    void decodeCAT109(const QByteArray& payload);
    void decodeCAT110(const QByteArray& payload);
    void decodeCAT111(const QByteArray& payload);
    void decodeCAT112(const QByteArray& payload);
    void decodeCAT113(const QByteArray& payload);
    void decodeCAT114(const QByteArray& payload);

    // === CAT102 sweep ===
    float sweepAzimuthDeg;
    QVector<float> arcSamples;

    // === CAT101 tracks ===
    struct Track {
        float azimuthDeg;
        float rangeNorm;
        int   trackNumber;
    };
    QVector<Track> tracks;

    // === CAT103 clusters ===
    struct Cluster {
        int x;
        int y;
    };
    QVector<Cluster> clusters;

    // === CAT110 3D positions ===
    struct Position3D {
        int x;
        int y;
        int z;
    };
    QVector<Position3D> positions3D;

    // === CAT104 radar status ===
    struct RadarStatus {
        int rpm = 0;
        int temperature = 0;
        int status = 0;
    } radarStatus;

    // === CAT111 sector control ===
    struct SectorControl {
        int left = 0;    // 0.1 deg
        int right = 3599;// 0.1 deg
        int mode = 0;
    } sectorControl;

    // === CAT112 wind / motion ===
    struct Wind {
        int speed = 0;      // arbitrary units
        int direction = 0;  // 0.1 deg
    } wind;

    // === CAT113 threshold ===
    int threshold = 0;  // arbitrary units

    // === CAT114 heartbeat ===
    int lastHeartbeat = 0;
};
