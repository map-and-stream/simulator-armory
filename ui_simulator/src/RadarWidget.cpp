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

    switch (type) {
    case 101: decodeCAT101(payload); break;
    case 102: decodeCAT102(payload); break;
    case 103: decodeCAT103(payload); break;
    case 104: decodeCAT104(payload); break;
    case 105: decodeCAT105(payload); break;
    case 106: decodeCAT106(payload); break;
    case 107: decodeCAT107(payload); break;
    case 108: decodeCAT108(payload); break;
    case 109: decodeCAT109(payload); break;
    case 110: decodeCAT110(payload); break;
    case 111: decodeCAT111(payload); break;
    case 112: decodeCAT112(payload); break;
    case 113: decodeCAT113(payload); break;
    case 114: decodeCAT114(payload); break;
    default:
        qWarning() << "[UI] Unknown CAT" << type;
        break;
    }

    update();
}

//
// ====================== CAT102 (sweep + arcdata, binary) ======================
//
void RadarWidget::decodeCAT102(const QByteArray& payload) {
    if (payload.size() < 12) {
        qWarning() << "[UI] CAT102 payload too small" << payload.size();
        return;
    }

    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    auto get32 = [&p]() -> uint32_t {
        uint32_t v = (uint32_t(p[0]) << 24) |
                     (uint32_t(p[1]) << 16) |
                     (uint32_t(p[2]) << 8)  |
                     uint32_t(p[3]);
        p += 4;
        return v;
    };

    uint16_t radarid       = get16();
    uint16_t azimuth_01deg = get16();
    uint16_t elevation     = get16();
    uint32_t resolution    = get32();
    uint16_t arcLen        = get16();

    Q_UNUSED(radarid);
    Q_UNUSED(elevation);
    Q_UNUSED(resolution);

    int remaining = payload.constEnd() -
                    reinterpret_cast<const char*>(p);
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

    // qDebug() << "[UI] CAT102 decoded. azi =" << sweepAzimuthDeg
    //          << "arcLen =" << arcLen;
}

//
// ====================== CAT101 (tracks, binary simplified) ======================
//
void RadarWidget::decodeCAT101(const QByteArray& payload) {
    if (payload.size() < 10) {
        qWarning() << "[UI] CAT101 payload too small" << payload.size();
        return;
    }

    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid      = get16();
    uint16_t tracknumber  = get16();
    uint16_t range        = get16();
    uint16_t azimuth_01deg= get16();
    uint16_t elevation    = get16();

    Q_UNUSED(radarid);
    Q_UNUSED(elevation);

    float azimuthDeg = azimuth_01deg / 10.0f;

    // Normalize range to 0..1 (assume max 5000 for example)
    float rangeNorm = qMin(range / 5000.0f, 1.0f);

    Track t;
    t.azimuthDeg = azimuthDeg;
    t.rangeNorm  = rangeNorm;
    t.trackNumber= tracknumber;

    tracks.push_back(t);

    if (tracks.size() > 200)
        tracks.remove(0);
}

