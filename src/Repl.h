#pragma once

#include <string>

using namespace std;

const int BUFFER_SIZE = 64;


class ReplListener {
    public:
        // event triggered whenever REPL reads a line from STDIN
        virtual void inputLineEvent(const string &line) = 0;

        // event triggered whenever REPL prints a line to STDOUT
        virtual void outputLineEvent(const string &line) = 0;

        // event triggered whenever REPL prints a line to STDERR
        virtual void errorLineEvent(const string &line) = 0;
};

class Repl {
    public:
        enum EventType { INPUT, OUTPUT, ERROR };

        Repl();
        virtual ~Repl();

        // Start REPL process
        virtual void start();

        // Evaluate a string and emit input event
        virtual void eval(string s);

        // Evaluate a string as multiline and emit input event
        // Override if multiline expressions need to do something different
        virtual void evalMulti(string s);

        // Read standard output and error from REPL process and emit events
        virtual void readAsync();

        // Decides if REPL process is running
        virtual bool isRunning() const;

        virtual void setListener(ReplListener* listener);
        virtual ReplListener* getListener() const;

    protected:
        virtual void execProcess() = 0;

        void emit(const string &s, const EventType type);
        void emitLine(const string &line, const EventType type);

        bool running;
        ReplListener* listener;
        int replPid;
        char buf[BUFFER_SIZE];
        int pipes[3][2];
        string lastLine;
};
