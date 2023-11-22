#pragma once
#include "src/tcp-ip/TcpIpServer.h"
#include <src/tcp-ip/HtmlServer.h>

class ServerApp
{
public:
    ServerApp() = default;
    ~ServerApp() = default;
    ServerApp(const ServerApp&) = delete;
    ServerApp& operator=(const ServerApp&) = delete;

    void Init();
    void Run();
    void CleanUp();

private:

    bool InitGameServer();
    void HandleGameServer();
    void HandleRecv(ClientPtr sender);
    void CleanUpGameServer();

    bool InitWebServer();
    void HandleWebServer();
    void HandleWebConnection(WebClientPtr sender);
    void CleanUpWebServer();

    TcpIpServer* m_GameServer = nullptr;
    HtmlServer* m_WebServer = nullptr;
};
