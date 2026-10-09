#include "game.h"
#include <cstdio>

static Floor make_room_floor(const std::string& id,const std::string& name,int color,
                             int w,int h,const std::vector<std::string>& rows,
                             int up,int down){
    Floor f; f.id=id; f.name=name; f.color=color; f.w=w; f.h=h; f.up=up; f.down=down;
    f.tiles.resize(w*h,T_FLOOR);
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){
        char c = (y<(int)rows.size() && x<(int)rows[y].size())? rows[y][x] : '#';
        int t=T_FLOOR;
        switch(c){
            case '#': t=T_WALL; break;
            case '.': t=T_FLOOR; break;
            case 'D': t=T_DOOR; break;
            case 'E': t=T_ELEV; break;
            case 'S': t=T_SECRET; break;
            case 'P': t=T_PORTAL; break;
            case 'X': t=T_EXIT; break;
            case 'R': t=T_ROAD; break;
            case 'G': t=T_GRASS; break;
            case 'I': t=T_ICE; break;
            case 'K': t=T_DESK; break;
            case 'B': t=T_BED; break;
        }
        f.tiles[y*w+x]=t;
    }
    return f;
}

void world_load(){
    G.floors.clear();
    { std::vector<std::string> rows={
        "####################","#..................#","#..K....K....K.....#",
        "#..................#","#........E.........#","#..................#",
        "#.....S............#","#..................#","#..................X","####################"};
      Floor f=make_room_floor("L","LOBBY",0x3a7bd5,20,10,rows,1,-1);
      f.npcs.push_back({"amir","AMIR","L-001","AMIR L-001",6,3,0,0x00a0ff});
      G.floors.push_back(f); }
    for(int i=1;i<=4;i++){
        std::vector<std::string> rows={
          "####################","#..................#","#..B....B....B.....#",
          "#..................#","#........E.........#","#..................#",
          "#..................#","####################"};
        char id[8]; snprintf(id,sizeof(id),"M%d",i);
        char nm[16]; snprintf(nm,sizeof(nm),"MAIN %d",i);
        Floor f=make_room_floor(id,nm,0x8a2be2,20,8,rows,i-1,i+1);
        if(i==1){
            f.npcs.push_back({"dalia","DALIA","M-101","DALIA M-101",4,3,1,0xff6fae});
            f.npcs.push_back({"arwa","ARWA","M-102","ARWA M-102",12,3,1,0xf2c14e});
        }
        G.floors.push_back(f);
    }
    const char* cnames[][2]={{"C-001","NINO"},{"C-002","T-002"},{"C-007","LEA"},{"C-014","SAMI"}};
    for(int i=0;i<4;i++){
        std::vector<std::string> rows={
          "####################","#K.K.K.K.K.K.K.K.K.#","#..................#",
          "#..................#","#........E.........#","#..................#",
          "####################"};
        char id[8]; snprintf(id,sizeof(id),"C%d",i+1);
        char nm[16]; snprintf(nm,sizeof(nm),"COMMON %d",i+1);
        Floor f=make_room_floor(id,nm,0x2ecc71,20,7,rows,5+i,5+i);
        f.npcs.push_back({cnames[i][0],cnames[i][0],cnames[i][0],"",4,3,(int)G.floors.size(),0x2ecc71});
        G.floors.push_back(f);
    }
    { std::vector<std::string> rows={
        "####################","#..................#","#....R....R....R...#",
        "#..................#","#........E.........#","#..................#",
        "#..................#","####################"};
      G.floors.push_back(make_room_floor("B1","PARKING",0x555555,20,8,rows,0,-1)); }
    { std::vector<std::string> rows={
        "####################","#.#.#.#.#.#.#.#.#..#","#.#.#.#.#.#.#.#.#..#",
        "#..................#","#.#.#.#.#.#.#.#.#..#","#.#.#.#.#.#.#.#.#..#",
        "#........P.........#","####################"};
      G.floors.push_back(make_room_floor("B2","BACKROOMS",0x9b59b6,20,8,rows,0,-1)); }
    { std::vector<std::string> rows={
        "GGGGGGGGGGGGGGGGGGGG","G..................G","G....R....R....R...G",
        "G..................G","G........P.........G","G..................G",
        "GRRRRRRRRRRRRRRRRRRG","GGGGGGGGGGGGGGGGGGGG"};
      Floor f=make_room_floor("OUT","ILOT22",0x27ae60,20,8,rows,0,-1);
      f.npcs.push_back({"gabriel","GABRIEL","PARK","GABRIEL",6,3,(int)G.floors.size(),0xe67e22});
      G.floors.push_back(f); }
    { std::vector<std::string> rows={
        "RRRRRRRRRRRRRRRRRRRR","R..................R","R...I...I...I......R",
        "R..................R","R........P.........R","R..................R",
        "RRRRRRRRRRRRRRRRRRRR"};
      G.floors.push_back(make_room_floor("ROAD","MAIN ROAD",0x34495e,20,7,rows,0,-1)); }
    G.player.floor=0; G.player.tx=9; G.player.ty=4;
}

