/*
 *  keyMapper.cpp
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "mapKeyboard.h"

extern ofColor white;
extern ofColor black;
extern ofColor yellow;
extern ofColor gray;

remapKey::remapKey(double _w, double _h, char nt):pianoKey()
{
	note=nt;
	w=_w;
	h=_h;
	o_w=_w;
	o_h=_h;
	bSharp=false;
	bSelected=false;
	for (int j=0; j<3; j++) {
		int cur=notes.size();
		notes.push_back(instrument("holder",1,60));
		notes[cur].setDefault(true);
	}
	buttons.setup(2,25,OF_VERT,500);
	clearNotes.setup("clear mapped note", "fonts/Arial.ttf", 24);
}

remapOctave::remapOctave(double width,char octave_begin_note):pianoOctave()
{
	w=width,h=width/1.2;
	double wwid=w/7.;
	int sharps[5]={1,3,6,8,10};
	for (int i=0; i<12; i++) {
		char nt=octave_begin_note+i;
		keys.push_back(remapKey(wwid,h,nt));
	}
	for (unsigned int i=0; i<5; i++) {
		keys[sharps[i]].setSharp(true);
	}
}

void remapKeyboard::setup(double wid,double nOctaves, unsigned char chan)
{
	w=wid;
	channel=chan;
	int numProgs=loadProgramNames("midiPrograms.ini");
	programs.setTextSize(20);
	for (unsigned int i=0; i<getProgramNames().size(); i++) {
		programs.setValue(getProgramNames()[i]);
	}
	programs.setMode(false);
	printOut.loadFont("fonts/DinC.ttf");
	printOut.setSize(24);
	printOut.setMode(OF_FONT_LEFT);
	printOut.setMode(OF_FONT_TOP);
	octaves.clear();
	for (int i=0; i<nOctaves; i++) {
		octaves.push_back(remapOctave(wid/nOctaves,i*12));
	}
	pressKey(0);
	xDis=wid/16.;
	w=w+xDis;
	h=octaves[0].h+xDis/2.;
	clearMapped.setup("reset", "Arial.ttf", 24);
}

void remapKeyboard::draw(double _x, double _y){
	pianoKeyboard::draw(_x,_y);
}

string getNoteName(unsigned char note)
{
	string ret;
	string octave=ofToString((MIDI_KEYBOARD_START+note)/12);
	int k=note%12;
	ret=(k<2)?"C":(k<4)?"D":(k==4)?"E":(k<7)?"F":(k<9)?"G":(k<11)?"A":"B";
	if(k==1||k==3||k==6||k==8||k==10) ret+="#";
	return	ret+octave;
}



void remapKeyboard::drawKeyboardControls(int _x, int _y, int _w, int _h)
{
	double offset=50;
	double binLeft=_x+offset/2;
	double binTop=_y+offset/2;
	double binW=_w-offset;
	double binH=_h-offset;
  ofSetShadowDarkness(.4);
	ofShadowRounded(binLeft, binTop, binW, binH, 15, 10);
	ofRect(binLeft, binTop, binW, binH);
	
	double yPos=_y+offset*1/2+10;
	double xPos=_x+offset*1/2+10;
	ofSetColor(255, 255, 255);
  printOut.setSize(24);
	printOut.setMode(OF_FONT_LEFT);
	printOut.setMode(OF_FONT_TOP);
	printOut.drawString("Select Default Instrument", xPos, yPos+offset*2);
	ofSetColor(255, 255, 255);
	printOut.drawString("Clear assigned instruments from entire keyboard", xPos, yPos);
	clearMapped.draw(xPos+offset, yPos+offset);
	programs.draw(xPos+offset, yPos+offset*3);
}

void remapKeyboard::drawKeyInfo(int _x, int _y, int _w, int _h)
{
  int offset=50;
  ofRectangle box(_x+offset/2+20, _y+offset/2, _w-offset-40, _h-offset*2);
  
	pianoKey * key=0;
	for (unsigned int i=0; i<size(); i++) {
		if ((*this)[i].isSelected()) {
			key=&((*this)[i]);
		}
	}
	if(key){
		pianoKey & k=*key;
    
    //_-_-_-_-_ draw the shape of the box
		ofSetColor(black.opacity(.75));
		ofBeginShape();
    ofVertex(key->x, key->y+key->h);
    ofBezierVertex(k.x, k.y+k.h, k.x+(k.x-box.x)/20,box.y,box.x,box.y);
		ofVertex(box.x+box.width, box.y);
    ofBezierVertex(box.x+box.width,box.y, (k.x+k.w)+((k.x+k.w)-(box.x+box.width))/20,box.y,k.x+k.w, k.y+k.h);
		ofEndShape(true);
    ofFlat();
		ofRoundedRect(box.x-20, box.y, box.width+40, box.height, 20);
    
    
		/*ofSetColor(255,255,255);
		string intro="when this key is pressed...";
		printOut.drawString(intro, _x+offset, _y+offset);
		int nextLine=_y+offset*2+printOut.stringHeight(intro);
		int xDrawPosition=_x+2*offset;
		key->buttons[0].draw(xDrawPosition,nextLine);
		xDrawPosition+=key->buttons[0].w+offset/2;
		string dflt="play default instrument";
		ofSetColor(255,255,255);
		printOut.drawString(dflt, xDrawPosition, nextLine);
		xDrawPosition+=printOut.stringWidth(dflt)+offset*3;
		key->buttons[1].draw(xDrawPosition,nextLine);
		xDrawPosition+=key->buttons[1].w+offset/2;
		string mpd="play mapped note";
		ofSetColor(255,255,255);
		printOut.drawString(mpd, xDrawPosition, nextLine);
		xDrawPosition+=printOut.stringWidth(mpd)+offset;
		if(!key->notes[0].isDefault()){
			key->notes[0].draw(xDrawPosition,nextLine);
			key->clearNotes.draw(xDrawPosition+key->notes[0].w+offset, nextLine);
		}
		else {
			printOut.drawString("[no note is mapped, drag block to key]", xDrawPosition, nextLine);
		}*/

    printOut.setMode(OF_FONT_CENTER);
    printOut.setMode(OF_FONT_TOP);
    printOut.setSize(30);
    string line="When key is pressed, play ";
    if(!key->notes[0].isDefault()){
			line+=key->notes[0].title;
      key->clearNotes.draw(box.x+(box.width-k.clearNotes.w)/2,box.y+30+printOut.stringHeight(line)+10);
		}
    else {
      line+=programs.getString();
    }

    ofSetColor(white);
    printOut.drawString(line, box.x+box.width/2, box.y+30);
	}
}

