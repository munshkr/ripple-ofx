#pragma once

#include "ofxEditor.h"
#include "Repl.h"

class Editor : public ofxEditor {
    public:
        Editor();
        virtual ~Editor();

        void draw();
        void keyPressed(int key);

        // REPL evaluation
        string getParagraph();
        void executeScript();

        Repl* getRepl();
        void setRepl(Repl*);

        void setViewportX(float x);
        float getViewportX();

        void setViewportY(float y);
        float getViewportY();

    private:
        ofxEditor editor;
        ofxEditorColorScheme colorScheme;
        ofxEditorSyntax syntax;

        Repl* repl;

        int viewportX;
        int viewportY;
};
