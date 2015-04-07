#include "TidalRepl.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>

const int NUM_PIPES = 3;

const int PARENT_WRITE_PIPE = 0;
const int PARENT_READ_PIPE  = 1;
const int PARENT_ERROR_PIPE = 2;

// always in a pipe[], pipe[0] is for read and pipe[1] is for write
const int READ_FD  = 0;
const int WRITE_FD = 1;

// select() timeout
const int TV_SEC  = 0;
const int TV_USEC = 10000;

const string NEWLINE = "\n";

#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
#define PARENT_ERROR_FD ( pipes[PARENT_ERROR_PIPE][READ_FD]  )

#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )
#define CHILD_ERROR_FD  ( pipes[PARENT_ERROR_PIPE][WRITE_FD] )


TidalRepl::TidalRepl() {
    listener = NULL;
}

TidalRepl::TidalRepl(TidalReplListener* lst) {
    listener = lst;
}

TidalRepl::~TidalRepl() {
    if (running) {
        if (replPid) kill(replPid, SIGTERM);

        close(PARENT_READ_FD);
        close(PARENT_WRITE_FD);
        close(PARENT_ERROR_FD);

        cout << "\e[0m" << endl;
    }
}

void TidalRepl::eval(string s, bool print) {
    if (!running) {
        cerr << "REPL is not running" << endl;
        return;
    }

    string sn = s + '\n';
    int res = write(PARENT_WRITE_FD, sn.c_str(), strlen(sn.c_str()));
    if (res == -1) perror("write");

    if (print) {
        emit(sn, INPUT);
        cout << "\e[33m" << s << "\e[0m" << endl;
    }
}

void TidalRepl::readAsync() {
    if (!running) {
        cerr << "REPL is not running" << endl;
        return;
    }

    fd_set rfds;
    struct timeval tv;

    // Watch parent stdin to see when it has input written by the child
    FD_ZERO(&rfds);
    FD_SET(PARENT_READ_FD, &rfds);
    FD_SET(PARENT_ERROR_FD, &rfds);

    // Timeout: 10000us, review if 0 is possible and doesn't hog the cpu
    tv.tv_sec = TV_SEC;
    tv.tv_usec = TV_USEC;

    int ret = select(PARENT_ERROR_FD+1, &rfds, NULL, NULL, &tv);
    // Don't rely on the value of tv now!

    if (ret == -1) {
        perror("select()");
    } else if (ret) {
        if (FD_ISSET(PARENT_READ_FD, &rfds)) {
            // Read from child’s stdout
            int count = read(PARENT_READ_FD, buf, BUFFER_SIZE);
            if (count == -1) {
                cerr << "stdout IO error" << endl;
            } else {
                buf[count] = 0;
                emit(buf, OUTPUT);
                cout << "\e[32m" << buf << "\e[0m";
            }
        }

        if (FD_ISSET(PARENT_ERROR_FD, &rfds)) {
            // Read from child’s stderr
            int count = read(PARENT_ERROR_FD, buf, BUFFER_SIZE);
            if (count == -1) {
                cerr << "stderr IO error" << endl;
            } else {
                buf[count] = 0;
                emit(buf, ERROR);
                cout << "\e[31m" << buf << "\e[0m";
            }
        }
    }
}

void TidalRepl::start() {
    initPipes();
    forkExec();
    running = true;
}

void TidalRepl::start(const string& bootPath) {
    start();

    ifstream f(bootPath.c_str());

    if (f.is_open()) {
        stringstream f_buf;
        f_buf << f.rdbuf();
        eval(f_buf.str(), false);
    } else {
        cerr << "Unable to open bootstrap file at " << bootPath << endl;
    }
}

bool TidalRepl::isRunning() const {
    return running;
}

void TidalRepl::setListener(TidalReplListener* lst) {
    listener = lst;
}

TidalReplListener* TidalRepl::getListener() const {
    return listener;
}

void TidalRepl::initPipes() {
    for (int i = 0; i < NUM_PIPES; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }
}

void TidalRepl::forkExec() {
    replPid = fork();

    if (replPid == 0) { // child
        dup2(CHILD_READ_FD,  STDIN_FILENO);
        dup2(CHILD_WRITE_FD, STDOUT_FILENO);
        dup2(CHILD_ERROR_FD, STDERR_FILENO);

        // Close fds not required by child. Also, we don't want the exec'ed
        // program to know these existed.
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(CHILD_ERROR_FD);
        close(PARENT_READ_FD);
        close(PARENT_WRITE_FD);
        close(PARENT_ERROR_FD);

        char* argv[] = { "/usr/bin/ghci", "-XOverloadedStrings", 0 };
        execv(argv[0], argv);
    } else { // parent
        // Close fds not required by parent
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(CHILD_ERROR_FD);
    }
}

void TidalRepl::emit(const string &str, const EventType type) {
    if (!listener) return;

    string::size_type pos, lastPos = 0;

    const string s = lastLine + str;
    lastLine.erase();

    while (true) {
        pos = s.find_first_of(NEWLINE, lastPos);

        if (pos == string::npos) {
            pos = s.length();
            if (pos != lastPos) lastLine = s.substr(lastPos, pos - lastPos);
            break;
        } else {
            if (pos != lastPos) {
                const string line = s.substr(lastPos, pos - lastPos);
                emitLine(line, type);
            }
        }

        lastPos = pos + 1;
    }
}

void TidalRepl::emitLine(const string& line, const EventType type) {
    if (!listener) return;

    switch (type) {
      case INPUT:
        listener->inputLineEvent(line);
        break;
      case OUTPUT:
        listener->outputLineEvent(line);
        break;
      case ERROR:
        listener->errorLineEvent(line);
        break;
    }
}
