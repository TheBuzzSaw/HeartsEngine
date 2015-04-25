#include "Game.hpp"
#include "Pile.hpp"
#include <chrono>
#include <fstream>
#include <cassert>
#include <lua.hpp>
using namespace std;

static const int Passes[4] = { 3, 1, 2, 0 };
static const char* const PackageName = "hearts";

static void* const PlayerLuaKey = (void*)0xbaba;
static void* const GameLuaKey = (void*)0xecec;

static void SetData(lua_State* state, void* key, void* value)
{
    lua_pushlightuserdata(state, key);
    lua_pushlightuserdata(state, value);
    lua_settable(state, LUA_REGISTRYINDEX);
}

static void* GetData(lua_State* state, void* key)
{
    lua_pushlightuserdata(state, key);
    lua_gettable(state, LUA_REGISTRYINDEX);
    void* result = lua_touserdata(state, -1);
    lua_pop(state, 1);
    return result;
}

static void ReportErrors(lua_State* state)
{
    cerr << "Lua error -- " << lua_tostring(state, -1) << '\n';
    lua_pop(state, 1);
}

static int DebugPrint(lua_State* state)
{
    cerr << "DEBUG PRINT\n";
    return 0;
}

struct Player
{
    Player();
    Player(const Player&) = delete;
    ~Player();

    Player& operator=(const Player&) = delete;

    lua_State* state = nullptr;
    int passFunction = LUA_NOREF;
    int playFunction = LUA_NOREF;
    int viewFunction = LUA_NOREF;

    int id = 0;
    int localScore = 0;
    int overallScore = 0;
    Pile<13> hand;
    Pile<3> pass;
    ofstream log;
};

struct Game
{
    Game(const GameParameters& gp);
    Game(const Game&) = delete;
    ~Game() = default;

    Game& operator=(const Game&) = delete;

    int HighestScore() const;

    mt19937 mt;
    bool heartsBroken = false;
    int pass = 3;
    GameParameters parameters;
    Player players[4];
};

static int LuaSetPassFunction(lua_State* state)
{
    auto argc = lua_gettop(state);

    if (argc > 0 && lua_isfunction(state, 1))
    {
        lua_settop(state, 1);

        Player* player =
            reinterpret_cast<Player*>(GetData(state, PlayerLuaKey));

        if (player->passFunction != LUA_NOREF)
            luaL_unref(state, LUA_REGISTRYINDEX, player->passFunction);

        player->passFunction = luaL_ref(state, LUA_REGISTRYINDEX);
    }

    return 0;
}

static int LuaLog(lua_State* state)
{
    auto argc = lua_gettop(state);

    Player* player = reinterpret_cast<Player*>(GetData(state, PlayerLuaKey));

    for (int i = 0; i < argc; ++i)
    {
        auto value = lua_tostring(state, i + 1);

        if (value && *value)
            player->log << value << '\n';
    }

    return 0;
}

static int LuaIsHeartsBroken(lua_State* state)
{
    Game* game = reinterpret_cast<Game*>(GetData(state, GameLuaKey));
    lua_pushboolean(state, game->heartsBroken);
    return 1;
}

Player::Player()
{
    state = luaL_newstate();
    luaL_openlibs(state);

    lua_newtable(state);
    lua_setglobal(state, PackageName);

    lua_getglobal(state, PackageName);
    lua_pushcfunction(state, DebugPrint);
    lua_setfield(state, -2, "dump");
    lua_pushcfunction(state, DebugPrint);
    lua_setfield(state, -2, "blargh");
    lua_pushcfunction(state, LuaIsHeartsBroken);
    lua_setfield(state, -2, "broken");
    lua_pushcfunction(state, LuaLog);
    lua_setfield(state, -2, "log");
    lua_pushcfunction(state, LuaSetPassFunction);
    lua_setfield(state, -2, "pass");
    lua_pop(state, 1);

    SetData(state, PlayerLuaKey, this);
}

Player::~Player()
{
    lua_close(state);
    log.close();
}

