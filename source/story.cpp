#include "game.h"
#include <cstdio>

static std::vector<DialogueNode> DLG;

void dlg_start(const std::vector<DialogueNode>&n,int id){
    DLG=n; G.dlgNode=id; G.dlgSel=0; G.state=ST_DIALOGUE;
}
static const DialogueNode* node_by_id(int id){
    for(auto&n:DLG) if(n.id==id) return &n;
    return nullptr;
}
void dlg_update(){
    const DialogueNode* n = node_by_id(G.dlgNode);
    if(!n){ G.state=ST_PLAY; return; }
    if(!n->choices.empty()){
        if(G.in.up){ G.dlgSel=(G.dlgSel+(int)n->choices.size()-1)%(int)n->choices.size(); SDL_Delay(120); }
        if(G.in.down){ G.dlgSel=(G.dlgSel+1)%(int)n->choices.size(); SDL_Delay(120); }
        if(G.in.aEdge){
            const Choice& c=n->choices[G.dlgSel];
            if(!c.setFlag.empty()) story_set_flag(c.setFlag);
            if(c.next<0) G.state=ST_PLAY; else G.dlgNode=c.next;
        }
    } else {
        if(G.in.aEdge){
            if(!n->setFlag.empty()) story_set_flag(n->setFlag);
            if(n->next<0) G.state=ST_PLAY; else G.dlgNode=n->next;
        }
    }
}
void dlg_draw(){
    const DialogueNode* n = node_by_id(G.dlgNode);
    if(!n) return;
    int bx=80, by=SCR_H-260, bw=SCR_W-160, bh=220;
    rect_fill(bx,by,bw,bh,C_PANEL); rect_line(bx,by,bw,bh,C_ORANGE);
    text(bx+20,by+16,n->speaker.c_str(),3,C_ORANGE);
    wrap_text(bx+20,by+56,n->text,1100,2,C_INK);
    for(size_t i=0;i<n->choices.size();i++){
        int y=by+bh-(int)(n->choices.size()-i)*38-16;
        bool s=(G.dlgSel==(int)i);
        if(s) rect_fill(bx+20,y-6,bw-40,34,C_ORANGE);
        text(bx+40,y,n->choices[i].text,2,s?C_BG:C_INK);
    }
    if(n->choices.empty()) text(bx+bw-80,by+bh-30,"A>",2,C_ORANGE);
}

bool story_flag(const std::string&f){ auto it=G.flags.find(f); return it!=G.flags.end()&&it->second=="1"; }
void story_set_flag(const std::string&f,const std::string&v){ G.flags[f]=v; }

void story_load(){
    G.days.clear();
    const char* titles[31]={
      "DALIA'S REQUEST","GABRIEL AND THE HORNETS","AMIR'S PAPERCRAFT",
      "THE ELEVATOR","ICE CREAM RUN","LOST AND FOUND","POWER OUTAGE",
      "THE ROOF","NINO'S GARDEN","T-002 WATCHES","MIRROR FLOOR","THE ARCADE",
      "LEA'S LETTER","SAMI'S CLOCK","MIDNIGHT SHIFT","THE BOILER",
      "A QUIET FLOOR","THE ARCHIVE","BLUE PRINTS","THE VISITOR",
      "SHADOW AMIR","ARWA'S SONG","THE TRAIN","BELOW B3","THE ORANGE DOOR",
      "KEEPER OF 14","THE REPLICA","ECHOES","THE LAST STAIRWAY","ALL FLOORS",
      "ILOT14 FOREVER"};
    const char* sums[31]={
      "DALIA ASKS YOU TO FIND THE STAIRWAY TO HELL.",
      "MEET AMIR. HORNETS NEST. YOU LOSE.",
      "CRAFT DAY. T-002 BUMPS AMIR. FALLS INTO BACKROOMS.",
      "THE ELEVATOR STARTS TALKING BACK.","THE ICE CREAM SHOP ON THE MAIN ROAD.",
      "FIND WHAT AMIR DROPPED IN THE BACKROOMS.","THE BUILDING LOSES POWER. BOTS ROAM.",
      "REACH THE ROOF. SOMETHING WAITS.","NINO GREW A GARDEN ON C-001.",
      "T-002 FOLLOWS YOU. DO NOT LOOK BACK.","THE MIRROR FLOOR SHOWS ANOTHER 14.",
      "OLD ARCADE ON B1 STILL WORKS.","LEA LEFT A LETTER UNDER A BED.",
      "SAMI'S CLOCK RUNS BACKWARDS.","THE NIGHT SHIFT ARRIVES.",
      "THE BOILER ROOM HUMS.","A FLOOR NO ONE TALKS ABOUT.",
      "THE ARCHIVE HAS YOUR FILE.","FLOOR PLANS AND SECRETS.",
      "SOMEONE KNOCKS AT L-001.","AMIR SEES HIMSELF.","ARWA SINGS TO THE ELEVATOR.",
      "A TRAIN THAT SHOULDN'T EXIST.","DEEPER THAN B3.","THE ORANGE DOOR OPENS.",
      "WHO KEEPS ILOT14 RUNNING.","A REPLICA OF THE WHOLE BUILDING.",
      "VOICES FROM EVERY FLOOR.","THE LAST STAIRWAY DOWN.","EVERY FLOOR AT ONCE.",
      "ILOT14 NEVER ENDS."};
    for(int i=0;i<31;i++){
        Day d; d.index=i+1; d.title=titles[i]; d.summary=sums[i];
        G.days.push_back(d);
    }
}

