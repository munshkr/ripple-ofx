#include "Workspace.h"

Workspace::Workspace() {
    Editor* ed;

    // create log viewers for each REPL
    replLog = new ReplLog(&repl);
    screplLog = new ReplLog(&screpl);

    // create Tidal editor
    ed = new Editor();
    editors.push_back(ed);
    ed->setRepl(&repl);
    repl.start("data/tidalStartup.hss");

    ofLog() << "Tidal editor X = " << ed->getViewportX();

    // create SC editor
    ed = new Editor();
    editors.push_back(ed);
    ed->setRepl(&screpl);
    screpl.start("data/scStartup.scd");

    // Split screen: set viewport to half screen width
    //int w = (ofGetWindowMode() == OF_WINDOW) ? ofGetViewportWidth() : ofGetScreenWidth();
    //ed->setViewportX(w/2);

    ofLog() << "SC editor X = " << ed->getViewportX();

    currentEditor = 0;
    showReplBuffer = true;
}

Workspace::~Workspace() {
    editors.clear();

    delete replLog;
    delete screplLog;
}

void Workspace::draw() {
    if (showReplBuffer) {
        if (editors[currentEditor]->getRepl() == &repl) {
            replLog->draw();
        } else {
            screplLog->draw();
        }
    }

    editors[currentEditor]->draw();

    // Split screen: render both editors
    //editors[0]->draw();
    //editors[1]->draw();
}

void Workspace::update() {
    for (auto it = editors.begin(); it < editors.end(); it++) {
        (*it)->getRepl()->readAsync();
    }
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
    for (auto it = editors.begin(); it < editors.end(); it++) {
        (*it)->resize(w, h);
    }
}

void Workspace::quit() {
    this->screpl.eval("Server.killAll()");
}

void Workspace::setReplBuffer(bool value) {
    showReplBuffer = value;
}

bool Workspace::getReplBuffer() const {
    return showReplBuffer;
}
