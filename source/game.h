#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <map>

constexpr int SCR_W = 1280;
constexpr int SCR_H = 720;
constexpr int TILE  = 40;

enum TileId { T_FLOOR=0, T_WALL, T_DOOR, T_ELEV, T_SECRET, T_PORTAL,
              T_EXIT, T_ROAD, T_GRASS, T_ICE, T_DESK, T_BED, T_COUNT };

struct Col { Uint8 r,g,b,a; };
extern const Col C_BG, C_INK, C_ORANGE, C_PANEL, C_GRID, C_OK, C_WARN, C_TEXT;

struct Input {
    bool up=0,down=0,left=0,right=0,a=0,b=0,plus=0,minus=0;
    bool aEdge=0,bEdge=0;
    void poll();
};
struct Player { int tx,ty,dir,floor; };
struct NPC {
    std::string id, name, room, line;
    int tx, ty, floor, color;
    int dayMin=0, dayMax=31;
};
struct Choice { std::string text; int next; std::string setFlag; };
struct DialogueNode {
    int id; std::string speaker, text;
    std::vector<Choice> choices;
    int next;
    std::string setFlag, giveTask;
};
struct DayTask { std::string id, desc; bool done=false; };
struct Day { int index; std::string title, summary; std::vector<DayTask> tasks; };
struct Floor {
    std::string id, name; int color, w, h;
    std::vector<int> tiles;
    std::vector<NPC> npcs;
    int up=-1, down=-1;
};
enum State { ST_MENU, ST_SAVES, ST_SETTINGS, ST_INTRO, ST_PLAY, ST_DIALOGUE,
             ST_MINIGAME, ST_PAUSE, ST_DAYEND, ST_TBC, ST_JOURNAL };
struct Game {
    State state = ST_MENU, prev = ST_MENU;
    Input in;
    Player player{0,0,0,0};
    std::vector<Floor> floors;
    std::vector<Day> days;
    int day = 1, slot = 0, sel = 0;
    std::map<std::string,std::string> flags;
    int dlgNode = 0, dlgSel = 0;
    int minigame = 0;
    Uint32 tick = 0;
    bool running = true;
};
extern Game G;

void font_init();
void text(int x,int y,const std::string&s,int scale=2,const Col&c=C_TEXT);
int  textw(const std::string&s,int scale=2);
void rect_fill(int x,int y,int w,int h,const Col&c);
void rect_line(int x,int y,int w,int h,const Col&c);
void blueprint_bg();
void wrap_text(int x,int y,const std::string&s,int maxw,int scale,const Col&c);

void world_load();
void world_draw();
bool world_walkable(int f,int tx,int ty);
NPC* world_npc_at(int f,int tx,int ty);

void story_load();
void story_start_day(int d);
void story_update();
bool story_flag(const std::string&f);
void story_set_flag(const std::string&f,const std::string&v="1");
void dlg_start(const std::vector<DialogueNode>&n,int id=0);
void dlg_update();
void dlg_draw();

void mg_start(int id);
void mg_update();
void mg_draw();

void save_write(int slot);
bool save_read(int slot);
void save_draw_slot(int slot,int x,int y,bool sel);

void sfx(int id);
void music(int id);
