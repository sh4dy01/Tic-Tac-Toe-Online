#include "GameData.h"

GameData::GameData(const std::vector<PlayerMove>& allMoves)
{
    for (auto& move : allMoves)
    {
        AllMoves.emplace_back(move);
    }
}

GameData::GameData(const Json& j)
{
    for (auto& move : j["AllMoves"])
    {
        AllMoves.emplace_back(PlayerMove(move));
    }
}

Json GameData::Serialize()
{
    Json j;

    for (auto& move : AllMoves)
    {
        j["AllMoves"].push_back(move.Serialize());
    }
    return j;
}

Json PlayerMove::Serialize()
{
    Json j;
    j["PlayerName"] = PlayerName;
    j["PlayerPiece"] = PlayerPiece;
    j["BoardCell"] = BoardCell;

    return j;
}
