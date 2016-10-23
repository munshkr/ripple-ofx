#pragma once

#include "Editor.h"
#include "Repl.h"
#include "TidalRepl.h"
#include "SuperColliderRepl.h"

class Workspace : public ReplListener {
    public:
        Workspace();
        virtual ~Workspace();

        void draw();
        void update();
        void keyPressed(int key);
        void resize(int w, int h);

        // show/hide REPL output buffer
        void setReplBuffer(bool value);
        bool getReplBuffer() const;

        // REPL events
        void inputLineEvent(const string& line);
        void outputLineEvent(const string& line);
        void errorLineEvent(const string& line);

    private:
        void appendReplBuffer(const string& line, const Repl::EventType type);
        void drawReplBuffer();

        // REPLs
        TidalRepl repl;
        SuperColliderRepl screpl;

        // editors
        vector<Editor*> editors;
        unsigned int currentEditor;

        // REPL-related vars
        list< pair<Repl::EventType, string> > replBuffer;
        bool showReplBuffer;
        unsigned int replBufferSize;    // to avoid calling replBuffer.size(),
                                        // which is O(n) on some C++ implementations.
};