bool world_walkable(int f,int tx,int ty){
    if(f<0||f>=(int)G.floors.size()) return false;
    const Floor& fl=G.floors[f];
    if(tx<0||ty<0||tx>=fl.w||ty>=fl.h) return false;
    int t=fl.tiles[ty*fl.w+tx];
    return !(t==T_WALL||t==T_DESK);
}
NPC* world_npc_at(int f,int tx,int ty){
    if(f<0||f>=(int)G.floors.size()) return nullptr;
    for(auto&n:G.floors[f].npcs) if(n.tx==tx&&n.ty==ty) return &n;
    return nullptr;
}
static Col tile_color(int t,int base){
    Col c;
    switch(t){
        case T_WALL:   c={30,40,70,255}; break;
        case T_FLOOR:  c={(Uint8)((base>>16)&0xff),(Uint8)((base>>8)&0xff),(Uint8)(base&0xff),255}; break;
        case T_DOOR:   c={200,160,90,255}; break;
        case T_ELEV:   c={255,140,0,255}; break;
        case T_SECRET: c={150,60,200,255}; break;
        case T_PORTAL: c={80,220,255,255}; break;
        case T_EXIT:   c={80,220,120,255}; break;
        case T_ROAD:   c={60,70,90,255}; break;
        case T_GRASS:  c={60,180,90,255}; break;
        case T_ICE:    c={240,200,220,255}; break;
        case T_DESK:   c={120,80,40,255}; break;
        case T_BED:    c={200,120,160,255}; break;
        default:       c={40,40,60,255}; break;
    }
    return c;
}
void world_draw(){
    rect_fill(0,0,SCR_W,SCR_H,C_BG);
    if(G.player.floor<0||G.player.floor>=(int)G.floors.size()) return;
    const Floor& f=G.floors[G.player.floor];
    int camx = G.player.tx*TILE - SCR_W/2;
    int camy = G.player.ty*TILE - SCR_H/2;
    if(camx<0) camx=0; if(camy<0) camy=0;
    int maxx=f.w*TILE-SCR_W, maxy=f.h*TILE-SCR_H;
    if(camx>maxx) camx=maxx; if(camy>maxy) camy=maxy;
    if(camx<0) camx=0; if(camy<0) camy=0;
    for(int y=0;y<f.h;y++) for(int x=0;x<f.w;x++){
        Col c=tile_color(f.tiles[y*f.w+x],f.color);
        rect_fill(x*TILE-camx, y*TILE-camy, TILE-1, TILE-1, c);
    }
    for(auto&n:f.npcs){
        if(G.day<n.dayMin||G.day>n.dayMax) continue;
        Col c{(Uint8)((n.color>>16)&0xff),(Uint8)((n.color>>8)&0xff),(Uint8)(n.color&0xff),255};
        rect_fill(n.tx*TILE-camx+6, n.ty*TILE-camy+6, TILE-12, TILE-12, c);
        text(n.tx*TILE-camx-4, n.ty*TILE-camy-18, n.name, 1, C_INK);
    }
    rect_fill(G.player.tx*TILE-camx+8, G.player.ty*TILE-camy+8, TILE-16, TILE-16, C_ORANGE);
    std::string badge = f.id + " - " + f.name;
    rect_fill(SCR_W-360,0,360,44,C_PANEL);
    text(SCR_W-350,12,badge.c_str(),2,C_ORANGE);
}
