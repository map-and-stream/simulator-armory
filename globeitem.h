/**
 * @file globeitem.h
 * @brief تعریف کلاس رندرکننده کره زمین و ساختارهای داده‌ای مرتبط.
 */

#ifndef GLOBEITEM_H
#define GLOBEITEM_H

#include <QQuickItem>
#include <QGeoCoordinate>
#include <QColor>

/**
 * @struct TrackData
 * @brief ساختار نگهدارنده اطلاعات یک مسیر پروازی/حرکتی.
 */
struct TrackData {
    QList<QGeoCoordinate> points; ///< لیست مختصات جغرافیایی مسیر
    QColor color;                 ///< رنگ نمایشی مسیر
    float width;                  ///< ضخامت خط مسیر
};

/**
 * @class GlobeItem
 * @brief کلاس مدیریت رندرینگ بصری کره زمین و مسیرها در Qt Quick Scene Graph.
 * @details این کلاس با استفاده از QSGNode کدهای گرافیکی بهینه را به GPU ارسال می‌کند.
 */
class GlobeItem : public QQuickItem {

    Q_OBJECT
    /**
     * @property centerLon
     * @brief طول جغرافیایی مرکز کره زمین برای چرخش حول محور Y.
     */
    Q_PROPERTY(double centerLon READ centerLon WRITE setCenterLon NOTIFY centerChanged)

    QML_ELEMENT

public:
    /**
     * @brief سازنده پیش‌فرض GlobeItem.
     */
    GlobeItem();

    /**
     * @brief دریافت طول جغرافیایی فعلی مرکز.
     * @return مقدار طول جغرافیایی به صورت double.
     */
    double centerLon() const { return m_centerLon; }

    /**
     * @brief تنظیم طول جغرافیایی مرکز و درخواست رندر مجدد.
     * @param lon مقدار طول جغرافیایی جدید.
     */
    void setCenterLon(double lon) {
        if (m_centerLon != lon) { m_centerLon = lon; emit centerChanged(); update(); }
    }

signals:

    /**
     * @brief سیگنال تغییر طول جغرافیایی مرکز.
     */
    void centerChanged();

protected:
    /**
     * @brief بروزرسانی نودهای گرافیکی در Scene Graph.
     * @param oldNode نود قدیمی رندر شده (در صورت وجود برای بازیافت).
     * @param updateData داده‌های کمکی بروزرسانی.
     * @return QSGNode* نود جدید یا بروز شده برای رندر.
     */
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    double m_centerLon = 50.0; ///< طول جغرافیایی مرکز (پیش‌فرض روی ۵۰ درجه شرقی)
    QList<TrackData> m_tracks; ///< نگهدارنده داده‌های ۱۰۰۰ مسیر جهت رندر بهینه

    /**
     * @brief تولید داده‌های تصادفی برای تست رندرینگ ۱۰۰۰ مسیر.
     */
    void generateRandomTracks();

    /**
     * @brief تبدیل مختصات جغرافیایی به پوینت‌های محلی دو بعدی (Projection).
     * @param coord مختصات ورودی شامل طول و عرض جغرافیایی.
     * @return QPointF مختصات نگاشت شده روی صفحه.
     */
    QPointF project(const QGeoCoordinate &coord);
};

#endif
