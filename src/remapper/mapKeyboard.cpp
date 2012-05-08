/*
 *  keyMapper.cpp
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "mapKeyboard.h"
#include "midiConfig.h"

extern ofColor white;
extern ofColor black;
extern ofColor yellow;
extern ofColor gray;
extern ofColor blue;

remapKey::remapKey(double _w, double _h, char nt):pianoKey()
{
	note=nt;
	w=_w;
	h=_h;
	o_w=_w;
	o_h=_h;
	bSharp=false;
	bSelected=false;
	for (int j=0; j<1; j++) {
		notes.push_back(instrument("holder",1,60));
		notes.back().setDefault(true);
	}
	buttons.setup(2,25,OF_VERT,500);
	clearNotes.setup("Clear assigned instrument", 24);
}

remapOctave::remapOctave(double width,int numKeys,char octave_begin_note):pianoOctave()
{
	w=0,h=width/1.2;
	double wwid=width/7.;
	int sharps[5]={1,3,6,8,10};
	for (int i=0; i<numKeys; i++) {
		char nt=octave_begin_note+i;
		keys.push_back(remapKey(wwid,h,nt));
		for(int j=0; j<5; j++){
			if(sharps[j]==i) keys[i].setSharp(true);
		}
	}
	for(unsigned int i=0; i<keys.size(); i++){
		if(!keys[i].isSharp()) w+=keys[i].w;
	}
}

void remapKeyboard::setup(double wid,double nOctaves, unsigned char chan)
{
	w=wid;
	channel=chan;
	//programs.setTextSize(20);
	if(programs.size()<getSynthInstruments().size())
		for (unsigned int i=0; i<getSynthInstruments().size(); i++) {
			programs.setValue(getSynthInstruments()[i].name);
		}
	programs.setMode(false);
  programs.dallasStyle();
	printOut.loadFont("fonts/HelveticaCond.otf");
	printOut.setSize(26);
	printOut.setMode(OF_FONT_LEFT);
	printOut.setMode(OF_FONT_TOP);
	octaves.clear();
	for (int i=0; i<nOctaves; i++) {
		octaves.push_back(remapOctave(wid/(nOctaves+1/7.),12,i*12));
	}
	octaves.push_back(remapOctave(wid/(nOctaves+1/7.),1,nOctaves*12));
	pressKey(0);
	framePad.y=wid/64;
	framePad.x=wid/64.;
	w=w+framePad.x*2;
	h=octaves[0].h+framePad.y;
	clearMapped.setup("Clear all accoustic instruments", 22);
  programs.w=max(programs.w,clearMapped.w);

  changeProgram(programs.getChoiceNumber());
}

void remapKeyboard::draw(double _x, double _y){
	pianoKeyboard::draw(_x,_y);
}

void remapKeyboard::draw(double _x, double _y,double _w, double _h){
  x=_x+(_w-w)/2;
  
  info=ofRectangle(x,y+h+20,w,printOut.stringHeight("Kjhg")*3);
  
  int pad=(_h-(h+info.height+20))/2;
  
  y=_y+pad;
  info.y=y+h+20;
  
	pianoKeyboard::draw(x,y);
  drawKeyInfo();
}

string getNoteName(unsigned char note)
{
	string ret;
	//string octave=ofToString((MIDI_KEYBOARD_START+note)/12);
	int k=note%12;
	ret=(k<2)?"C":(k<4)?"D":(k==4)?"E":(k<7)?"F":(k<9)?"G":(k<11)?"A":"B";
	if(k==1||k==3||k==6||k==8||k==10) ret+="#";
	return	ret;//+octave;
}



void remapKeyboard::drawKeyboardControls(int _x, int _y, int _w, int _h)
{
  ofPoint margin(50,50);
  ofPoint pad(20,20);
  
  _y-=3*pad.y/2;
  
  ofSetColor(yellow);
  printOut.setSize(21);
	printOut.setMode(OF_FONT_LEFT);
	printOut.setMode(OF_FONT_TOP);
	printOut.drawString("Clear assigned instruments from entire keyboard", _x+pad.x, _y+_h/5);
	printOut.drawString("Select synthesized instrument for unassigned keys", _x+pad.x, _y+3*_h/5+pad.y);
	clearMapped.draw(_x+pad.x,_y+2*_h/5);
	programs.draw(_x+pad.x,_y+4*_h/5+pad.y);
}

void remapKeyboard::drawKeyInfo()
{
  ofPoint margin(25,25);
  ofPoint pad(20,15);
  
  //info=ofRectangle(_x, _y+margin.y, _w, _h-margin.y);
  
	pianoKey * key=0;
	for (unsigned int i=0; i<size(); i++) {
		if ((*this)[i].isSelected()) {
			key=&((*this)[i]);
		}
	}
	if(key){
		pianoKey & k=*key;
    
    printOut.setSize(29);
    printOut.setMode(OF_FONT_MID);
    string line="Press this key to play ";
    if(!key->notes[0].isDefault()){
			line+=key->notes[0].title;
      //key->clearNotes.draw(box.x+(box.width-k.clearNotes.w)/2,box.y+30+printOut.stringHeight(line)+10);
		}
    else {
      line+=getNoteName(key->notes[0].base.note%12)+" on the ";
      line+=programs.getString();
    }
    
    //_-_-_-_-_ draw the shape of the info box
    ofFill();
    ofSetLineWidth(1);
    ofColor keyColor;
		if(!k.notes[0].isDefault()){
      if(k.isSharp()) keyColor=k.notes[0].base.color-.2*255.;
      else keyColor=k.notes[0].base.color;
    }
    else if(!k.isSharp()) keyColor=white;
    else keyColor=black;
    ofSetColor(keyColor);
    
    ofEnableSmoothing();
    ofRect(info.x, info.y, info.width, info.height);
    ofNoFill();
    if(k.isSharp()) ofSetColor(white);
    else ofSetColor(black);
    ofSetLineWidth(3);
    ofRect(info.x, info.y, info.width, info.height);
    int kH=k.y+k.h-20;
    int offSet=k.w/2;
    ofFill();
    ofSetColor(keyColor);
    ofTriangle(k.x+k.w/2, kH+2, k.x+k.w/2-offSet, info.y+2, k.x+k.w/2+offSet, info.y+2);
    if(k.isSharp()) ofSetColor(white);
    else ofSetColor(black);
    ofLine(k.x+k.w/2-offSet, info.y, k.x+k.w/2, kH);
    ofLine(k.x+k.w/2,kH,k.x+k.w/2+offSet,info.y);
    ofDisableSmoothing();
    ofSetLineWidth(2);

    if(k.isSharp()) ofSetColor(white);
    else ofSetColor(black);
    printOut.drawString(line, info.x+framePad.x, info.y+info.height/2);
    
    if(!key->notes[0].isDefault()){
      key->clearNotes.draw(info.x+info.width-(k.clearNotes.w+framePad.x),info.y+(info.height-key->clearNotes.h)/2);
		}
	}
}

bool remapKeyboard::clickDown(int _x, int _y)
{
	bool ret=false;
	if(programs.clickDown(_x, _y)){
		if(programs.isSelected()){
			changeProgram(programs.getChoiceNumber());
		}
	}
	else if(programs.justSelected()){
		changeProgram(programs.getChoiceNumber());
	}
	else if(clearMapped.clickDown(_x, _y)){
		for (unsigned int i=0; i<(*this).size(); i++) {
			(*this)[i].resetInstruments();
			selectButton(i,0);
		}
	}
	else if(ret=pianoKeyboard::clickDown(_x,_y)){
		if(getKey().notes[0].isDefault()) getKey().notes[0].base.note=getKey().getNote()+MIDI_KEYBOARD_START,getKey().notes[0].base.channel=1;
		getKey().notes[0].play();
	}
	return ret;
}

void remapKeyboard::changeProgram(int choiceNum)
{
	vector<unsigned char> msg;
	msg.push_back(MIDI_PROGRAM_CHANGE+channel);
	msg.push_back(getSynthInstruments()[choiceNum].number);
	ofGetAppPtr()->midiToSend(msg);
}

bool remapKeyboard::clickUp()
{
	programs.clickUp();
	clearMapped.clickUp();
	if(pianoKeyboard::clickUp())
    getKey().notes[0].stop();
	return false;
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

bool remapKeyboard::selectButton(int num){
	getKey().buttons.select(num);
	return false;
}

bool remapKeyboard::selectButton(int key, int num){
	(*this)[key].buttons.select(num);
	return false;
}