/*
 *  remapInstruments.cpp
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "remapInstruments.h"

extern ofColor white;
extern ofColor black;
extern ofColor yellow;
extern ofColor gray;
extern ofColor blue;

bool remapInst::clickUp()
{
	base.clickUp();
	bool ret=false;
	for (unsigned int i=0; i<blocks.size(); i++) {
		if(blocks[i].clickUp()){
			ret=true;
			lastBlock=i;
			if (blocks[i].x<x+w) {
				ret=false;
				blocks.erase(blocks.begin()+i);
				lastBlock=-1;
			}
			stop();
		}
	}
	bHolding=false;
	return ret;
}

remapInst::remapInst(string title, unsigned char channel,unsigned char note)
{
	point=22;
  setup(title,channel,note);
	base.header.setSize(point);
	base.w=max(float(200), base.header.stringWidth(title)+20);
	base.h=max(float(20), base.header.stringHeight("Kjg")+10);
	w=base.w;
	h=base.h;
}

void remapInst::operator=(const instrument & t)
{
	w=t.w;
	base=t.base;
  base.color=t.base.color;
	bHolding=t.bHolding;
	blocks=t.blocks;
	title=t.title;
  setup(title,base.channel,base.note);
	scroll.x=t.scroll.x,scroll.y=t.scroll.y;
	lastBlock=t.lastBlock;
	bDefault=t.bDefault;
	point=t.point;
}


/*bool remapInst::over(instrument & t, int i)
{
	dragBlock & tmp=blocks[i];
	return ((tmp.x+tmp.xDisp>t.x&&tmp.x+tmp.xDisp<t.x+t.w)||(tmp.x+tmp.xDisp+tmp.w>=t.x&&tmp.x+tmp.xDisp+tmp.w<=t.x+t.w))&&\
	((tmp.y+tmp.yDisp>t.y&&tmp.y+tmp.yDisp<t.y+t.h)||(tmp.y+tmp.yDisp+tmp.h>=t.y&&tmp.y+tmp.yDisp+tmp.h<=t.y+t.h));
}*/

bool remapInst::clickDown(double _x, double _y)
{
	bool ret=0;
	if(!bHolding&&base.clickDown(_x, _y)){
		blocks.push_back(dragBlock(base));
		base.clickUp();
    blocks[blocks.size()-1].w=base.w;
		relMouse.x=_x-base.x;
		relPos.y=_y-base.y;
		play();
		bHolding=true;
		ret=1;
	}
	else for (unsigned int i=0; i<blocks.size(); i++) {
		if(!bHolding&&blocks[i].clickDown(_x,_y)){
			play();
			bHolding=true;
			relMouse.x=_x-blocks[i].x-scroll.x;
			relPos.y=_y-blocks[i].y-scroll.y;
		}
	}
	return ret;
}

void remapInst::draw(double _x, double _y)
{
	x=_x, y=_y;
	base.draw(x,y);
}

void remapInst::mouseMotion(double _x, double _y)
{
	for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].mouseMotion((blocks[i].pressed())?_x-relMouse.x:_x,_y);
	}
}

void remapInst::drawBackground()
{
	for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].soundBlock::draw();
	}
}

void remapBand::setup(xmlParse * config,double hgt)
{
  bar.w=rightBorder=0;
  blockMargin.x=10;
  blockMargin.y=5;
  numBlocksPerBin=4;
	clearBut.setup("clear blocks",24);
	clearBut.setAvailable(true);
	string font=config->prop;
	for (unsigned int i=0; i<config->size(); i++) {
		xmlParse xml=config->child[i];
		if (xml.label=="instrument") {
			string col=xml.prop;
			string title=xml.name;
			long color=strtol(col.c_str(),NULL,0);
			int curInst=instruments.size();
			unsigned char note, channel;
			bool percussive=false;
			double delay=0;
			map<string,int> list;
			list["note"]=0;
			list["channel"]=1;
			list["percussive"]=2;
			list["delay"]=3;
			list["dropdown"]=4;
			for(int j=0;j<xml.size();j++){
				string * node=xml.getSubnode(j);
				switch (list.find(node[0])->second) {
					case 0:
						note=ofToInt(node[1]);
						break;
					case 1:
						channel=ofToInt(node[1]);
						break;
					case 2:
						percussive=true;
						break;
					case 3:
						delay=ofToFloat(node[1]);
						break;
						break;
					default:
						break;
				}
			}
			instruments.push_back( new remapInst(title,channel,note));
			instruments[curInst]->setColor(color);
		}
	}
	configureSize();
}

void remapBand::configureSize()
{
  adjustSize();
  margin=ofPoint (20,20);
	binPad=ofPoint(20,20);
  numBlocksPerBin=4;
	numBins=min(16,int(instruments.size()));
  numBins=(numBins%4)?numBins/numBlocksPerBin+1:numBins/numBlocksPerBin;
  w=(bar.w+bin.width+binPad.x)*numBins+margin.x;
  h=numBlocksPerBin*(cell.y+blockMargin.y)-blockMargin.y+margin.y;
}

void remapBand::draw(double _x, double _y)
{
	x=_x,y=_y;
  
  int nbpb=numBlocksPerBin;
	//
	for(int i=0; i<numBins; i++){
		double binLeft=x+margin.x+(bin.width+binPad.x)*i;
		double binRight=x+margin.x+bin.width+(bin.width+binPad.x)*i;
    
    for (unsigned int j=0; j<nbpb&&j+i*nbpb<instruments.size()&&j+i*nbpb<16; j++) {
      instruments[j+i*nbpb]->draw(binLeft+blockMargin.x,y+margin.y+binPad.y/2+blockMargin.y/2+(cell.y+blockMargin.y*2)*j);
      if(j<numBlocksPerBin-1){
        double tmpY=instruments[j+i*nbpb]->y+cell.y;
      }
    }
	}
	
}

void remapBand::drawDraggedBlocks()
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->drawBackground();
	}
}

bool remapBand::clickDown(int _x, int _y)
{
	bool ret=false;
	for (unsigned int i=0; i<instruments.size(); i++) {
		if(!bHolding&&instruments[i]->over(_x,_y)){
			ret=bHolding=true;
			lastInst=i;
		}
	}
	bar.clickDown(_x, _y);
	return ret;
}

bool remapBand::clickUp()
{
	bool ret=false;
	for (unsigned int i=0; i<instruments.size(); i++) {
		if(instruments[i]->clickUp()){
			ret=true;
			lastInst=i;
		}
	}
	bHolding=false;
	clearBut.clickUp();
	bar.clickUp();
	return ret;
}

bool remapBand::mouseMotion(int _x, int _y)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->mouseMotion(_x,_y);
	}
	if(bar.mouseMotion(_x,_y)){
		for (unsigned int i=0; i<instruments.size(); i++) {
			instruments[i]->update(-bar.getScrollPosition(),OF_VERT);
		}
	}
}