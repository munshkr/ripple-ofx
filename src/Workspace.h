#pragma once

#include "Editor.h"
#include "Repl.h"
#include "ReplLog.h"
#include "TidalRepl.h"
#include "SuperColliderRepl.h"

class Workspace {
    public:
        Workspace();
        virtual ~Workspace();

        void draw();
        void update();
        void keyPressed(int key);
        void resize(int w, int h);
        void quit();

        // show/hide REPL output buffer
        void setReplBuffer(bool value);
        bool getReplBuffer() const;

    private:
        Editor* createEditor(Repl& repl);

        // REPLs
        TidalRepl repl;
        SuperColliderRepl screpl;

        ReplLog* replLog;
        ReplLog* screplLog;

        // editors
        vector<Editor*> editors;
        unsigned int currentEditor;

        bool showReplBuffer;
};