static void day1(){
    static std::vector<DialogueNode> n={
      {0,"DALIA","HEY. YOU'RE NEW. I NEED A FAVOR. FIND THE STAIRWAY TO HELL. IT'S BEHIND A SECRET DOOR IN THE LOBBY.",{},1,"",""},
      {1,"DALIA","TAKE THE DOOR. FIND GABRIEL OUT IN THE PARKING. COME BACK WHEN YOU DO.",{{"OK",-1,"d1_accept"}},-1,"",""},
    };
    dlg_start(n,0);
}
static void day2(){
    static std::vector<DialogueNode> n={
      {0,"GABRIEL","YOU'RE DALIA'S RUNNER? GOOD. THERE'S A HORNET NEST UP ON M2.",{},1,"",""},
      {1,"GABRIEL","AMIR SAYS HE KNOWS A WAY. GO WITH HIM.",{{"LET'S GO",2,"d2_start"}},-1,"",""},
      {2,"AMIR","I'LL GO FIRST. IF I YELL, RUN.",{},3,"",""},
      {3,"NARRATOR","THE NEST FALLS. EVERYTHING GOES DARK.",{},4,"",""},
      {4,"AMIR","...I THINK WE'RE IN THE PARKING. LET'S NOT TELL DALIA.",{{"AGREED",-1,"d2_done"}},-1,"",""},
    };
    dlg_start(n,0);
}
static void day3(){
    static std::vector<DialogueNode> n={
      {0,"AMIR","PAPERCRAFT DAY. GRAB PAPER ROLLS, GLUE, UHU, PENS, RULERS.",{},1,"",""},
      {1,"AMIR","CAREFUL ON THE STAIRS. T-002 IS-",{},2,"",""},
      {2,"NARRATOR","AMIR BUMPS INTO T-002. HE FLIES FACE FIRST. THE FLOOR OPENS.",{},3,"",""},
      {3,"NARRATOR","YOU'RE IN THE BACKROOMS.",{{"FIND HIM",-1,"d3_backrooms"},{"PANIC",-1,"d3_backrooms"}},-1,"",""},
    };
    dlg_start(n,0);
}

void story_start_day(int d){
    switch(d){
      case 1: day1(); break;
      case 2: day2(); break;
      case 3: day3(); break;
      default: {
          static std::vector<DialogueNode> n;
          n.clear();
          char b[128]; snprintf(b,sizeof(b),"DAY %d: %s",d,G.days[d-1].title.c_str());
          n.push_back({0,"JOURNAL",b,{},1,"",""});
          n.push_back({1,"JOURNAL",G.days[d-1].summary,{{"BEGIN",-1,""}},-1,"",""});
          dlg_start(n,0);
      }
    }
    story_set_flag("day_started_"+std::to_string(d));
}

void story_update(){
    NPC* npc = world_npc_at(G.player.floor,G.player.tx,G.player.ty);
    if(npc){
        if(G.day==1 && npc->id=="dalia") day1();
        else if(G.day==2 && npc->id=="gabriel") day2();
        else if(G.day==3 && npc->id=="amir") day3();
        else {
            static std::vector<DialogueNode> n={{0,"NPC","...",{{"HI",-1,""},{"BYE",-1,""}},-1,"",""}};
            n[0].speaker=npc->name;
            n[0].text=npc->line.empty()? "..." : npc->line;
            dlg_start(n,0);
        }
    }
}
