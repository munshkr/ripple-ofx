#include "Workspace.h"

const unsigned int OUTPUT_FONT_SIZE = 12;

Workspace::Workspace() {
    Editor* ed;

    repl.setListener(this);
    screpl.setListener(this);

    // create Tidal editor
    ed = new Editor(&repl);
    editors.push_back(ed);
    ed->setup();
    repl.start("data/boot.hss");

    // create SC editor
    ed = new Editor(&screpl);
    editors.push_back(ed);
    ed->setup();
    screpl.start();

    currentEditor = 0;
    showReplBuffer = true;
    replBufferSize = 0;
}

Workspace::~Workspace() {
    editors.clear();
}

void Workspace::draw() {
    if (showReplBuffer) drawReplBuffer();
    editors[currentEditor]->draw();
}

void Workspace::update() {
    repl.readAsync();
    screpl.readAsync();
    editors[currentEditor]->update();
}

void Workspace::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
            case '1':
                currentEditor = 0;
                return;
            case '2':
                currentEditor = 1;
                return;
            case 'o':
                setReplBuffer(!getReplBuffer());
                return;
        }
    }

    editors[currentEditor]->keyPressed(key);
}

void Workspace::resize(int w, int h) {
    editors[currentEditor]->resize(w, h);
}

void Workspace::setReplBuffer(bool value) {
    showReplBuffer = value;
}

bool Workspace::getReplBuffer() const {
    return showReplBuffer;
}

void Workspace::inputLineEvent(const string& line) {
    appendReplBuffer(line, Repl::INPUT);
}

void Workspace::outputLineEvent(const string& line) {
    appendReplBuffer(line, Repl::OUTPUT);
}

void Workspace::errorLineEvent(const string& line) {
    appendReplBuffer(line, Repl::ERROR);
}

void Workspace::appendReplBuffer(const string& line, const Repl::EventType type) {
    if (replBufferSize > ofGetHeight() / OUTPUT_FONT_SIZE) {
        replBuffer.pop_front();
    } else {
        replBufferSize++;
    }
    replBuffer.push_back(make_pair(type, line));
}

void Workspace::drawReplBuffer() {
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
