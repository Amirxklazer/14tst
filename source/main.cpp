#include "game.h"
#include <cstdio>
#include <cstring>

Game G;
SDL_Window*  win = nullptr;
SDL_Renderer* ren = nullptr;

const Col C_BG    {10,20,40,255};
const Col C_INK   {230,240,255,255};
const Col C_ORANGE{255,120,20,255};
const Col C_PANEL {18,32,60,235};
const Col C_GRID  {40,70,120,255};
const Col C_OK    {80,220,120,255};
const Col C_WARN  {240,80,80,255};
const Col C_TEXT  {220,230,245,255};

static const unsigned char FONT5x7[][5] = {
 {0,0,0,0,0},{0x00,0x00,0x5F,0x00,0x00},{0x00,0x07,0x00,0x07,0x00},{0x14,0x7F,0x14,0x7F,0x14},
 {0x24,0x2A,0x7F,0x2A,0x12},{0x23,0x13,0x08,0x64,0x62},{0x36,0x49,0x55,0x22,0x50},{0x00,0x05,0x03,0x00,0x00},
 {0x00,0x1C,0x22,0x41,0x00},{0x00,0x41,0x22,0x1C,0x00},{0x14,0x08,0x3E,0x08,0x14},{0x08,0x08,0x3E,0x08,0x08},
 {0x00,0x50,0x30,0x00,0x00},{0x08,0x08,0x08,0x08,0x08},{0x00,0x60,0x60,0x00,0x00},{0x20,0x10,0x08,0x04,0x02},
 {0x3E,0x51,0x49,0x45,0x3E},{0x00,0x42,0x7F,0x40,0x00},{0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4B,0x31},
 {0x18,0x14,0x12,0x7F,0x10},{0x27,0x45,0x45,0x45,0x39},{0x3C,0x4A,0x49,0x49,0x30},{0x01,0x71,0x09,0x05,0x03},
 {0x36,0x49,0x49,0x49,0x36},{0x06,0x49,0x49,0x29,0x1E},{0x00,0x36,0x36,0x00,0x00},{0x00,0x56,0x36,0x00,0x00},
 {0x08,0x14,0x22,0x41,0x00},{0x14,0x14,0x14,0x14,0x14},{0x00,0x41,0x22,0x14,0x08},{0x02,0x01,0x51,0x09,0x06},
 {0x32,0x49,0x79,0x41,0x3E},{0x7E,0x11,0x11,0x11,0x7E},{0x7F,0x49,0x49,0x49,0x36},{0x3E,0x41,0x41,0x41,0x22},
 {0x7F,0x41,0x41,0x22,0x1C},{0x7F,0x49,0x49,0x49,0x41},{0x7F,0x09,0x09,0x09,0x01},{0x3E,0x41,0x49,0x49,0x7A},
 {0x7F,0x08,0x08,0x08,0x7F},{0x00,0x41,0x7F,0x41,0x00},{0x20,0x40,0x41,0x3F,0x01},{0x7F,0x08,0x14,0x22,0x41},
 {0x7F,0x40,0x40,0x40,0x40},{0x7F,0x02,0x0C,0x02,0x7F},{0x7F,0x04,0x08,0x10,0x7F},{0x3E,0x41,0x41,0x41,0x3E},
 {0x7F,0x09,0x09,0x09,0x06},{0x3E,0x41,0x51,0x21,0x5E},{0x7F,0x09,0x19,0x29,0x46},{0x46,0x49,0x49,0x49,0x31},
 {0x01,0x01,0x7F,0x01,0x01},{0x3F,0x40,0x40,0x40,0x3F},{0x1F,0x20,0x40,0x20,0x1F},{0x3F,0x40,0x38,0x40,0x3F},
 {0x63,0x14,0x08,0x14,0x63},{0x07,0x08,0x70,0x08,0x07},{0x61,0x51,0x49,0x45,0x43},{0x00,0x7F,0x41,0x41,0x00},
 {0x02,0x04,0x08,0x10,0x20},{0x00,0x41,0x41,0x7F,0x00},{0x04,0x02,0x01,0x02,0x04},{0x40,0x40,0x40,0x40,0x40},
 {0x00,0x01,0x02,0x04,0x00},
};
static int glyph_index(char c){
    if(c==' ') return 0;
    if(c>='!'&&c<='`') return c-'!'+1;
    if(c>='a'&&c<='z') return (c-'a')+'A'-'!'+1;
    return 0;
}
void font_init(){}
void text(int x,int y,const std::string&s,int scale,const Col&c){
    SDL_SetRenderDrawColor(ren,c.r,c.g,c.b,c.a);
    int cx=x;
    for(char ch:s){
        const unsigned char* g=FONT5x7[glyph_index(ch)];
        for(int col=0;col<5;col++) for(int row=0;row<7;row++)
            if(g[col]&(1<<row)){ SDL_Rect r{cx+col*scale, y+row*scale, scale, scale}; SDL_RenderFillRect(ren,&r); }
        cx+=6*scale;
    }
}
int textw(const std::string&s,int scale){ return (int)s.size()*6*scale; }
void rect_fill(int x,int y,int w,int h,const Col&c){
    SDL_SetRenderDrawColor(ren,c.r,c.g,c.b,c.a);
    SDL_Rect r{x,y,w,h}; SDL_RenderFillRect(ren,&r);
}
void rect_line(int x,int y,int w,int h,const Col&c){
    SDL_SetRenderDrawColor(ren,c.r,c.g,c.b,c.a);
    SDL_Rect r{x,y,w,h}; SDL_RenderDrawRect(ren,&r);
}
void blueprint_bg(){
    rect_fill(0,0,SCR_W,SCR_H,C_BG);
    SDL_SetRenderDrawColor(ren,C_GRID.r,C_GRID.g,C_GRID.b,120);
    for(int x=0;x<SCR_W;x+=40) SDL_RenderDrawLine(ren,x,0,x,SCR_H);
    for(int y=0;y<SCR_H;y+=40) SDL_RenderDrawLine(ren,0,y,SCR_W,y);
}
void wrap_text(int x,int y,const std::string&s,int maxw,int scale,const Col&c){
    std::string line, word; int cy=y;
    auto flush=[&](){ if(!line.empty()){ text(x,cy,line,scale,c); cy+=9*scale; line.clear(); } };
    for(size_t i=0;i<=s.size();++i){
        char ch = i<s.size()? s[i]:' ';
        if(ch==' '){
            if(textw(line+word,scale)>maxw){ flush(); line=word; }
            else { if(!line.empty()) line+=' '; line+=word; }
            word.clear();
        } else word+=ch;
    }
    flush();
}

