
#include "ZeroTheorem.h"
#include "Region.h"
#include "script.h"
#include "Deck.h"

#include "boost/bind.hpp"
#include "boost/algorithm/string/join.hpp"

#include "ServerWrapper.h"

struct BoxArea {
    int x,y,w,h,c;
    string target;
};

struct Trigger {
    char key;
    string command;
};

static vector<string> script;
static vector<Trigger> triggers;
static vector<BoxArea> onevents;
static string ondone;
static int script_pc = 0;
static Region* focus;
static fs::path path = DEFAULTFOLDER "/scripts/script.txt";
static int loadcount = 0;
static int delaytimer = 0;
static string delayondone;
static int mousedown = 0;

void script_load () {
    
    script.clear();
    triggers.clear();

    if(!loadcount) {
        loadcount++;
        // load a file the first time in only - successive resets just reset
        //fs::path path = DEFAULTFOLDER "/script_day_.txt"; //getOpenFilePath();
        fs::path path_temp = getOpenFilePath();
        if(!path_temp.empty()) {
            path = path_temp;
        }
    }
	if( path.empty() ) exit(0);

    string line;
    char buffer[1000];
    std::cout << "Current directory is: " << getcwd(buffer, 1000) << "\n";
    ifstream myfile(path.string().c_str());
    if (myfile.is_open()) {
        while (myfile.good()) {
            getline (myfile,line);
            script.push_back(line);
        }
        myfile.close();
    } else {
        cout << "Unable to open file";
        exit(0);
    }
    script_pc = 0;
}

void script_step() {
    if(script_pc < script.size()) script_pc++;
}

Region* script_get_movie(const char* filename) {
    Region* r = 0;
    for(int i = 0; i < regions.size() ; i++) {
        if(regions[i]->filename == filename) {
            r = regions[i];
            break;
        }
    }
    return r;
}

void script_set_movie(const char* filename) {
    Region* r = 0;
    for(int i = 0; i < regions.size() ; i++) {
        if(regions[i]->filename == filename) {
            r = regions[i];
            break;
        }
    }
    if(!r) {
        // we have to make the movie from scratch!
        r = new Region( 0,HEIGHT,   0,   0,   0,REGION_MOVIE,filename);
        regions.push_back(r);
    }
    // we found we already had this movie loaded earlier... just restart it
    focus = r;
}

void script_goto(string label);

