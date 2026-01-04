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
    void decodeCAT102(const QByteArray& payload);
    void decodeCAT101(const QByteArray& payload);

    float sweepAzimuthDeg;
    QVector<float> arcSamples;

    struct Track {
        float azimuthDeg;
        float rangeNorm;
    };
    QVector<Track> tracks;
};
