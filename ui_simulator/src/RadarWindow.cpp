#include "RadarWindow.h"

RadarWindow::RadarWindow() {
    radarWidget = new RadarWidget(this);
    setCentralWidget(radarWidget);

    resize(800, 800);
    setWindowTitle("Radar Simulator UI");

    connect(&client, &RadarClient::frameReceived,
            radarWidget, &RadarWidget::updateFromFrame);

    client.connectToServer("127.0.0.1", 3040);
}
