#pragma once

#include "ofxEditor.h"
#include "TidalRepl.h"

class Editor : public TidalReplListener {
    public:
        Editor();
        virtual ~Editor();

        void setup();

        void update();

        /// draw the editor over the current viewport
        void draw();

        void keyPressed(int key);

        /// reshape the drawing area
        /// call this if you change the window size (fullscreen, etc)
        void resize(int width, int height);

        // REPL evaluation
        string getParagraph();
        void executeScript();

        // show/hide REPL output buffer
        void setReplBuffer(bool value);
        bool getReplBuffer() const;

        // TidalRepl events
        void inputLineEvent(const string& line);
        void outputLineEvent(const string& line);
        void errorLineEvent(const string& line);

    private:
        void appendReplBuffer(const string& line, const TidalRepl::EventType type);
        void drawReplBuffer();

        ofxEditor editor;
        ofxEditorColorScheme colorScheme;
        ofxEditorSyntax syntax;

        TidalRepl repl;

        list< pair<TidalRepl::EventType, string> > replBuffer;
        bool showReplBuffer;
        unsigned int replBufferSize;    // to avoid calling replBuffer.size(),
                                        // which is O(n) on some C++ implementations.
};
