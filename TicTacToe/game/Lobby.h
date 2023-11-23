#pragma once
#include <string>
#include "../tcp-ip/ISerializable.h"
#include "TicTacToe.h"

struct LobbyData : ISerializable
{
    LobbyData() = default;
    LobbyData(const Json& j);
    LobbyData(const int id, const std::string& playerX, const std::string& playerO);

    Json Serialize() override;

    int ID = -1;
    bool IsFastMode = false;
    std::string PlayerX, PlayerO;
};

struct Lobby
{
    Lobby();
    Lobby(bool isFastMode);
    Lobby(const std::string& playerX, const std::string& playerO);
    Lobby(const int id, const std::string& playerX, const std::string& playerO);
    ~Lobby() = default;

    std::string& GetOpponentName(const std::string&);
    void AddPlayerToLobby(const std::string& name);
    void RemovePlayerFromLobby(const std::string& name);

    unsigned int GetPlayerCount() const { return PlayerCount;}

    bool IsInLobby(const std::string& name) const { return Data.PlayerX == name || Data.PlayerO == name; }
    bool IsLobbyFull() const {  return !Data.PlayerX.empty() && !Data.PlayerO.empty(); }
    bool IsLobbyEmpty() const { return Data.PlayerX.empty() && Data.PlayerO.empty(); }

    unsigned int PlayerCount = 0;
    LobbyData Data;
    TicTacToe::Board Board;
};
