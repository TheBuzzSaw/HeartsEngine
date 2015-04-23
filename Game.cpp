#include "Game.hpp"
#include "Pile.hpp"
#include <chrono>
#include <lua.hpp>
using namespace std;

static const int Passes[4] = { -1, +1, +2, 0 };
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

    int localScore = 0;
    int overallScore = 0;
    Pile<13> hand;
};

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
    lua_pop(state, 1);

    SetData(state, PlayerLuaKey, this);
}

Player::~Player()
{
    lua_close(state);
}

struct Game
{
    Game();
    Game(const Game&) = delete;
    ~Game() = default;

    Game& operator=(const Game&) = delete;

    mt19937 mt;
    bool heartsBroken = false;
    int pass = 3;
    Player players[4];
};

Game::Game() : mt(chrono::system_clock::now().time_since_epoch().count())
{
    for (auto& player : players)
        SetData(player.state, GameLuaKey, this);
}

void PlayGame(const GameParameters& gp)
{
    Game game;

    for (int i = 0; i < 4; ++i)
    {
        auto path = gp.scripts[i];
        if (path && *path)
        {
            auto result = luaL_dofile(game.players[i].state, path);
            if (result) ReportErrors(game.players[i].state);
        }
    }

    Pile<52> deck;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 2; j <= 14; ++j)
            deck.Push(Card(j, i));
    }

    deck.Shuffle(game.mt);

    for (int i = 0; i < 52; ++i)
        game.players[i & 3].hand.Push(deck.Pop()); // Deal like a human!

    for (int i = 0; i < 4; ++i) game.players[i].hand.Sort();

    for (int i = 0; i < 4; ++i)
        cout << "Player " << (i + 1) << ": " << game.players[i].hand << endl;
}