//
// ====================== CAT103 (clusters, simple binary) ======================
//
// Assume layout: radarid(2) + x(2) + y(2)
// Adjust if your encoder uses 32-bit coordinates.
void RadarWidget::decodeCAT103(const QByteArray& payload) {
    if (payload.size() < 6) {
        qWarning() << "[UI] CAT103 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid = get16();
    int16_t  x       = static_cast<int16_t>(get16());
    int16_t  y       = static_cast<int16_t>(get16());

    Q_UNUSED(radarid);

    Cluster c;
    c.x = x;
    c.y = y;
    clusters.push_back(c);

    if (clusters.size() > 200)
        clusters.remove(0);
}

//
// ====================== CAT104 (radar status) ======================
//
// Assume layout:
// radarid(2), latitude(4), longitude(4), altitude(2),
// status(2), temperature(2), sectorleft(2), sectorright(2),
// rpm(2), maxrange(2)
void RadarWidget::decodeCAT104(const QByteArray& payload) {
    if (payload.size() < 22) {
        qWarning() << "[UI] CAT104 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    auto get32 = [&p]() -> uint32_t {
        uint32_t v = (uint32_t(p[0]) << 24) |
                     (uint32_t(p[1]) << 16) |
                     (uint32_t(p[2]) << 8)  |
                     uint32_t(p[3]);
        p += 4;
        return v;
    };

    uint16_t radarid = get16();
    uint32_t lat     = get32();
    uint32_t lon     = get32();
    uint16_t alt     = get16();
    uint16_t status  = get16();
    uint16_t temp    = get16();
    uint16_t sleft   = get16();
    uint16_t sright  = get16();
    uint16_t rpm     = get16();
    uint16_t maxrng  = get16();

    Q_UNUSED(radarid);
    Q_UNUSED(lat);
    Q_UNUSED(lon);
    Q_UNUSED(alt);
    Q_UNUSED(sleft);
    Q_UNUSED(sright);
    Q_UNUSED(maxrng);

    radarStatus.status      = status;
    radarStatus.temperature = temp;
    radarStatus.rpm         = rpm;
}

//
// ====================== CAT105 (parameter/LED) ======================
//
// Not directly visualized on PPI, can be logged only.
// Assume layout: radarid(2), parameter(2), led(2)
void RadarWidget::decodeCAT105(const QByteArray& payload) {
    if (payload.size() < 6) {
        qWarning() << "[UI] CAT105 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid   = get16();
    uint16_t parameter = get16();
    uint16_t led       = get16();

    Q_UNUSED(radarid);
    Q_UNUSED(parameter);
    Q_UNUSED(led);

    // You can log or later overlay text if needed.
}

//
// ====================== CAT106 (pointing / beam control) ======================
//
// Assume: radarid(2), azimuth(2), elevation(2)
void RadarWidget::decodeCAT106(const QByteArray& payload) {
    if (payload.size() < 6) {
        qWarning() << "[UI] CAT106 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid = get16();
    uint16_t az      = get16();
    uint16_t el      = get16();

    Q_UNUSED(radarid);
    Q_UNUSED(az);
    Q_UNUSED(el);

    // Could be used to draw a second beam, if desired.
}

//
// ====================== CAT107 (TX control) ======================
//
// Assume: radarid(2), tx(2)
void RadarWidget::decodeCAT107(const QByteArray& payload) {
    if (payload.size() < 4) {
        qWarning() << "[UI] CAT107 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid = get16();
    uint16_t tx      = get16();

    Q_UNUSED(radarid);
    Q_UNUSED(tx);

    // Could visualize TX on/off as indicator later.
}

//
// ====================== CAT108 (IFF / identification) ======================
//
// Assume: radarid(2), tracknumber(2), type(2), foe(2)
void RadarWidget::decodeCAT108(const QByteArray& payload) {
    if (payload.size() < 8) {
        qWarning() << "[UI] CAT108 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid    = get16();
    uint16_t track      = get16();
    uint16_t type       = get16();
    uint16_t foe        = get16();

    Q_UNUSED(radarid);
    Q_UNUSED(track);
    Q_UNUSED(type);
    Q_UNUSED(foe);

    // Could color-code tracks later by friend/foe.
}

//
// ====================== CAT109 (track confirm) ======================
//
// Assume: radarid(2), tracknumber(2)
void RadarWidget::decodeCAT109(const QByteArray& payload) {
    if (payload.size() < 4) {
        qWarning() << "[UI] CAT109 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid = get16();
    uint16_t track   = get16();

    Q_UNUSED(radarid);
    Q_UNUSED(track);
}

//
// ====================== CAT110 (3D Cartesian position) ======================
//
// Assume: radarid(2), x(2), y(2), z(2)
void RadarWidget::decodeCAT110(const QByteArray& payload) {
    if (payload.size() < 8) {
        qWarning() << "[UI] CAT110 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> int16_t {
        int16_t v = int16_t((uint16_t(p[0]) << 8) | uint16_t(p[1]));
        p += 2;
        return v;
    };

    uint16_t radarid = static_cast<uint16_t>(get16());
    int16_t  x       = get16();
    int16_t  y       = get16();
    int16_t  z       = get16();

    Q_UNUSED(radarid);

    Position3D pos;
    pos.x = x;
    pos.y = y;
    pos.z = z;

    positions3D.push_back(pos);
    if (positions3D.size() > 200)
        positions3D.remove(0);
}

//
// ====================== CAT111 (sector / mode) ======================
//
// Assume: radarid(2), sectorleft(2), sectorright(2), rpm(2), mode(2), enable(2)
void RadarWidget::decodeCAT111(const QByteArray& payload) {
    if (payload.size() < 12) {
        qWarning() << "[UI] CAT111 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid = get16();
    uint16_t left    = get16();
    uint16_t right   = get16();
    uint16_t rpm     = get16();
    uint16_t mode    = get16();
    uint16_t enable  = get16();

    Q_UNUSED(radarid);
    Q_UNUSED(rpm);
    Q_UNUSED(enable);

    sectorControl.left  = left;
    sectorControl.right = right;
    sectorControl.mode  = mode;
}

//
// ====================== CAT112 (wind / motion) ======================
//
// Assume: radarid(2), speed(2), direction(2)
void RadarWidget::decodeCAT112(const QByteArray& payload) {
    if (payload.size() < 6) {
        qWarning() << "[UI] CAT112 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid   = get16();
    uint16_t speed     = get16();
    uint16_t direction = get16();

    Q_UNUSED(radarid);

    wind.speed     = speed;
    wind.direction = direction;
}

//
// ====================== CAT113 (threshold) ======================
//
// Assume: radarid(2), threshold(2)
void RadarWidget::decodeCAT113(const QByteArray& payload) {
    if (payload.size() < 4) {
        qWarning() << "[UI] CAT113 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid  = get16();
    uint16_t thr      = get16();

    Q_UNUSED(radarid);

    threshold = thr;
}

//
// ====================== CAT114 (heartbeat) ======================
//
// Assume: radarid(2), messagenumber(2)
void RadarWidget::decodeCAT114(const QByteArray& payload) {
    if (payload.size() < 4) {
        qWarning() << "[UI] CAT114 payload too small" << payload.size();
        return;
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    auto get16 = [&p]() -> uint16_t {
        uint16_t v = (uint16_t(p[0]) << 8) | uint16_t(p[1]);
        p += 2;
        return v;
    };

    uint16_t radarid = get16();
    uint16_t msgno   = get16();

    Q_UNUSED(radarid);

    lastHeartbeat = msgno;
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

    // ====================== Sector (CAT111) ======================
    if (sectorControl.left != sectorControl.right) {
        // sector angles in 0.1 deg, convert to degrees
        float leftDeg  = sectorControl.left / 10.0f;
        float rightDeg = sectorControl.right / 10.0f;

        // Qt uses 0 at 3 o'clock, positive CCW; radar 0 usually at 12 o'clock.
        // We'll approximate by rotating angles.
        float startDeg = -rightDeg; // negative for clockwise
        float spanDeg  = rightDeg - leftDeg;

        p.save();
        p.setPen(QPen(Qt::darkGreen, 2, Qt::DashLine));
        p.drawArc(-r, -r, 2*r, 2*r,
                  int(startDeg * 16),
                  int(spanDeg * 16));
        p.restore();
    }

    // CLOCKWISE sweep line (CAT102)
    p.save();
    p.rotate(sweepAzimuthDeg);
    p.setPen(QPen(Qt::green, 2));
    p.drawLine(0, 0, 0, -r);
    p.restore();

    // ====================== ARC BLIPS (CAT102) ======================
    if (!arcSamples.isEmpty()) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 0)); // Yellow
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

        float x = radius * qSin(angleRad);
        float y = -radius * qCos(angleRad);

        p.drawEllipse(QPointF(x, y), 4, 4);
    }

    // ====================== CLUSTERS (CAT103) ======================
    // Interpret (x,y) as Cartesian in some arbitrary units, normalized to radius.
    if (!clusters.isEmpty()) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 128, 0)); // orange
        float maxCoord = 10000.0f;       // adjust as needed

        for (const Cluster& c : clusters) {
            float nx = qBound(-1.0f, c.x / maxCoord, 1.0f);
            float ny = qBound(-1.0f, c.y / maxCoord, 1.0f);

            float x = nx * r;
            float y = -ny * r;

            p.drawEllipse(QPointF(x, y), 3, 3);
        }
    }

    // ====================== 3D positions (CAT110) ======================
    // Project x,y like clusters; z is not used visually here, but could color-code.
    if (!positions3D.isEmpty()) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 255, 255)); // cyan
        float maxCoord = 10000.0f;

        for (const Position3D& pos : positions3D) {
            float nx = qBound(-1.0f, pos.x / maxCoord, 1.0f);
            float ny = qBound(-1.0f, pos.y / maxCoord, 1.0f);

            float x = nx * r;
            float y = -ny * r;

            p.drawEllipse(QPointF(x, y), 4, 4);
        }
    }

    // ====================== Threshold ring (CAT113) ======================
    if (threshold > 0) {
        float thrNorm = qMin(threshold / 255.0f, 1.0f); // assume 0..255
        int thrRadius = int(r * thrNorm);

        p.setPen(QPen(Qt::red, 1, Qt::DashLine));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPoint(0, 0), thrRadius, thrRadius);
    }

    // ====================== Wind arrow (CAT112) ======================
    if (wind.speed > 0) {
        p.save();
        // direction in 0.1 degrees: convert to degrees
        float dirDeg = wind.direction / 10.0f;
        float dirRad = qDegreesToRadians(dirDeg);

        float len = r * qMin(wind.speed / 50.0f, 1.0f); // scale speed
        float x = len * qSin(dirRad);
        float y = -len * qCos(dirRad);

        p.setPen(QPen(Qt::cyan, 2));
        p.drawLine(QPointF(0, 0), QPointF(x, y));

        // small arrow head
        QPointF tip(x, y);
        QPointF left(tip.x() - 5, tip.y() + 5);
        QPointF right(tip.x() + 5, tip.y() + 5);
        p.drawLine(tip, left);
        p.drawLine(tip, right);

        p.restore();
    }

    // ====================== Overlay: status + heartbeat ======================
    p.resetTransform();
    p.setPen(Qt::green);
    QFont f = p.font();
    f.setPointSize(10);
    p.setFont(f);

    QString statusText = QString("RPM: %1  TEMP: %2  STATUS: %3")
                             .arg(radarStatus.rpm)
                             .arg(radarStatus.temperature)
                             .arg(radarStatus.status);
    p.drawText(10, 20, statusText);

    QString hbText = QString("HB: %1").arg(lastHeartbeat);
    p.drawText(10, 40, hbText);

    // Heartbeat indicator
    if (lastHeartbeat > 0) {
        p.setBrush(Qt::green);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(width() - 20, 20), 6, 6);
    }
}

