#include "RadarWidget.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>

RadarWidget::RadarWidget(QWidget* parent)
    : QWidget(parent),
    sweepAzimuthDeg(0.0f)
{
    setMinimumSize(600, 600);
}

void RadarWidget::updateFromFrame(const QByteArray& frame) {
    if (frame.size() < 7) {
        qWarning() << "[UI] Frame too small" << frame.size();
        return;
    }

    quint8 type = static_cast<quint8>(frame[6]);
    QByteArray payload = frame.mid(7);

    if (payload.isEmpty()) {
        qWarning() << "[UI] Empty payload for type" << type;
        return;
    }

    if (type == 102) {
        decodeCAT102(payload);
        update();
    }
    else if (type == 101) {
        decodeCAT101(payload);
        update();
    }
}

//
// ====================== CAT102 (sweep + arcdata) ======================
//
void RadarWidget::decodeCAT102(const QByteArray& payload) {
    if (payload.size() < 12) {
        qWarning() << "[UI] CAT102 payload too small" << payload.size();
        return;
    }

    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (p[0] << 8) | p[1];
        p += 2;
        return v;
    };

    auto get32 = [&p]() -> uint32_t {
        uint32_t v = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
        p += 4;
        return v;
    };

    uint16_t radarid       = get16();
    uint16_t azimuth_01deg = get16();
    uint16_t elevation     = get16();
    uint32_t resolution    = get32();
    uint16_t arcLen        = get16();

    int remaining = payload.constEnd() - reinterpret_cast<const char*>(p);
    if (remaining < arcLen) {
        qWarning() << "[UI] CAT102 arcdata truncated";
        return;
    }

    // CLOCKWISE sweep
    sweepAzimuthDeg = azimuth_01deg / 10.0f;

    arcSamples.clear();
    arcSamples.reserve(arcLen);

    for (int i = 0; i < arcLen; ++i) {
        float intensity = p[i] / 255.0f;
        arcSamples.push_back(intensity);
    }

    qDebug() << "[UI] CAT102 decoded. azi =" << sweepAzimuthDeg
             << "arcLen =" << arcLen;
}

//
// ====================== CAT101 (tracks anywhere on PPI) ======================

void RadarWidget::decodeCAT101(const QByteArray& payload) {
    if (payload.size() < 12) {
        qWarning() << "[UI] CAT101 payload too small";
        return;
    }

    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (p[0] << 8) | p[1];
        p += 2;
        return v;
    };

    uint16_t radarid      = get16();
    uint16_t tracknumber  = get16();
    uint16_t range        = get16();   // raw range
    uint16_t azimuth_01deg= get16();
    uint16_t elevation    = get16();

    float azimuthDeg = azimuth_01deg / 10.0f;

    // Normalize range to 0..1 (assume max 5000 for example)
    float rangeNorm = qMin(range / 5000.0f, 1.0f);

    Track t;
    t.azimuthDeg = azimuthDeg;
    t.rangeNorm  = rangeNorm;

    tracks.push_back(t);

    if (tracks.size() > 200)
        tracks.remove(0);
}

//
// ====================== PAINT ======================
//
void RadarWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), Qt::black);

    int w = width();
    int h = height();
    int r = qMin(w, h) / 2 - 10;

    p.translate(w / 2, h / 2);

    // Range rings
    p.setPen(QPen(Qt::green, 1));
    p.drawEllipse(QPoint(0, 0), r, r);
    p.drawEllipse(QPoint(0, 0), r * 2 / 3, r * 2 / 3);
    p.drawEllipse(QPoint(0, 0), r / 3, r / 3);

    // CLOCKWISE sweep line
    p.save();
    p.rotate(sweepAzimuthDeg);
    p.setPen(QPen(Qt::green, 2));
    p.drawLine(0, 0, 0, -r);
    p.restore();

    // ====================== ARC BLIPS ======================
    if (!arcSamples.isEmpty()) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 255, 0));

        float angleRad = qDegreesToRadians(sweepAzimuthDeg);
        float cosA = qCos(angleRad);
        float sinA = qSin(angleRad);

        int n = arcSamples.size();
        for (int i = 0; i < n; ++i) {
            float t = (i + 0.5f) / n;
            float radius = r * t;
            float intensity = arcSamples[i];
            if (intensity < 0.05f)
                continue;

            float x = radius * sinA;
            float y = -radius * cosA;

            float size = 4.0f + 6.0f * intensity;
            p.drawEllipse(QPointF(x, y), size / 2.0f, size / 2.0f);
        }
    }

    // ====================== TRACKS (CAT101) ======================
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::yellow);

    for (const Track& t : tracks) {
        float angleRad = qDegreesToRadians(t.azimuthDeg);
        float radius   = r * t.rangeNorm;

        // Convert polar → Cartesian (PPI coordinates)
        float x = radius * qSin(angleRad);
        float y = -radius * qCos(angleRad);

        // Draw the track point
        p.drawEllipse(QPointF(x, y), 4, 4);


        p.drawEllipse(QPointF(x, y), 4, 4);
    }
}