void Input::poll(){
    SDL_Event e; aEdge=bEdge=false;
    while(SDL_PollEvent(&e)){
        if(e.type==SDL_QUIT) G.running=false;
        if(e.type==SDL_KEYDOWN) switch(e.key.keysym.sym){
            case SDLK_UP: case SDLK_w: up=true; break;
            case SDLK_DOWN: case SDLK_s: down=true; break;
            case SDLK_LEFT: case SDLK_a: left=true; break;
            case SDLK_RIGHT: case SDLK_d: right=true; break;
            case SDLK_RETURN: case SDLK_SPACE: a=true; aEdge=true; break;
            case SDLK_ESCAPE: case SDLK_BACKSPACE: b=true; bEdge=true; break;
            case SDLK_PLUS: case SDLK_EQUALS: plus=true; break;
            case SDLK_MINUS: minus=true; break;
        }
        if(e.type==SDL_KEYUP) switch(e.key.keysym.sym){
            case SDLK_UP: case SDLK_w: up=false; break;
            case SDLK_DOWN: case SDLK_s: down=false; break;
            case SDLK_LEFT: case SDLK_a: left=false; break;
            case SDLK_RIGHT: case SDLK_d: right=false; break;
            case SDLK_RETURN: case SDLK_SPACE: a=false; break;
            case SDLK_ESCAPE: case SDLK_BACKSPACE: b=false; break;
            case SDLK_PLUS: case SDLK_EQUALS: plus=false; break;
            case SDLK_MINUS: minus=false; break;
        }
    }
}
void sfx(int){} void music(int){}

static std::string slotpath(int s){
    char b[128]; snprintf(b,sizeof(b),"sdmc:/switch/Ilot14/save%d.dat",s); return b;
}
void save_write(int slot){
    FILE* f=fopen(slotpath(slot).c_str(),"wb"); if(!f) return;
    fwrite(&G.day,sizeof(int),1,f);
    int n=(int)G.flags.size(); fwrite(&n,sizeof(int),1,f);
    for(auto&kv:G.flags){
        int l=(int)kv.first.size();  fwrite(&l,sizeof(int),1,f); fwrite(kv.first.data(),1,l,f);
        int m=(int)kv.second.size(); fwrite(&m,sizeof(int),1,f); fwrite(kv.second.data(),1,m,f);
    }
    fclose(f);
}
bool save_read(int slot){
    FILE* f=fopen(slotpath(slot).c_str(),"rb"); if(!f) return false;
    fread(&G.day,sizeof(int),1,f);
    int n=0; fread(&n,sizeof(int),1,f); G.flags.clear();
    for(int i=0;i<n;i++){
        int l,m; std::string k,v;
        fread(&l,sizeof(int),1,f); k.resize(l); if(l) fread(&k[0],1,l,f);
        fread(&m,sizeof(int),1,f); v.resize(m); if(m) fread(&v[0],1,m,f);
        G.flags[k]=v;
    }
    fclose(f); return true;
}
void save_draw_slot(int slot,int x,int y,bool sel){
    Col c = sel? C_ORANGE : C_PANEL;
    rect_fill(x,y,520,90,c); rect_line(x,y,520,90,C_GRID);
    char b[64]; snprintf(b,sizeof(b),"SLOT %d",slot+1); text(x+18,y+14,b,3,C_INK);
    FILE* f=fopen(slotpath(slot).c_str(),"rb");
    if(f){ int d; fread(&d,sizeof(int),1,f); fclose(f);
        char sb[64]; snprintf(sb,sizeof(sb),"DAY %d",d); text(x+18,y+50,sb,2,C_INK);
    } else text(x+18,y+50,"EMPTY",2,C_INK);
}

