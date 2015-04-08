#pragma once

#include "ofMain.h"
#include "ofxEditor.h"
#include "TidalRepl.h"

class ofApp : public ofBaseApp, TidalReplListener {
    public:
        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void windowResized(int w, int h);
        void setReplBuffer(bool value);
        bool getReplBuffer() const;

        ofxEditor editor;
        ofxEditorColorScheme colorScheme;
        ofxEditorSyntax syntax;
        bool debug; //< show grid and fps?

        void inputLineEvent(const string& line);
        void outputLineEvent(const string& line);
        void errorLineEvent(const string& line);

    private:
        void setTidalSyntax(ofxEditorColorScheme &scheme);
        string getParagraph();
        void executeScript();
        void appendReplBuffer(const string& line, const TidalRepl::EventType type);
        void drawReplBuffer();

        TidalRepl repl;

        list< pair<TidalRepl::EventType, string> > replBuffer;
        unsigned int replBufferSize;
        bool showReplBuffer;
};
