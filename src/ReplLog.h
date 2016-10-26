#pragma once

// FIXME replace with #include <list>, etc.
#include "ofxEditor.h"
#include "Repl.h"

class ReplLog : public ReplListener {
    public:
        ReplLog(Repl&);
        virtual ~ReplLog();
        void draw();

        // REPL events
        void inputLineEvent(const string& line);
        void outputLineEvent(const string& line);
        void errorLineEvent(const string& line);

    private:
        void appendReplBuffer(const string& line, const Repl::EventType type);

        Repl* repl;

        list< pair<Repl::EventType, string> > buffer;
        unsigned int bufferSize;    // to avoid calling buffer.size(),
                                    // which is O(n) on some C++ implementations.
};
