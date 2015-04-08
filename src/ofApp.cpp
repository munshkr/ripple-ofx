#include "ofApp.h"

const unsigned int OUTPUT_FONT_SIZE = 12;

void ofApp::setup() {
    replBufferSize = 0;
    debug = false;

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

    setReplBuffer(true);

    // open a file by default
    //ofFile testFile;
    //testFile.open("hi.tidal", ofFile::ReadOnly);
    //editor.setText(testFile.readToBuffer().getText());
    //ofLogNotice() << "num chars: " << editor.getNumCharacters() << " num lines: " << editor.getNumLines();

    repl.setListener(this);
    repl.start("data/boot.hss");
}

void ofApp::draw() {
    if (showReplBuffer) drawReplBuffer();

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
            case 'o':
                setReplBuffer(!getReplBuffer());
                return;
        }
    }
    editor.keyPressed(key);
}

void ofApp::windowResized(int w, int h) {
    editor.resize(w, h);
}

void ofApp::setReplBuffer(bool value) {
    showReplBuffer = value;
}

bool ofApp::getReplBuffer() const {
    return showReplBuffer;
}

void ofApp::executeScript() {
    bool selection = editor.isSelection();
    if (selection) {
        editor.flashSelection();
        const string &s = editor.getText();
        repl.eval(s);
    } else {
        const string &s = getParagraph();
        repl.eval(":{\n" + s + "\n:}");
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
    appendReplBuffer(line, TidalRepl::INPUT);
}

void ofApp::outputLineEvent(const string& line) {
    appendReplBuffer(line, TidalRepl::OUTPUT);
}

void ofApp::errorLineEvent(const string& line) {
    appendReplBuffer(line, TidalRepl::ERROR);
}

void ofApp::appendReplBuffer(const string& line, const TidalRepl::EventType type) {
    if (replBufferSize > ofGetHeight() / OUTPUT_FONT_SIZE) {
        replBuffer.pop_front();
    } else {
        replBufferSize++;
    }
    replBuffer.push_back(make_pair(type, line));
}

void ofApp::drawReplBuffer() {
    list< pair<TidalRepl::EventType, string> >::const_iterator it = replBuffer.begin();
    for (int y = 0; it != replBuffer.end(); it++, y += OUTPUT_FONT_SIZE) {
        const TidalRepl::EventType type = it->first;
        const string &line = it->second;

        switch (type) {
          case TidalRepl::INPUT:
            ofSetColor(0, 64, 0);
            break;
          case TidalRepl::OUTPUT:
            ofSetColor(64);
            break;
          case TidalRepl::ERROR:
            ofSetColor(127, 0, 0);
            break;
        }

        ofDrawBitmapString(line, 0, y);
    }
}
