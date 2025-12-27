/**
 * @file Main.qml
 * @brief رابط کاربری اصلی پروژه رندرینگ کره زمین.
 * @details این فایل شامل المان بصری GlobeItem، کنترل‌های موس برای چرخش و افکت‌های بصری اتمسفر است.
 */

import QtQuick
import QtQuick.Controls
import MarbleTrackDisplay

Window {
    id: root
    width: 1024
    height: 768
    visible: true
    title: qsTr("GPU Accelerated Globe Renderer")
    color: "#050505" // رنگ پس‌زمینه تیره برای جلوه بهتر اتمسفر

    // کانتینر اصلی جهت مدیریت ابعاد و مرکزیت کره
    Item {
        id: globeContainer
        width: Math.min(parent.width, parent.height) * 0.8
        height: width
        anchors.centerIn: parent

        // ۱. المان اصلی رندر شده در C++ (Scene Graph)
        GlobeItem {
            id: globe
            anchors.fill: parent
            centerLon: 0

            // کنترل تعاملی چرخش کره با استفاده از کشیدن موس
            MouseArea {
                anchors.fill: parent
                property real lastX: 0

                onPressed: (mouse) => lastX = mouse.x

                onPositionChanged: (mouse) => {
                    if (pressed) {
                        let dx = mouse.x - lastX
                        // ضریب 0.4 برای سرعت چرخش نرم و بهینه
                        globe.centerLon -= dx * 0.4
                        lastX = mouse.x
                    }
                }
            }
        }

        // ۲. افکت هاله اتمسفر (Atmospheric Glow)
        Rectangle {
            anchors.fill: globe
            radius: width / 2
            color: "transparent"
            border.color: "#FFFFFF" // سفید با شفافیت برای لبه‌ها
            border.width: 1
            opacity: 0.3
            z: 5
        }

        // ۳. حلقه محیطی تزئینی برای حس عمق
        Rectangle {
            anchors.fill: globe
            radius: width / 2
            color: "transparent"
            border.color: "#FFFFFF"
            border.width: 2
            scale: 1.02 // کمی بزرگتر از بدنه اصلی کره
            z: 4
        }
    }

    // ۴. پنل اطلاعات وضعیت رندرینگ در پایین صفحه
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        width: statusLabel.width + 40
        height: 46
        color: "transparent" // پس‌زمینه نیمه شفاف
        border.color: "#FFFFFF"
        border.width: 1
        radius: 5

        Label {
            id: statusLabel
            anchors.centerIn: parent
            text: "Tracks Loaded: 1000 | Render API: OpenGL | Mode: Scene Graph (GPU)"
            color: "white"
            font.pixelSize: 14
            font.family: "Segoe UI, Arial"
        }
    }
}
