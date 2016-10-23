#include "ofApp.h"

void ofApp::setup() {
    debug = false;

    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    // handle ESC internally since we use it to exit selection
    ofSetEscapeQuitsApp(false);
}

void ofApp::draw() {
    workspace.draw();

    if (debug) {
        ofSetColor(255);
        ofDrawBitmapString("fps: " + ofToString((int) ofGetFrameRate()), ofGetWidth() - 70, ofGetHeight() - 10);
    }
}

void ofApp::update() {
    workspace.update();
}

void ofApp::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
            case 'q':
                ofExit();
                return;
            case 'd':
                debug = !debug;
                return;
            case 'f':
                ofToggleFullscreen();
                return;
        }
    }

    workspace.keyPressed(key);
}

void ofApp::windowResized(int w, int h) {
    workspace.resize(w, h);
}
