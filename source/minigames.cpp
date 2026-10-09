#include "game.h"
#include <cstdio>
#include <cstdlib>

static int mg_id=0;
static int bots_caught=0;
static int bots_total=5;
static Uint32 mg_start_ms=0;
static int mg_time_limit=45000;
static int px=SCR_W/2, py=SCR_H/2;
static int hornet_hp=3;
static int br_x=1, br_y=1;
static const int BR_W=15, BR_H=9;
static int br_maze[BR_H][BR_W];
static int craft_step=0;

static void seed_backrooms(){
    srand(14);
    for(int y=0;y<BR_H;y++) for(int x=0;x<BR_W;x++)
        br_maze[y][x]=(x==0||y==0||x==BR_W-1||y==BR_H-1)?1:(rand()%100<28);
    br_maze[1][1]=0; br_maze[BR_H-2][BR_W-2]=0;
}
void mg_start(int id){
    mg_id=id; mg_start_ms=SDL_GetTicks(); G.state=ST_MINIGAME;
    if(id==1) bots_caught=0;
    else if(id==2){ hornet_hp=3; px=SCR_W/2; py=SCR_H-200; }
    else if(id==3) craft_step=0;
    else if(id==4){ br_x=1; br_y=1; seed_backrooms(); }
}
static void mg_bots(){
    if(G.in.left) px-=6; if(G.in.right) px+=6;
    if(G.in.up) py-=6;   if(G.in.down) py+=6;
    if(px<40) px=40; if(px>SCR_W-40) px=SCR_W-40;
    if(py<80) py=80; if(py>SCR_H-40) py=SCR_H-40;
    if(G.in.aEdge && bots_caught<bots_total){ bots_caught++; SDL_Delay(80); }
    Uint32 el=SDL_GetTicks()-mg_start_ms;
    if(bots_caught>=bots_total || el>(Uint32)mg_time_limit){
        G.state=ST_PLAY; story_set_flag("mg_bots_done");
    }
}
static void mg_hornets(){
    if(G.in.left) px-=6; if(G.in.right) px+=6;
    if(G.in.up) py-=6;   if(G.in.down) py+=6;
    if(px<40) px=40; if(px>SCR_W-40) px=SCR_W-40;
    if(py<80) py=80; if(py>SCR_H-40) py=SCR_H-40;
    if(G.in.aEdge){ hornet_hp--; SDL_Delay(150); }
    if(hornet_hp<=0){ G.state=ST_PLAY; story_set_flag("mg_hornets_done"); }
}
static void mg_craft(){
    static Uint32 last=0; Uint32 now=SDL_GetTicks();
    if(now-last>600){ craft_step=(craft_step+1)%100; last=now; }
    if(G.in.aEdge && craft_step>=45 && craft_step<=55){
        story_set_flag("mg_craft_done"); G.state=ST_PLAY;
    }
}
static void mg_backrooms(){
    if(G.in.left  && br_x>0     && br_maze[br_y][br_x-1]==0) br_x--;
    if(G.in.right && br_x<BR_W-1 && br_maze[br_y][br_x+1]==0) br_x++;
    if(G.in.up    && br_y>0     && br_maze[br_y-1][br_x]==0) br_y--;
    if(G.in.down  && br_y<BR_H-1 && br_maze[br_y+1][br_x]==0) br_y++;
    if(br_x==BR_W-2 && br_y==BR_H-2){ story_set_flag("mg_backrooms_done"); G.state=ST_PLAY; }
}
void mg_update(){
    switch(mg_id){
        case 1: mg_bots(); break;
        case 2: mg_hornets(); break;
        case 3: mg_craft(); break;
        case 4: mg_backrooms(); break;
    }
}
void mg_draw(){
    rect_fill(0,0,SCR_W,SCR_H,C_BG);
    switch(mg_id){
    case 1: {
        text(40,30,"BOT ROUNDUP",4,C_ORANGE);
        char b[64]; snprintf(b,sizeof(b),"CAUGHT %d/%d",bots_caught,bots_total);
        text(40,90,b,3,C_INK);
        for(int i=0;i<bots_total;i++){
            Col c = (i<bots_caught)? C_OK : C_PANEL;
            rect_fill(200+i*90,SCR_H/2,60,60,c);
        }
        rect_fill(px-20,py-20,40,40,C_ORANGE); break;
    }
    case 2: {
        text(40,30,"HORNET NEST",4,C_ORANGE);
        char b[64]; snprintf(b,sizeof(b),"HP %d",hornet_hp); text(40,90,b,3,C_INK);
        rect_fill(px-20,py-20,40,40,C_ORANGE);
        rect_fill(SCR_W/2-60,180,120,120,C_WARN); break;
    }
    case 3: {
        text(40,30,"CRAFT DAY",4,C_ORANGE);
        text(40,90,"PRESS A IN THE GREEN ZONE",2,C_INK);
        rect_fill(200,SCR_H/2,880,60,C_PANEL);
        rect_fill(200+(int)(craft_step/100.0f*880), SCR_H/2, 10, 60, C_ORANGE);
        rect_fill(200+(int)(0.45f*880), SCR_H/2, (int)(0.10f*880), 60, C_OK); break;
    }
    case 4: {
        text(40,30,"BACKROOMS",4,C_ORANGE);
        int ox=60, oy=110, s=52;
        for(int y=0;y<BR_H;y++) for(int x=0;x<BR_W;x++){
            Col c = br_maze[y][x]? Col{20,20,30,255} : Col{140,120,60,255};
            rect_fill(ox+x*s,oy+y*s,s-2,s-2,c);
        }
        rect_fill(ox+br_x*s+6,oy+br_y*s+6,s-14,s-14,C_ORANGE); break;
    }
    }
}
