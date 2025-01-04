#include "WiFi.h"
#include <WebServer.h>

class WebServerMenu
{
public:
    WebServerMenu(WebServer *server);
    void update();
    void begin();

private:
    WebServer *server;

    void handleRoot();
    void handleSubmit();
    void handleHeartbeat();
    void handleStartStop();
};