bool remapKeyboard::clickDown(int _x, int _y)
{
	bool ret=false;
	if(programs.clickDown(_x, _y)){
		if(programs.isSelected()){
			vector<unsigned char> msg;
			msg.push_back(MIDI_PROGRAM_CHANGE+channel);
			msg.push_back(programs.getChoiceNumber());
			ofGetAppPtr()->midiToSend(msg);
		}
	}
	else if(programs.justSelected()){
		vector<unsigned char> msg;
		msg.push_back(MIDI_PROGRAM_CHANGE+channel);
		msg.push_back(programs.getChoiceNumber());
		ofGetAppPtr()->midiToSend(msg);
	}
	else if(clearMapped.clickDown(_x, _y)){
		for (unsigned int i=0; i<(*this).size(); i++) {
			(*this)[i].resetInstruments();
			selectButton(i,0);
		}
	}
	else if(ret=pianoKeyboard::clickDown(_x,_y)){
    if(getKey().notes[0].isDefault()) getKey().notes[0].base.note=getKey().getNote()+MIDI_KEYBOARD_START;
    getKey().notes[0].play();
  }
	return ret;
}

bool remapKeyboard::clickUp()
{
	programs.clickUp();
	clearMapped.clickUp();
	if(pianoKeyboard::clickUp())
    getKey().notes[0].stop();
  
}

int remapKeyboard::getButtonChoice(int num)
{
	if(num<0) num=activeKey();
	return (*this)[num].buttons.getChoice();
}

vector<instrument> & remapKeyboard::getActiveNotes()
{
	return getKey().notes;
}

vector<instrument> & remapKeyboard::getNotes(int num)
{
	return (*this)[num].notes;
}
