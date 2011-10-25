/*
 *  keyRemapper.cpp
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "keyRemapper.h"

void remapper::setup()
{
	yDis=50;
	kb.setup(7*ofGetWidth()/8., 4);
	band.setup(parseFile("instruments.xml"));
	band.setHeight(ofGetHeight()-(kb.h+yDis),0,0);
	pianoBackground.loadImage("images/woodPanel.jpg");
	dinc.loadFont("fonts/DinC.ttf");
	dinc.setSize(35);
	dinc.setMode(OF_FONT_TOP);
}

void remapper::update(int num)
{
	band.setHeight(ofGetHeight()-(kb.h+yDis+num),0,0);
	band.update();
}

void remapper::draw(double _x, double _y)
{
	
	/*ofSetColor(0x90734B);
	ofRect(_x, _y, ofGetWidth(), band.h/2);
	ofShade(_x,_y+ band.h/2, 10, ofGetWidth(), OF_DOWN, .3);
	ofShade(_x,_y+ band.h/2, 20, ofGetWidth(), OF_UP, .3);*/
	
	
	//------- Draw the keyboard under the band, and draw a shadow on the background under the keyboard
	
	//ofSetColor(195,172,155);
	//ofSetColor(0xffffff); // for steel background
  
  //Draw the keyboard background
	//ofSetColor(0xaaaaaa);
	//pianoBackground.draw(-10, kb.y-yDis/2,ofGetWidth()+10,kb.h+yDis);
  
  ofSetColor(0x333333);
  ofRaised(.4);
  ofRoundedRect(0, kb.y-yDis/2, ofGetWidth(), kb.h+yDis, 4);
  
  
	double indent=30;
	ofShadowRounded(kb.x, kb.y, kb.w, kb.h, indent, indent);
	
	kb.draw((ofGetWidth()-kb.w)/2, band.y+band.h+20);
	
	
	//------ Draw the shadow below the keyboard
	ofShade(_x, kb.y+kb.h+yDis/2, 10, ofGetWidth(), OF_UP, .3);
	ofShade(_x, kb.y+kb.h+yDis/2, 15, ofGetWidth(), OF_DOWN, .5);
	//ofShade(_x, kb.y-yDis/2, 10, ofGetWidth(), OF_UP, .3);
	
	//-------- draw the band and the shading to the right of the instruments
	int verticalAug=70;
	band.draw(_x, _y+verticalAug);
	band.drawInstruments();
  ofSetShadowDarkness(.3);
	ofShade(band.x+band.w+20, _y, 15, band.h+verticalAug, OF_LEFT);
	ofShade(band.x+band.w+20, _y, 15, band.h+verticalAug, OF_RIGHT,false);
	ofSetColor(0x333333);
	dinc.drawString("INSTRUMENTS", _x+50, _y+(verticalAug-dinc.stringHeight("I"))/2);
	
	//------ Draw the light shade over the keyboard
	//ofShade(band.x, band.y+band.h, 15, ofGetWidth(), OF_DOWN, .5,false); //for steel
	ofShade(band.x, band.y+band.h, 15, ofGetWidth(), OF_DOWN, .5); // for wood
	
	//------- draw the controls fro setting default and clearing mapped notes
	kb.drawKeyboardControls(band.x+band.w+20, band.y-20, ofGetWidth()-(band.x+band.w+20), band.h+20);
	ofSetColor(0x333333);
	dinc.drawString("SETTINGS", band.x+band.w+70, _y+(verticalAug-dinc.stringHeight("I"))/2);
	
	//------- draw the information for the currently selected key
	kb.drawKeyInfo(0, kb.y+kb.h+yDis, ofGetWidth(), ofGetHeight()-(kb.y+kb.h+yDis/2));
	
	//------- draw the block in hand
	held.drawBackground();
}

bool remapper::clickDown(int _x, int _y)
{
	if(kb.clickDown(_x, _y));
	for (unsigned int i=0; i<kb.getActiveNotes().size(); i++) {
		/*if(kb.getActiveNotes()[i].over(_x,_y)){
			held=kb.getActiveNotes()[i];
			kb.getActiveNotes()[i].setDefault(true);
			held.clickDown(_x, _y);
		}*/
	}
	if(band.clickDown(_x, _y)){
		//band.lastInstrument().clickUp();
		//band.lastInstrument().clear();
		held=band.lastInstrument();
		held.clickDown(_x, _y);
	}
	kb.getKey().buttons.clickDown(_x, _y);
}

bool remapper::clickUp(int _x, int _y)
{
	kb.clickUp();
	/*if(band.clickUp(kb.getActiveNotes())){
		kb.selectButton(1);
	}*/
  if(band.clickUp()){
    kb.selectButton(1);
  }
	for (unsigned int i=0; i<kb.getActiveNotes().size(); i++) {
		kb.getActiveNotes()[i].clickUp();
	}
	if(held.isHeld()){
		bool ret=false;
		for (unsigned int i=0; i<kb.size(); i++) {
			if(kb[i].isSharp()&&!ret&&kb[i].over(_x, _y)){
				kb.getNotes(i)[0]=held;
				kb.getNotes(i)[0].setDefault(false);
				//kb.getNotes(i)[0].resizeByFont(40);
				kb.selectButton(i,1);
				ret=true;
				//kb[i].select(false);
			}
		}
		for (unsigned int i=0; i<kb.size(); i++) {
			if(!kb[i].isSharp()&&!ret&&kb[i].over(_x, _y)){
				kb.getNotes(i)[0]=held;
				kb.getNotes(i)[0].setDefault(false);
				kb.selectButton(i,1);
				//kb[i].select(false);
				ret=true;
			}
		}
		held.clickUp();
		held.clear();
	}
	//if(held.clickUp(kb.getActiveNotes()))
	//	kb.selectButton(1);
	kb.getKey().buttons.clickUp();
}

void remapper::drag(int _x, int _y)
{
	band.mouseMotion(_x, _y);
	held.mouseMotion(_x, _y);
  kb.mouseMotion(_x, _y);
	if(held.isHeld()){
		bool tmp=false;
		for (unsigned int i=0; i<kb.size(); i++) {
			if(kb[i].isSharp()&&kb[i].over(_x, _y)&&!tmp){
				tmp=true;
				kb.pressKey(i);
			}
			else kb[i].select(false);
		}
		for (unsigned int i=0; i<kb.size(); i++) {
			if(!kb[i].isSharp()&&kb[i].over(_x, _y)&&!tmp){
				tmp=true;
				kb.pressKey(i);
			}
			else if(!kb[i].over(_x, _y)) kb[i].select(false);
		}
	}
}

void remapper::resize(int _w, int _h)
{
	kb.setup(7*ofGetWidth()/8., 4);
	band.setHeight(ofGetHeight()-(kb.h+yDis),0,0);
	band.update();
}