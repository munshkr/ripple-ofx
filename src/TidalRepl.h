#pragma once

#define BUFFER_SIZE 4096

#include <string>

using namespace std;

class TidalRepl {
    public:
        TidalRepl();
        ~TidalRepl();

        void eval(string s);
        void read_async();

    private:
        int repl_pid;
        char buf[BUFFER_SIZE];
        int pipes[3][2];
};