Game::Game(const GameParameters& gp)
    : mt(chrono::system_clock::now().time_since_epoch().count())
    , parameters(gp)
{
    int id = 0;
    char logFile[] = "log.0.txt";

    for (int i = 0; i < 4; ++i)
    {
        auto& player = players[i];

        player.hand.Sort();
        player.id = ++id;
        logFile[4] = '0' + player.id;
        player.log.open(logFile, ofstream::binary);
        SetData(player.state, GameLuaKey, this);

        auto path = gp.scripts[i];
        if (path && *path)
        {
            auto result = luaL_dofile(player.state, path);
            if (result) ReportErrors(player.state);
        }
    }
}

int Game::HighestScore() const
{
    int result = 0;

    for (const auto& player : players)
        result = max(result, player.overallScore);

    return result;
}

void PlayGame(const GameParameters& gp)
{
    Game game(gp);
    int leadPlayer = uniform_int_distribution<int>(0, 3)(game.mt);
    int pass = -1;

    while (game.HighestScore() < game.parameters.scoreLimit)
    {
        pass = (pass + 1) & 3;
        int offset = Passes[pass];

        Pile<52> deck;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 2; j <= 14; ++j)
                deck.Push(Card(j, i));
        }

        deck.Shuffle(game.mt);

        for (int i = 0; i < 52; ++i)
            game.players[i & 3].hand.Push(deck.Pop()); // Deal like a human!

        for (int i = 0; i < 4; ++i)
            cout << "Player " << (i + 1) << ": " <<
                game.players[i].hand << endl;

        if (offset != 0)
        {
            for (auto& player : game.players)
            {
                player.pass.Clear();
                int cardIndices[3] = {};

                if (player.passFunction != LUA_NOREF)
                {
                    lua_rawgeti(
                        player.state,
                        LUA_REGISTRYINDEX,
                        player.passFunction);

                    auto status = lua_pcall(player.state, 0, 3, 0);
                    if (status)
                    {
                        ReportErrors(player.state);
                    }
                    else
                    {
                        for (int i = 0; i < 3; ++i)
                        {
                            cardIndices[i] = lua_tointeger(player.state, i + 1);
                            auto type = lua_type(player.state, i + 1);
                            cout << lua_typename(player.state, type) << endl;
                        }

                        lua_settop(player.state, 0);
                    }
                }
                else
                {
                    cerr << "No pass function...\n";
                }

                cout << "Card Indices:";

                for (int i = 0; i < 3; ++i)
                    cout << ' ' << cardIndices[i];

                cout << endl;

                sort(cardIndices, cardIndices + 3);

                bool selectionIsValid = true;

                for (int i = 0; i < 3; ++i)
                {
                    auto cardIndex = cardIndices[i];

                    if (cardIndex < 1 || cardIndex > 13)
                    {
                        selectionIsValid = false;
                        cerr << "invalid card index -- " << cardIndex << '\n';
                    }
                }

                if (cardIndices[0] >= cardIndices[1] ||
                    cardIndices[1] >= cardIndices[2])
                {
                    selectionIsValid = false;
                    cerr << "failed to select three distinct cards\n";
                }

                if (!selectionIsValid)
                {
                    cardIndices[0] = 11;
                    cardIndices[1] = 12;
                    cardIndices[2] = 13;
                }

                for (int i = 0; i < 3; ++i)
                    player.pass.Push(
                        player.hand.Remove(
                            cardIndices[2 - i] - 1));
            }

            for (int i = 0; i < 4; ++i)
            {
                auto& giver = game.players[i];
                auto& recipient = game.players[(i + offset) & 3];

                assert(giver.pass.IsFull());
                assert(!recipient.hand.IsFull());

                for (int j = 0; j < 3; ++j)
                    recipient.hand.Push(giver.pass.Pop());

                recipient.hand.Sort();
            }
        }

        break;

        for (int i = 0; i < 13; ++i)
        {
            Pile<4> play;

            for (int j = 0; j < 4; ++j)
            {
                auto& player = game.players[(leadPlayer + j) & 3];
                // Player plays card.
            }
        }
    }
}
