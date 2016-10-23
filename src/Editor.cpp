#include "Editor.h"

const unsigned int OUTPUT_FONT_SIZE = 12;

Editor::Editor() {
    replBufferSize = 0;
}

Editor::~Editor() {}

void Editor::setup() {
    // make sure to load editor font before anything else!
    //ofxEditor::loadFont("fonts/PrintChar21.ttf", 24);
    ofxEditor::loadFont("fonts/topaz/TopazPlus_a1200_v1.0.ttf", 96);
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

    //setReplBuffer(true);

    // open a file by default
    //ofFile testFile;
    //testFile.open("hi.tidal", ofFile::ReadOnly);
    //editor.setText(testFile.readToBuffer().getText());
    //ofLogNotice() << "num chars: " << editor.getNumCharacters() << " num lines: " << editor.getNumLines();

    //repl.setListener(this);
    //repl.start("data/boot.hss");

    //screpl.setListener(this);
    //screpl.start();
}

void Editor::update() {
    //repl.readAsync();
    //screpl.readAsync();
}

void Editor::draw() {
    //if (showReplBuffer) drawReplBuffer();
    editor.draw();
}

void Editor::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
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
            case 'r':
                executeSuperColliderScript();
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

void Editor::resize(int w, int h) {
    editor.resize(w, h);
}

void Editor::executeScript() {
    bool selection = editor.isSelection();
    if (selection) {
        editor.flashSelection();
        const string &s = editor.getText();
        repl.eval(s);
    } else {
        const string &s = getParagraph();
        repl.evalMulti(s);
    }
}

void Editor::executeSuperColliderScript() {
    bool selection = editor.isSelection();
    if (selection) {
        editor.flashSelection();
        const string &s = editor.getText();
        screpl.eval(s);
    } else {
        const string &s = getParagraph();
        screpl.evalMulti(s);
    }
}

string Editor::getParagraph() {
    unsigned int pos = editor.getCurrentPos();
    const string text = editor.getText();

    size_t start = text.rfind("\n\n", pos);
    if (start == string::npos) start = 0;

    size_t end = text.find("\n\n", pos);
    if (end == string::npos) end = text.size();

    size_t len = end - start;
    start = start == 0 ? start : (start + 2);

    string subs = text.substr(start, len);
    //ofLog() << "substring: '" << subs << "'";

    editor.flashText(start, end);

    return subs;
}

void Editor::setReplBuffer(bool value) {
    showReplBuffer = value;
}

bool Editor::getReplBuffer() const {
    return showReplBuffer;
}

void Editor::inputLineEvent(const string& line) {
    appendReplBuffer(line, Repl::INPUT);
}

void Editor::outputLineEvent(const string& line) {
    appendReplBuffer(line, Repl::OUTPUT);
}

void Editor::errorLineEvent(const string& line) {
    appendReplBuffer(line, Repl::ERROR);
}

void Editor::appendReplBuffer(const string& line, const Repl::EventType type) {
    if (replBufferSize > ofGetHeight() / OUTPUT_FONT_SIZE) {
        replBuffer.pop_front();
    } else {
        replBufferSize++;
    }
    replBuffer.push_back(make_pair(type, line));
}

void Editor::drawReplBuffer() {
    list< pair<Repl::EventType, string> >::const_iterator it = replBuffer.begin();
    for (int y = 0; it != replBuffer.end(); it++, y += OUTPUT_FONT_SIZE) {
        const Repl::EventType type = it->first;
        const string &line = it->second;

        switch (type) {
          case Repl::INPUT:
            ofSetColor(0, 64, 0);
            break;
          case Repl::OUTPUT:
            ofSetColor(64);
            break;
          case Repl::ERROR:
            ofSetColor(127, 0, 0);
            break;
        }

        ofDrawBitmapString(line, 0, y);
    }
}
