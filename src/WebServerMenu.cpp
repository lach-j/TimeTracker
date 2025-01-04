#include "WebServerMenu.h"
#include "TimeTracker.h"
#include "Config.h"
#include <WebServer.h>
#include <uri/UriBraces.h>

extern TimeTracker timeTracker;
extern Config config;

WebServerMenu::WebServerMenu(WebServer *server) : server(server)
{
}

void WebServerMenu::begin()
{
    server->on("/", HTTP_GET, std::bind(&WebServerMenu::handleRoot, this));
    server->on("/submit", HTTP_POST, std::bind(&WebServerMenu::handleSubmit, this));
    server->on("/heartbeat", HTTP_GET, std::bind(&WebServerMenu::handleHeartbeat, this));

    server->on(UriBraces("/startstop/{}"), std::bind(&WebServerMenu::handleStartStop, this));
    server->begin();
}

void WebServerMenu::handleStartStop()
{
    String id = server->pathArg(0);

    timeTracker.startStopActivity(std::stoi(id.c_str()));

    server->send(200);
}

void WebServerMenu::handleHeartbeat()
{
    TrackedActivity activity = timeTracker.getActiveActivity();
    server->send(200, "application/json", "{\"duration\":" + String(activity.duration) + ", \"activeActivity\":\"" + activity.name + "\" }");
}

void WebServerMenu::handleRoot()
{
    String html = R"(<!DOCTYPE html>
    <html>
        <head>
            <meta name="viewport" content="width=device-width, initial-scale=1">
            <link rel="icon" href="data:,">
            <style>
html {
    font-family: Helvetica;
    display: inline-block;
    margin: 0px auto;
    text-align: center;
}
.button {
    background-color: #4CAF50;
    border: none;
    color: white;
    padding: 16px 40px;
    text-decoration: none;
    font-size: 30px;
    margin: 2px;
    cursor: pointer;
}
.button2 {
    background-color: #555555;
}

.startstop {
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 20px;
}
form {
    margin: 20px auto;
    max-width: 400px;
}
input {
    margin: 10px 0;
    padding: 5px;
    width: 100%;
}
            </style>
            <script>
function formatSeconds(millis) {
    const seconds = Math.floor(millis / 1000);
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const remainingSeconds = seconds % 60;
    
    return [hours, minutes, remainingSeconds]
        .map(v => String(v).padStart(2, '0'))
        .join(':');
}

function startStopActivity(i)
{
    fetch(`/startstop/${i}`, { method: 'POST' });
}

function fetchTime() {
    setTimeout(() => fetch("/heartbeat")
        .then(x => x.json())
        .then(x => {
            document.getElementById("activity").innerText = x.activeActivity;
            document.getElementById("timer").innerText = formatSeconds(x.duration);
            fetchTime();
        })
        .catch(() => fetchTime())
    , 1000);
}
fetchTime();
            </script>
        </head>

        <body>
            <h1>Current Times</h1>

            <p id="activity"></p>
            <p id="timer"></p>

    <form action='/submit' method='POST'>)";

    for (int i = 0; i < 4; i++)
    {
        html += "<button class='startstop' type='button' onclick=\"startStopActivity(" + String(i) + ")\"><input type='text' name='activity" + String(i) +
                "' value='" + timeTracker.getActivity(i).name +
                "' placeholder='Activity " + String(i + 1) + "'></button>";
    }
    html += R"(<input type='submit' value='Update Activities' class='button'>
    </form>

    </body></html>)";

    server->send(200, "text/html", html);
}

void WebServerMenu::handleSubmit()
{
    // Parse form data
    for (int i = 0; i < 4; i++)
    {
        String activityName = server->arg("activity" + String(i));
        if (activityName.length() > 0)
        {
            // URL decode is handled automatically by the WebServer library
            config.setActivityName(i, activityName);
            timeTracker.setActivityName(i, activityName);
        }
    }

    // Redirect back to home page
    server->sendHeader("Location", "/");
    server->send(303);
}

void WebServerMenu::update()
{
    server->handleClient();
}