static const char* MENU_ITEMS[]={"NEW GAME","CONTINUE","SAVE SLOTS","SETTINGS","EXIT"};
static const int MENU_N=5;

static void draw_menu(){
    blueprint_bg();
    rect_fill(80,80,180,180,C_ORANGE); text(120,140,"14",8,C_BG);
    text(80,290,"ILOT14",5,C_INK); text(84,335,"A WORLD GAME",2,C_TEXT);
    int bx=760, by=110;
    rect_fill(bx,by,440,420,C_PANEL); rect_line(bx,by,440,420,C_ORANGE);
    for(int i=0;i<MENU_N;i++){
        bool s=(G.sel==i);
        if(s) rect_fill(bx+20,by+40+i*70,400,54,C_ORANGE);
        text(bx+40,by+56+i*70,MENU_ITEMS[i],3,s?C_BG:C_INK);
    }
    text(80,SCR_H-40,"A=OK  B=BACK  +/-=MOVE",2,C_TEXT);
}
static void draw_saves(){
    blueprint_bg();
    text(80,60,"SAVE SLOTS",4,C_ORANGE);
    for(int i=0;i<3;i++) save_draw_slot(i,80,160+i*120,G.sel==i);
    text(80,SCR_H-40,"A=SELECT  B=BACK",2,C_TEXT);
}
static void draw_settings(){
    blueprint_bg();
    text(80,60,"SETTINGS",4,C_ORANGE);
    text(80,160,"BRIGHTNESS   [====----]",2,C_INK);
    text(80,210,"AUDIO        [======--]",2,C_INK);
    text(80,260,"TEXT SPEED   [=====---]",2,C_INK);
    text(80,SCR_H-40,"B=BACK",2,C_TEXT);
}
static void draw_pause(){
    rect_fill(0,0,SCR_W,SCR_H,Col{0,0,0,180});
    rect_fill(SCR_W/2-260,SCR_H/2-180,520,360,C_PANEL);
    rect_line(SCR_W/2-260,SCR_H/2-180,520,360,C_ORANGE);
    text(SCR_W/2-140,SCR_H/2-130,"PAUSED",5,C_ORANGE);
    const char* items[]={"RESUME","JOURNAL","SAVE","QUIT TO MENU"};
    for(int i=0;i<4;i++){
        bool s=(G.sel==i);
        if(s) rect_fill(SCR_W/2-220,SCR_H/2-40+i*60,440,46,C_ORANGE);
        text(SCR_W/2-200,SCR_H/2-26+i*60,items[i],3,s?C_BG:C_INK);
    }
}
static void draw_journal(){
    blueprint_bg();
    char b[64]; snprintf(b,sizeof(b),"DAY %d / 31",G.day);
    text(80,60,b,4,C_ORANGE);
    const Day& d = G.days[G.day-1];
    text(80,130,d.title.c_str(),3,C_INK);
    wrap_text(80,180,d.summary,1100,2,C_TEXT);
    int y=300;
    for(auto&t:d.tasks){
        text(80,y,(t.done?"[X] ":"[ ] ")+t.desc,2,t.done?C_OK:C_TEXT);
        y+=36;
    }
    text(80,SCR_H-40,"B=BACK",2,C_TEXT);
}

static bool init(){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER)!=0) return false;
    win=SDL_CreateWindow("Ilot14",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCR_W,SCR_H,SDL_WINDOW_SHOWN);
    if(!win) return false;
    ren=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!ren) return false;
    font_init(); return true;
}

