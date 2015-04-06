#include "ofApp.h"

const unsigned int MAX_REPL_LINES = 70;

void ofApp::setup() {
    replBufferSize = 0;

    ofSetVerticalSync(true);

    // handle ESC internally since we use it to exit selection
    ofSetEscapeQuitsApp(false);

    // make sure to load editor font before anything else!
    //ofxEditor::loadFont("fonts/PrintChar21.ttf", 24);
    ofxEditor::loadFont("fonts/topaz/TopazPlus_a1200_v1.0.ttf", 48);
    //ofxEditor::loadFont("fonts/C64_Pro-STYLE.ttf", 48);

    // white text with gray shadow, on black background
    ofBackground(0);
    //editor.getSettings().setTextShadowColor(ofColor::gray);

    // setup color scheme via xml file
    colorScheme.loadFile("colorScheme.xml");
    editor.setColorScheme(&colorScheme);

    // setup Tidal syntax via xml file
    syntax.loadFile("tidalSyntax.xml");
    editor.getSettings().addSyntax(&syntax);
    editor.getSettings().printSyntaxes();

    editor.setAutoFocus(true);

    // open a file by default
    //ofFile testFile;
    //testFile.open("hi.tidal", ofFile::ReadOnly);
    //editor.setText(testFile.readToBuffer().getText());
    //ofLogNotice() << "num chars: " << editor.getNumCharacters() << " num lines: " << editor.getNumLines();

    debug = false;

    repl.setListener(this);
    repl.start("data/boot.hss");
}

void ofApp::draw() {
    list< pair<EventType, string> >::const_iterator it = replBuffer.begin();
    for (int y = 0; it != replBuffer.end(); it++, y += 10) {
        const EventType type = it->first;
        const string &line = it->second;

        switch (type) {
          case OUTPUT:
            ofSetColor(64);
            break;
          case INPUT:
            ofSetColor(0, 127, 0);
            break;
          case ERROR:
            ofSetColor(127, 0, 0);
            break;
        }

        ofDrawBitmapString(line, 0, y);
    }

    editor.draw();

    if (debug) {
        ofSetColor(255);
        ofDrawBitmapString("fps: " + ofToString((int) ofGetFrameRate()), ofGetWidth() - 70, ofGetHeight() - 10);
    }

}

void ofApp::update() {
    repl.readAsync();
}

void ofApp::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
            case 'q':
                ofExit();
                return;
            case 's':
                if (editor.getColorScheme()) {
                    editor.clearColorScheme();
                } else {
                    editor.setColorScheme(&colorScheme);
                }
                return;
            case 'e':
                executeScript();
                return;
            case 'd':
                debug = !debug;
                return;
            case 'f':
                ofToggleFullscreen();
                return;
            case 'l':
                editor.setLineWrapping(!editor.getLineWrapping());
                return;
            case 'n':
                editor.setLineNumbers(!editor.getLineNumbers());
                return;
            case 'z':
                editor.setAutoFocus(!editor.getAutoFocus());
                return;
            case 'w':
                ofxEditor::setTextShadow(!ofxEditor::getTextShadow());
                return;
            case '1':
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                editor.setCurrentLinePos(1, 5);
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                break;
            case '2':
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                editor.setCurrentLinePos(5, 2);
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                break;
        }
    }
    editor.keyPressed(key);
}

void ofApp::windowResized(int w, int h) {
    editor.resize(w, h);
}

void ofApp::executeScript() {
    bool selection = editor.isSelection();
    if (selection) {
        editor.flashSelection();
        const string &s = editor.getText();
        repl.eval(s, false);
        inputLineEvent(s);
    } else {
        const string &s = getParagraph();
        repl.eval(":{\n" + s + "\n:}", false);
        inputLineEvent(s);
    }
}

string ofApp::getParagraph() {
    unsigned int pos = editor.getCurrentPos();
    const string text = editor.getText();

    size_t start = text.rfind("\n\n", pos);
    if (start == string::npos) start = 0;

    size_t end = text.find("\n\n", pos);
    if (end == string::npos) end = text.size();

    size_t len = end - start;
    start = start == 0 ? start : (start + 2);

    string subs = text.substr(start, len);
    //cerr << "substring: '" << subs << "'" << endl;

    editor.flashText(start, end);

    return subs;
}

void ofApp::inputLineEvent(const string& line) {
    appendReplBuffer(line, INPUT);
}

void ofApp::outputLineEvent(const string& line) {
    appendReplBuffer(line, OUTPUT);
}

void ofApp::errorLineEvent(const string& line) {
    appendReplBuffer(line, ERROR);
}

void ofApp::appendReplBuffer(const string& line, const EventType type) {
    if (replBufferSize > MAX_REPL_LINES) {
        replBuffer.pop_front();
    } else {
        replBufferSize++;
    }
    replBuffer.push_back(make_pair(type, line));
}
