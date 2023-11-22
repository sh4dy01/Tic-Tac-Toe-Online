#include "ClientApp.h"
#include "Window.h"
#include "src/core/StateMachine/AppStates/ConnectionState.h"
#include "src/core/StateMachine/AppStates/EndState.h"
#include "src/core/StateMachine/AppStates/GameState.h"
#include "src/core/StateMachine/AppStates/HistoryState.h"
#include "src/core/StateMachine/AppStates/MenuState.h"
#include "src/core/StateMachine/AppStates/LobbyState.h"
#include "src/tcp-ip/TcpIpClient.h"
#include "threading/Thread.h"

using namespace TicTacToe;

void ClientApp::Init()
{
    FontRegistry::LoadFont("bold-font");

    m_IsRunning = true;
    m_SharedIsRunning.WaitGet().Get() = true;

    m_TimeManager.Init();

    m_Window = new Window();
    m_Window->Create("Tic Tac Toe Online!", 1280, 720);

    m_GameSettings.SetGameMode(GAMEMODE_CLASSIC);
    
    m_StateMachine = new Shared<StateMachine>();
    {
        auto lock = m_StateMachine->WaitGet();
        lock->AddState("MenuState", new MenuState(&lock.Get(), m_Window));
        lock->AddState("LobbyState", new LobbyState(&lock.Get(), m_Window));
        lock->AddState("ConnectionState", new ConnectionState(&lock.Get(), m_Window));
        lock->AddState("GameState", new GameState(&lock.Get(), m_Window));
        lock->AddState("HistoryState", new HistoryState(&lock.Get(), m_Window));
        lock->AddState("EndState", new EndState(&lock.Get(), m_Window));
        lock->InitState("MenuState");
        lock->Start();
    }
}

void ClientApp::Run()
{
    if (!m_IsRunning)
        throw std::runtime_error("ClientApp is not initialized!");

    while (m_IsRunning)
    {
        m_TimeManager.Tick();

        m_Window->PollEvents();
        m_InputHandler.Update();

        Update(TimeManager::GetDeltaTime());
        m_Window->Render();
        m_IsRunning &= m_Window->IsOpen();
        auto sharedRunning = m_SharedIsRunning.TryGet();
        if (sharedRunning.IsValid())
            m_IsRunning &= sharedRunning.Get();

    }

    m_SharedIsRunning.WaitGet().Get() = false;
    Cleanup();
}

void ClientApp::RunClient(std::string* adress)
{
    DebugLog("ip in runClient " + *adress + "...\n");

    try
    {
        m_Client = new TcpIpClient();
        m_Client->Connect(adress->c_str(), DEFAULT_PORT);
        m_IsClientRunning = true;
        m_IsClientConnected.WaitGet().Get() = true;
        DebugLog("Connected to server!\n");
    }
    catch (const TcpIp::TcpIpException& e)
    {
        DebugLog("Failed to connect to server: " + std::string(e.what()) + "\n");
        m_IsClientRunning = false;
        m_IsClientConnected.WaitGet().Get() = false;
    }

    std::stringstream ss;
    while (m_IsClientRunning)
    {
        try
        {
            while (m_Client->FetchPendingData(ss))
            {
                Json j;
                try
                {
                    j = Json::parse(ss);
                }
                catch (const std::exception& e)
                {
                    DebugLog("Failed to parse JSON from server!");
                    return;
                }
                if (!j.contains("Type"))
                {
                    DebugLog("Json does not contain a Type!\n");
                    return;
                }
                m_StateMachine->WaitGet()->OnReceiveData(j);
                ss.str(std::string());
            }
        }
        catch (const TcpIp::TcpIpException& e)
        {
            DebugLog("Failed to fetch data from server: " + std::string(e.what()) + "\n");
            m_IsClientRunning = false;
        }

        if (!m_Client->IsConnected())
        {
            DebugLog("Disconnected from server!\n");
            m_IsClientRunning = false;
        }

        auto sharedRunning = m_SharedIsRunning.TryGet();
        if (sharedRunning.IsValid())
            m_IsClientRunning &= sharedRunning.Get();
    }

    m_SharedIsRunning.WaitGet().Get() = false;
    m_Client->Disconnect();
    m_IsClientConnected.WaitGet().Get() = false;
    RELEASE(m_Client);
}

void ClientApp::Send(const std::string& data)
{
    if (!m_IsClientConnected.WaitGet().Get())
    {
        DebugLog("Client isn't connected to server! \n");
        return;
    }

    if (!data.empty())
        m_Client->Send(data);
}

void ClientApp::Update(float delta)
{
    static float timeSinceLastUpdate = 0.0f;
    timeSinceLastUpdate += delta;
    auto lock = m_StateMachine->TryGet();
    if (lock.IsValid())
    {
        lock->Update(timeSinceLastUpdate);
        timeSinceLastUpdate = 0.0f;
    }
    else if (timeSinceLastUpdate > 0.3f)
    {
        auto lock2 = m_StateMachine->WaitGet();
        lock2->Update(timeSinceLastUpdate);
    }
}

void ClientApp::Cleanup()
{
    RELEASE(m_StateMachine);

    for (auto& drawable : m_Window->GetDrawables())
    {
        RELEASE(drawable);
    }

    RELEASE(m_Window);

    if (m_ClientThread)
    {
        m_ClientThread->Start();
        m_ClientThread->Wait();
        RELEASE(m_ClientThread);
    }

    FontRegistry::ClearFonts();
    PlayerShapeRegistry::ClearPlayerShapes();
}

bool ClientApp::TryToConnect(const std::string* ip)
{
    if (m_ClientThread != nullptr)
        return false; // TODO: Handle reconnecting

    DebugLog("Try to connect to " + *ip + "...\n");

    m_ClientThread = Thread::Create([](LPVOID ip) -> DWORD
    {
        std::string* ipStr = static_cast<std::string*>(ip);

        DebugLog("IP in connect thread " + *ipStr + "...\n");

        ClientApp::GetInstance().RunClient(ipStr);

        return 0;
    }, ip, true);

    return true;
    /*
    float timeoutConnection = 0.0f;
    while (!m_IsClientConnected.WaitGet().Get())
    {
        timeoutConnection += TimeManager::GetDeltaTime();

        if (timeoutConnection > CONNECTION_TIMEOUT_TIME)
        {
            DebugLog("Failed to connect to server!\n");
            return false;
        }
    }

    return true;*/
}
