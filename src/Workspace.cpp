#include "Workspace.h"

#define SPLIT_SCREEN

Workspace::Workspace() {
    // create log viewers for each REPL
    replLog = new ReplLog(repl);
    screplLog = new ReplLog(screpl);

    createEditor(repl);
    createEditor(screpl);

#ifdef SPLIT_SCREEN
    // 3rd editor for split screen
    Editor* ed = createEditor();
    subThread.setEditor(ed);
    subThread.startThread(true);
#endif

    repl.start("data/tidalStartup.hss");
    screpl.start("data/scStartup.scd");

    currentEditor = 0;
    showReplBuffer = true;

    // force resize to position editors
    int w = (ofGetWindowMode() == OF_WINDOW) ? ofGetViewportWidth() : ofGetScreenWidth();
    int h = (ofGetWindowMode() == OF_WINDOW) ? ofGetViewportHeight() : ofGetScreenHeight();
    resize(w, h);
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

#ifdef SPLIT_SCREEN
    // Render both editors
    editors[currentEditor]->draw();
    editors[2]->draw();
#else
    editors[currentEditor]->draw();
#endif
}

void Workspace::update() {
    for (auto it = editors.begin(); it < editors.end(); it++) {
        Repl* repl = (*it)->getRepl();
        if (repl) repl->readAsync();
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
#ifdef SPLIT_SCREEN
    editors[0]->setViewportX(0);
    editors[0]->setViewportY(h / 2);
    editors[1]->setViewportX(0);
    editors[1]->setViewportY(h / 2);

    editors[2]->setViewportX(w / 2);
    editors[2]->setViewportY(h / 2);
#endif

    for (auto it = editors.begin(); it < editors.end(); it++) {
        (*it)->resize(w, h);
    }
}

void Workspace::quit() {
    this->screpl.eval("Server.killAll()");
#ifdef SPLIT_SCREEN
    subThread.stopThread();
#endif
}

void Workspace::setReplBuffer(bool value) {
    showReplBuffer = value;
}

bool Workspace::getReplBuffer() const {
    return showReplBuffer;
}

Editor* Workspace::createEditor(Repl& repl) {
    Editor* e = new Editor();
    editors.push_back(e);
    e->setRepl(&repl);
    return e;
}

Editor* Workspace::createEditor() {
    Editor* e = new Editor();
    editors.push_back(e);
    return e;
}