bool script_run_command(string command) {

    // split it
    vector <string> scratch = cinder::split( command, ' ' );
    vector <string> fields;
    for(int z = 0; z < scratch.size(); z++) {
        if(scratch[z].size() > 0) fields.push_back(scratch[z]); // remove the spaces
    }
        
    if(fields.size()) {

        console() << "running " << command << endl;
        
        string term = fields[0];

        if(term == "video2" && fields.size() > 1) {
            Region* r = 0;
            string filename = fields[1];
            for(int i = 0; i < regions.size() ; i++) {
                if(regions[i]->filename == filename) {
                    r = regions[i];
                    break;
                }
            }
            if(!r) {
                r = new Region(  400, 400, 400,400,   0,REGION_VIDEO,"");
                regions.push_back(r);
                r->filename = filename;
            }
            focus = r;
        }

        if(term == "words" && fields.size() > 1) {
            Region* r = 0;
            string filename = fields[1];
            for(int i = 0; i < regions.size() ; i++) {
                if(regions[i]->kind == REGION_WORDS) {
                    r = regions[i];
                    break;
                }
            }
            if(!r) {
                
                regions.push_back(new Region(  55,   0,   0,1080,   0,REGION_WORDS,
                                             "PEDAL TO THE MEDAL – THE GOLD MEDAL OF ENTITY CRUNCHING!"
                                             "FASTER, FARTHER, FURTHER, FORWARD!"
                                             "HURRY!  UNEMPLOYMENT IS GAINING ON YOU!"
                                           //  "GREAT JOB!  IT’S ALL DOWNHILL FROM HERE!"
                                        //     "YOU’VE GOT THE SLOOOOOOWS…"
                                        //     "DON’T SHIRK IT – WORK IT!"
                                        //     "IDLE FEET ARE THE DEVIL’S PLAYGROUND…"
                                       //      "FULL SPEED AHEAD!  STAY THE COURSE!"
                                          //   "YOU’RE ALL CHARGED UP!  RELAX AND COAST A BIT…"
                                       //      "HIGH RPM = HIGH PAY!"
                                       //      "LOW RPM = UNEMPLOYMENT LINE!"
                                       //      "MUSH!"
                                       //      "YOU’RE STROLLING – GET SCROLLING!"
                                       //      "COFFEE BREAK’S OVER – BACK ON YOUR TREADS!"
                                             ));
                regions.back()->speed = 15;
            }
            focus = r;

        }

        if(term == "video" && fields.size() > 1) {
            Region* r = 0;
            string filename = fields[1];
            for(int i = 0; i < regions.size() ; i++) {
                if(regions[i]->filename == filename) {
                    r = regions[i];
                    break;
                }
            }
            if(!r) {
                r = new Region(  400, 400, DECKWIDTH/DECKRATIO, DECKHEIGHT/DECKRATIO,   0,REGION_DECKV,"");
                regions.push_back(r);
                r->filename = filename;
            }
            focus = r;
        }
        
        if(term == "image" && fields.size() > 1) {
            Region* r = 0;
            string filename = fields[1];
            for(int i = 0; i < regions.size() ; i++) {
                if(regions[i]->filename == filename) {
                    r = regions[i];
                    break;
                }
            }
            if(!r) {
                r = new Region(   0,   0,1920,1080,   0,REGION_IMAGE,filename);
                regions.push_back(r);
                r->filename = filename;
            }
            focus = r;
            r->visible = 1;
        }

        else if(term == "reset" && fields.size() > 1) {
            script_set_movie(fields[1].c_str());
            if(focus) focus->reset();
        }

        else if((term == "rate" || term == "speed") && fields.size() > 1) {
            float rate = atof(fields[1].c_str());
            if(focus) focus->rate = rate;
            console() << "set " << focus->filename << " to speed " << rate << endl;
        }
        else if((term == "fasterer") && fields.size() > 1) {
            float rate = atof(fields[1].c_str());
            for(int i = 0; i < regions.size(); i++) {
                regions[i]->rate += rate;
            }
        }
        else if(term == "play" && fields.size() > 1) {
            // find or load movie and start play from start
            script_set_movie(fields[1].c_str());
            if(focus) focus->play();
        }

        else if(term == "move" && focus && fields.size() > 2) {
            // find or load movie and start play from start
            focus->x = atoi(fields[1].c_str());
            focus->y = atoi(fields[2].c_str());
        }

        else if(term == "size" && focus && fields.size() > 2) {
            // find or load movie and start play from start
            focus->w = atoi(fields[1].c_str());
            focus->h = atoi(fields[2].c_str());
        }

        else if(term == "rotate" && focus && fields.size() > 1) {
            // find or load movie and start play from start
            focus->rotate = atoi(fields[1].c_str());
        }

        else if(term == "load" && fields.size() > 1) {
            script_set_movie(fields[1].c_str());
        }

        else if((term == "stop" || term == "pause") && fields.size() > 1) {
            script_set_movie(fields[1].c_str());
            if(focus) focus->stop();
        }
        
        else if(term == "hide" && fields.size() > 1) {
            Region * r = script_get_movie(fields[1].c_str());
            if(r) r->hide();
        }

        else if(term == "show" && fields.size() > 1) {
            script_set_movie(fields[1].c_str());
            if(focus) focus->visible = 1;
            if(focus) focus->dirty = 1; // mark as dirty so that we force up a frame
            if(focus && focus->movie) focus->movie.seekToFrame( focus->cframe ); // force seek to that frame too
        }

        else if(term == "unhide" && fields.size() > 1) {
            Region* r = script_get_movie(fields[1].c_str());
            if(r) r->visible = 1;
            if(r) r->dirty = 1; // mark as dirty so that we force up a frame
        }
        
        else if(term == "rangehard" && fields.size() > 1 && focus) {
            focus->range_low_hard = atoi(fields[1].c_str());
        }

        else if(term == "range" && fields.size() > 2 && focus) {
            int low = atoi(fields[1].c_str());
            int high = atoi(fields[2].c_str());
            if(low<0)low=0;
            if(high>focus->nframes) high=focus->nframes;
            focus->range_low = low;
            focus->range_high = high;
            focus->done = 0;
        }

        else if(term == "onmouse" && fields.size() > 5) {
            BoxArea box;
            box.x = atoi(fields[1].c_str());
            box.y = atoi(fields[2].c_str());
            box.w = atoi(fields[3].c_str());
            box.h = atoi(fields[4].c_str());
            box.c = 0;
            box.target = fields[5];
            onevents.push_back(box);
        }

        else if(term == "onkey" && fields.size() > 2) {
            BoxArea box;
            box.x = box.y = box.w = box.h = 0;
            box.c = atoi(fields[1].c_str());
            box.target = fields[2];
            onevents.push_back(box);
        }

        else if(term == "ondone" && fields.size() > 1) {
            ondone = fields[1];
        }

        else if(term == "goto" && fields.size() > 1) {
            script_goto(fields[1]);
        }

        else if(term == "loops" && fields.size() > 1) {
            int loopcount = atoi(fields[1].c_str());
            if(focus) {
                focus->loopcount = loopcount;
            }
        }

        else if(term == "looping" && fields.size() > 1) {
            int looping = atoi(fields[1].c_str());
            if(focus) {
                focus->looping = looping;
            }
        }

        else if(term == "delay" && fields.size() > 2) {
            delayondone = fields[1];
            delaytimer = atoi(fields[2].c_str());
        }

        else if(term == "target" && fields.size() > 6 && focus) {
            focus->targetx = atoi(fields[1].c_str());
            focus->targety = atoi(fields[2].c_str());
            focus->targetw = atoi(fields[3].c_str());
            focus->targeth = atoi(fields[4].c_str());
            focus->targetease = atoi(fields[5].c_str());
            focus->targetspeed = atof(fields[6].c_str());
        }

        else if(term == "network") {
            serverStart();
            if(fields.size() > 1) {
                const char* msg = fields[1].c_str();
                const char* val = fields.size() < 3 ? "" : fields[2].c_str();
                serverBroadcastMessage(msg,val);
            }
        }

        else if(term == "netnext") {
            serverNextClient();
        }

        else if(term == "netprev") {
            serverPrevClient();
        }

        else if(term == "netnarrow") {
            serverStart();
            if(fields.size() > 1) {
                const char* msg = fields[1].c_str();
                const char* val = fields.size() < 3 ? "" : fields[2].c_str();
                serverNarrowcastNext(msg,val);
            }
        }

        else if(term == "key" && fields.size() > 2) {
            Trigger trigger;
            trigger.key = fields[1].c_str()[0];
            fields.erase(fields.begin()); // erase the "key" part of this
            fields.erase(fields.begin()); // erase the "key" part of this
            trigger.command = boost::algorithm::join(fields," "); // join back into a string
            triggers.push_back(trigger); // push onto a stack
        }
        
        // pingpong
        // animated effects on regions
        
        else if(term == "return") {
            if(focus && focus->looping && mousedown);
            else return 0;
        }


    }

    return 1;
}


