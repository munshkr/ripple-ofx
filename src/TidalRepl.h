#pragma once

#include <string>

#define BUFFER_SIZE 4096

using namespace std;

class TidalReplListener {
    public:
        // event triggered whenever Tidal reads a line from STDIN
        virtual void inputLineEvent(const string &line) = 0;

        // event triggered whenever Tidal prints a line to STDOUT
        virtual void outputLineEvent(const string &line) = 0;

        // event triggered whenever Tidal prints a line to STDERR
        virtual void errorLineEvent(const string &line) = 0;
};

class TidalRepl {
    public:
        TidalRepl();
        TidalRepl(TidalReplListener* listener);
        ~TidalRepl();

        // Start REPL process
        void start();

        // Start REPL process and run bootload script
        void start(const string& bootPath);

        // Evaluate a string and emit input event
        void eval(string s, bool print=true);

        // Read standard output and error from REPL process and emit events
        void readAsync();

        // Decides if REPL process is running
        bool isRunning() const;

        // Getter/setter of event listener
        void setListener(TidalReplListener* listener);
        TidalReplListener* getListener() const;

    protected:
        TidalReplListener* listener;

    private:
        // Initialize pipes
        void initPipes();

        // Fork process and make child execute REPL
        void forkExec();

        inline void emitInput(const string &s);
        inline void emitOutput(const string &s);
        inline void emitError(const string &s);

        // Determines
        bool running;
        int replPid;
        char buf[BUFFER_SIZE];
        int pipes[3][2];
};
