#pragma once

#include <QMainWindow>
#include "RadarClient.h"
#include "RadarWidget.h"

class RadarWindow : public QMainWindow {
    Q_OBJECT
public:
    RadarWindow();

private:
    RadarClient client;
    RadarWidget* radarWidget;
};