void script_run_till_done() {
    while(1) {
        string command = script[script_pc];
        if(script_run_command(command)) {
            script_step();
        } else {
            break;
        }
    }
}


void script_goto(string label) {
    if(focus)focus->done = 0;
    ondone = string("");
    onevents.clear();
    for(int i = 0; i < script.size(); i++) {
        if(script[i] == label ) {
            script_pc = i;
            script_run_till_done();
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Script::update() {
    
    if(delaytimer > 0 && delayondone.size() > 0) {
        delaytimer--;
        if(!delaytimer) {
            script_goto(delayondone);
        }
    }
    
    // if we finished a movie then goto ondone event
    if(focus && focus->visible && focus->done && ondone.size()>0) {
        script_goto(ondone);
    }
}

void Script::setup() {
    script_load();
    script_run_till_done();
    // regions.push_back(new Region(  100, 200, 200, 75,   0,REGION_FPS,""));

}

void Script::mouseUp(MouseEvent event) {
    mousedown = 0;
}

void Script::mouseDown(MouseEvent event) {
    mousedown = 1;
    int x = event.getX();
    int y = event.getY();
    for(int i = 0; i < onevents.size();i++) {
        BoxArea box = onevents[i];
        if(box.x < x && x < box.x+box.w && box.y < y && y < box.y+box.h && box.target.size() > 0) {
            script_goto(box.target);
            break;
        }
    }
}

void Script::keyDown( KeyEvent event ) {

    // run all commands attached to this key
    for(int i = 0; i < triggers.size();i++) {
        if(triggers[i].key == event.getChar()) {
            console() << "running command " << triggers[i].command << endl;
            script_run_command(triggers[i].command);
        }
    }

    // reset by resetting stopping and hiding all regions and then reload script from disk xxx this should be a scriptable command
    if(event.getChar() == 'r') {
        for(int i = 0; i < regions.size();i++) {
            Region* r = regions[i];
            r->reset();
            r->stop();
            r->hide();
        }
        ondone = string("");
        onevents.clear();
        focus = 0;
        script_load();
        script_run_till_done();
        return;
    }

    // goto to the matching keyed area
    for(int i = 0; i < onevents.size();i++) {
        BoxArea box = onevents[i];
        if(box.c == event.getChar()) {
            script_goto(box.target);
            break;
        }
    }

}

//
// code:
// >>>>>- skip over loops perfectly tightly if detecting a mouse down
//         --- need another way to fix this a bit tighter.... adjust frames a bit
//
// - need new words - only show certain words periodically - not every word ( put source on each box? )
//
// script:
// - test d7; we need to be able to keyboard drive this off and on
//
// - test phone
//           - phone does not respect initialization rotation
//           - it might be nice to stop the phone after it buzzes... can i have a pause point somehow? how?
//           - the user can actually move the phone around on the screen - could disable that too
//


