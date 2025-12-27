/**
 * @file main.cpp
 * @brief نقطه ورود اصلی برنامه برای مدیریت گرافیک و موتور QML.
 * @details این فایل تنظیمات مربوط به OpenGL، بافر استنسیل و لود کردن ماژول اصلی QML را انجام می‌دهد.
 */
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

/**
 * @brief تابع اصلی برنامه.
 * @param argc تعداد آرگومان‌های ورودی.
 * @param argv آرایه آرگومان‌های ورودی.
 * @return int کد خروجی برنامه.
 */
int main(int argc, char *argv[])
{
    // فعال‌سازی پشتیبانی از High DPI برای نمایشگرهای مدرن
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // اتصال سیگنال خطا برای مدیریت شکست در ایجاد آبجکت‌ها
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    //  بهینگی و کیفیت رندر: تنظیمات فرمت سطح (Surface Format)
    QSurfaceFormat format;
    format.setSamples(4);           // فعال‌سازی Anti-aliasing برای نرم شدن لبه‌های دایره
    format.setStencilBufferSize(8);  // اختصاص ۸ بیت برای بافر استنسیل جهت برش (Clipping) دقیق
    QSurfaceFormat::setDefaultFormat(format);

    //  اجبار به استفاده از OpenGL برای هماهنگی کامل با کدهای Scene Graph سفارشی
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    // بارگذاری ماژول اصلی برنامه
    engine.loadFromModule("MarbleTrackDisplay", "Main");

    return app.exec();
}