int main(int,char**){
    if(!init()) return 1;
    world_load(); story_load();
    G.state=ST_MENU; G.sel=0;
    while(G.running){
        G.in.poll();
        G.tick=SDL_GetTicks();
        switch(G.state){
        case ST_MENU:
            if(G.in.aEdge){
                if(G.sel==0){ G.day=1; G.flags.clear(); story_start_day(1); G.player={0,0,0,0}; G.state=ST_INTRO; }
                else if(G.sel==1){ if(save_read(G.slot)){ story_start_day(G.day); G.state=ST_PLAY; } }
                else if(G.sel==2){ G.state=ST_SAVES; G.sel=0; }
                else if(G.sel==3){ G.state=ST_SETTINGS; }
                else if(G.sel==4){ G.running=false; }
            }
            if(G.in.up)   { G.sel=(G.sel+MENU_N-1)%MENU_N; SDL_Delay(120); }
            if(G.in.down) { G.sel=(G.sel+1)%MENU_N; SDL_Delay(120); }
            draw_menu(); break;

        case ST_SAVES:
            if(G.in.bEdge){ G.state=ST_MENU; G.sel=0; }
            if(G.in.up)   { G.sel=(G.sel+2)%3; SDL_Delay(120); }
            if(G.in.down) { G.sel=(G.sel+1)%3; SDL_Delay(120); }
            if(G.in.aEdge){ G.slot=G.sel;
                if(save_read(G.slot)){ story_start_day(G.day); G.state=ST_PLAY; }
                else { G.day=1; G.flags.clear(); story_start_day(1); G.state=ST_INTRO; }
            }
            draw_saves(); break;

        case ST_SETTINGS:
            if(G.in.bEdge) G.state=ST_MENU;
            draw_settings(); break;

        case ST_INTRO:
            blueprint_bg();
            text(80,80,"ILOT14",7,C_ORANGE);
            wrap_text(80,240,"WELCOME TO ILOT14. EIGHT FLOORS UP. EIGHT DOWN. DALIA NEEDS HELP. FIND THE STAIRWAY TO HELL. PRESS A TO BEGIN.",1100,2,C_INK);
            if(G.in.aEdge) G.state=ST_PLAY;
            break;

        case ST_PLAY: {
            int nx=G.player.tx, ny=G.player.ty;
            if(G.in.up) ny--; if(G.in.down) ny++;
            if(G.in.left) nx--; if(G.in.right) nx++;
            if((nx!=G.player.tx||ny!=G.player.ty) && world_walkable(G.player.floor,nx,ny)){
                G.player.tx=nx; G.player.ty=ny;
            }
            if(G.in.bEdge){ G.prev=ST_PLAY; G.state=ST_PAUSE; G.sel=0; }
            if(G.in.plus){ G.prev=ST_PLAY; G.state=ST_JOURNAL; }
            if(G.in.aEdge) story_update();
            world_draw();
            const Day& d=G.days[G.day-1];
            char b[64]; snprintf(b,sizeof(b),"DAY %d",G.day);
            rect_fill(0,0,220,44,C_PANEL); text(14,12,b,3,C_ORANGE);
            text(14,SCR_H-30,d.title.c_str(),2,C_TEXT);
            break;
        }
        case ST_DIALOGUE: dlg_update(); world_draw(); dlg_draw(); break;
        case ST_MINIGAME: mg_update(); mg_draw(); break;

        case ST_PAUSE:
            if(G.in.bEdge) G.state=ST_PLAY;
            if(G.in.up)   { G.sel=(G.sel+3)%4; SDL_Delay(120); }
            if(G.in.down) { G.sel=(G.sel+1)%4; SDL_Delay(120); }
            if(G.in.aEdge){
                if(G.sel==0) G.state=ST_PLAY;
                else if(G.sel==1){ G.prev=ST_PAUSE; G.state=ST_JOURNAL; }
                else if(G.sel==2){ save_write(G.slot); G.state=ST_PLAY; }
                else if(G.sel==3){ G.state=ST_MENU; G.sel=0; }
            }
            world_draw(); draw_pause(); break;

        case ST_JOURNAL:
            if(G.in.bEdge) G.state=(G.prev==ST_PAUSE)?ST_PAUSE:ST_PLAY;
            draw_journal(); break;

        case ST_DAYEND: {
            blueprint_bg();
            char b[64]; snprintf(b,sizeof(b),"DAY %d COMPLETE",G.day);
            text(SCR_W/2-textw(b,5)/2,240,b,5,C_ORANGE);
            text(SCR_W/2-textw("TO BE CONTINUED",3)/2,360,"TO BE CONTINUED",3,C_INK);
            if(G.in.aEdge){
                if(G.day<31){ G.day++; story_start_day(G.day); G.state=ST_PLAY; }
                else G.state=ST_MENU;
            }
            break;
        }
        case ST_TBC:
            blueprint_bg();
            text(SCR_W/2-200,300,"TO BE CONTINUED",5,C_ORANGE);
            if(G.in.aEdge) G.state=ST_DAYEND;
            break;
        }
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
    return 0;
}
