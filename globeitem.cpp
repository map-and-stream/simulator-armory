/**
 * @file globeitem.cpp
 * @brief پیاده‌سازی رندرینگ کره زمین و مدیریت نودهای گرافیکی Scene Graph.
 */
#include "globeitem.h"
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGSimpleTextureNode>
#include <QSGClipNode>
#include <QtMath>
#include <QRandomGenerator>

GlobeItem::GlobeItem() {

    setFlag(ItemHasContents);
    generateRandomTracks();

}

void GlobeItem::generateRandomTracks() {

    auto *gen = QRandomGenerator::global();
    m_tracks.clear();

    for (int i = 0; i < 1000; ++i) {
        TrackData track;
        track.color = QColor::fromRgb(gen->generate());
        track.width = 1.0f + (static_cast<float>(gen->generateDouble()) * 2.0f);
        double startLat = -60.0 + (gen->generateDouble() * 120.0);
        double startLon = -180.0 + (gen->generateDouble() * 360.0);
        for (int j = 0; j < 5; ++j) {
            track.points << QGeoCoordinate(startLat + (j * 2.0), startLon + (j * 5.0));
        }
        m_tracks << track;
    }
}
QPointF GlobeItem::project(const QGeoCoordinate &coord) {
    double radius = qMin(width(), height()) / 2.0;
    double latRad = qDegreesToRadians(coord.latitude());
    double lonDiff = coord.longitude() - m_centerLon;

    // نرمال‌سازی طول جغرافیایی در بازه [-180, 180]
    while (lonDiff > 180) lonDiff -= 360;
    while (lonDiff < -180) lonDiff += 360;
    double lonRad = qDegreesToRadians(lonDiff);

    // بررسی اینکه آیا نقطه در نیم‌کره قابل رویت است
    if (qCos(lonRad) < 0) return QPointF(qQNaN(), qQNaN());
    double x = radius * qCos(latRad) * qSin(lonRad);
    double y = -radius * qSin(latRad);


    return QPointF(width() / 2 + x, height() / 2 + y);
}
QSGNode *GlobeItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
    QSGNode *root = oldNode ? oldNode : new QSGNode;

    QSGClipNode *clipNode = nullptr;
    QSGNode *mapContainer = nullptr;
    QSGSimpleTextureNode *texNode1 = nullptr;
    QSGSimpleTextureNode *texNode2 = nullptr;
    QSGNode *tracksContainer = nullptr;

    // مقداردهی اولیه نودها در صورت عدم وجود (اولین فریم)
    if (root->childCount() == 0) {
        clipNode = new QSGClipNode;
        clipNode->setIsRectangular(false);
        QSGGeometry *clipGeo = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 65);
        clipGeo->setDrawingMode(QSGGeometry::DrawTriangleFan);
        clipNode->setGeometry(clipGeo);
        clipNode->setFlag(QSGNode::OwnsGeometry);

        mapContainer = new QSGNode;
        texNode1 = new QSGSimpleTextureNode;
        texNode2 = new QSGSimpleTextureNode;

        QImage img(":/qt/qml/MarbleTrackDisplay/global_map.png");
        QSGTexture *tex = window()->createTextureFromImage(img);
        tex->setHorizontalWrapMode(QSGTexture::Repeat);

        texNode1->setTexture(tex);
        texNode2->setTexture(tex);
        texNode1->setOwnsTexture(true); // مدیریت حافظه Texture توسط نود

        tracksContainer = new QSGNode;

        mapContainer->appendChildNode(texNode1);
        mapContainer->appendChildNode(texNode2);
        clipNode->appendChildNode(mapContainer);
        root->appendChildNode(clipNode);
        root->appendChildNode(tracksContainer);
    } else {
        clipNode = static_cast<QSGClipNode*>(root->childAtIndex(0));
        mapContainer = clipNode->childAtIndex(0);
        texNode1 = static_cast<QSGSimpleTextureNode*>(mapContainer->childAtIndex(0));
        texNode2 = static_cast<QSGSimpleTextureNode*>(mapContainer->childAtIndex(1));
        tracksContainer = root->childAtIndex(1);
    }

    //  هندسه دایره (برش محیطی)
    const float size = static_cast<float>(qMin(width(), height()));
    const float r = size / 2.0f;
    const float cx = static_cast<float>(width() / 2.0f);
    const float cy = static_cast<float>(height() / 2.0f);

    QSGGeometry::Point2D *clipVertices = clipNode->geometry()->vertexDataAsPoint2D();
    clipVertices[0].set(cx, cy);
    for (int i = 1; i < 65; ++i) {
        const float angle = ((i - 1) / 63.0f) * 2.0f * static_cast<float>(M_PI);
        clipVertices[i].set(cx + r * std::cos(angle), cy + r * std::sin(angle));
    }
    clipNode->markDirty(QSGNode::DirtyGeometry);

    //  محاسبه چرخش بافت (Texture Offset)
    float uStart = static_cast<float>(m_centerLon / 360.0f) + 0.25f;
    uStart = uStart - std::floor(uStart);

    const QRectF mapRect(cx - r, cy - r, size, size);

    //  رندر دو لایه جهت حذف درز (Seam) بافت در هنگام چرخش
    QSGGeometry::updateTexturedRectGeometry(texNode1->geometry(), mapRect, QRectF(uStart, 0, 0.5, 1.0));
    const float uStart2 = uStart - 1.0f;
    QSGGeometry::updateTexturedRectGeometry(texNode2->geometry(), mapRect, QRectF(uStart2, 0, 0.5, 1.0));

    texNode1->markDirty(QSGNode::DirtyGeometry);
    texNode2->markDirty(QSGNode::DirtyGeometry);

    //  رسم مسیرها (Tracks)
    tracksContainer->removeAllChildNodes();
    for (const auto &track : m_tracks) {
        QList<QPointF> projectedPoints;
        for (const auto &coord : track.points) {
            const QPointF p = project(coord);
            if (!std::isnan(p.x())) projectedPoints << p;
        }

        if (projectedPoints.size() < 2) continue;

        auto *trackNode = new QSGGeometryNode;
        auto *geo = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), projectedPoints.size());
        geo->setDrawingMode(QSGGeometry::DrawLineStrip);
        geo->setLineWidth(track.width);

        QSGGeometry::Point2D *vertices = geo->vertexDataAsPoint2D();
        for (int i = 0; i < projectedPoints.size(); ++i) {
            vertices[i].set(static_cast<float>(projectedPoints[i].x()),
                            static_cast<float>(projectedPoints[i].y()));
        }

        trackNode->setGeometry(geo);
        trackNode->setFlag(QSGNode::OwnsGeometry);

        auto *mat = new QSGFlatColorMaterial;
        mat->setColor(track.color);
        trackNode->setMaterial(mat);
        trackNode->setFlag(QSGNode::OwnsMaterial);

        tracksContainer->appendChildNode(trackNode);
    }

    return root;
}
