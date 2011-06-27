#include "testApp.h"

#define RECONNECT_TIME 400
#define MSG_START "MSGSTRT"
#define MSG_END   "MSGEND"
#define TCP_HOST_IP "127.0.0.1"

//--------------------------------------------------------------
void testApp::setup(){
	// Network Setup
	
	ofSetVerticalSync(true);
	
	weConnected = tcpClient.setup(TCP_HOST_IP, 11999);
	
	connectTime = 0;
	deltaTime = 0;
	
	tcpClient.setVerbose(true);
	
	//MIDI Setup
	
	int numProgs=loadProgramNames("midiPrograms.ini");
	report.loadFont("fonts/DinC.ttf");
	background.loadImage("images/background.jpg");
	kb.setup(ofGetWidth()-100., 4);
	for(int i=0; i<numProgs; i++)
		test.setValue(getProgramNames()[i]);
	testBut.setTitle("reset");
	testBut.setTextSize(24);
	
	rmp.setup();
	//rmp.registerEvents();
	
	keyboard.openPort("KeyRig 49");
	rolandSynth.openPort("decoder");

}

//--------------------------------------------------------------
void testApp::update(){
	rmp.update(75);
	
	// Network Updates
	
	//we are connected - lets send our text and check what we get back
	if (weConnected) {
		//if data has been sent lets update our text
		string str = tcpClient.receive();
		if( str.length() > 0 ){
			msgRx = str;
		}
	}
	/*if(!weConnected){
		//if we are not connected lets try and reconnect every 5 seconds
		deltaTime = ofGetElapsedTimeMillis() - connectTime;
		
		if( deltaTime > 5000 ){
			weConnected = tcpClient.setup(TCP_HOST_IP, 11999);
			connectTime = ofGetElapsedTimeMillis();
		}
		
	}*/
}

//--------------------------------------------------------------
void testApp::draw(){
  //ofPushMatrix();
  //ofScale(ofGetWidth()/1920., ofGetHeight()/1200., 0);
	ofSetColor(128, 128, 128);
	//background.draw(0, 0,ofGetWidth(),ofGetHeight());
	
	ofBackground(0x33, 0x33, 0x33);
	
	ofSetColor(0x444400);
	for (int i=0; i*10<ofGetHeight(); i++) {
		ofRect(0, i*10, ofGetWidth(), 1);
	}
	for (int i=0; i*10<ofGetWidth(); i++) {
		ofRect(i*10, 0, 1, ofGetHeight());
	}

	/*kb.draw(ofGetWidth()/32, ofGetHeight()-(kb.h+ofGetWidth()/32));
	
	testBut.draw(kb.x+kb.w+40, kb.y+kb.h/2);
	
	test.draw(400,300);*/
	
	rmp.draw(0, 75);
	ofSetColor(0x333333);
	ofRoundBox(-30, 0, ofGetWidth()+60, 75, 15, .2);
	ofShade(0, 75, 15, ofGetWidth(), OF_DOWN, .5);
	ofSetColor(0xFFFFFF);
	report.setMode(OF_FONT_CENTER);
	report.setSize(50);
	report.drawString("DRAG INSTRUMENTS TO THE KEYS OF THE KEYBOARD", ofGetWidth()/2, 75-20);
  //ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	rmp.motion(x, y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	rmp.drag(x, y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	rmp.clickDown(x, y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	rmp.clickUp(x, y);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	rmp.resize(w, h);
}

void testApp::midiToSend(vector< unsigned char > message){
	rolandSynth.sendMessage(message);
	/*for (unsigned int i=0; i<message.size(); i++) {
		cout << int(message[i]) << " ";
	}
	
	cout <<" end of MIDI Message" << endl;*/
	
	msgTx=MSG_START;
	
	for (unsigned int i=0; i<message.size(); i++) {
		msgTx+=".";
		msgTx+=ofToString(message[i]);
	}
	
	msgTx+=".";
	msgTx+=MSG_END;
	
	cout << msgTx<<endl;
	
	if(weConnected){
		if(tcpClient.send(msgTx)){
			
		}else if(!tcpClient.isConnected()){
			weConnected = false;
		}
	}
}

//


void testApp::midiReceived(double deltatime, std::vector< unsigned char > *message, int port){
	if(message->size()>=3){
		if (message->at(0)==MIDI_NOTE_ON&&port==keyboard.getPort()) {
			if(message->at(1)>=MIDI_KEYBOARD_START&&message->at(1)<MIDI_KEYBOARD_END){
				rmp.kb[message->at(1)-MIDI_KEYBOARD_START].setPressed(message->at(2));
				if(!rmp.kb.getButtonChoice(message->at(1)-MIDI_KEYBOARD_START)){
					vector<unsigned char> msg;
					msg.push_back(MIDI_NOTE_ON+rmp.kb.getChannel());
					msg.push_back(message->at(1));
					msg.push_back(message->at(2));
					midiToSend(msg);
				}
				else {
					vector<instrument> & tmp=rmp.kb.getNotes(message->at(1)-MIDI_KEYBOARD_START);
					for (unsigned int i=0; i<tmp.size(); i++) {
						if(!tmp[i].isDefault()){
							tmp[i].play(message->at(2));
						}
					}
				}
			}
		}
	}
}