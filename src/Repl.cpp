#include "Repl.h"

// TODO check which of these is really necessary to include...
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <ofLog.h>
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

// select() timeout (0 means no delay)
const int TV_SEC  = 0;
const int TV_USEC = 0;

const string NEWLINE = "\n";

#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
#define PARENT_ERROR_FD ( pipes[PARENT_ERROR_PIPE][READ_FD]  )

#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )
#define CHILD_ERROR_FD  ( pipes[PARENT_ERROR_PIPE][WRITE_FD] )


Repl::Repl() {
    listener = NULL;
}

Repl::~Repl() {
    if (running) {
        if (replPid) kill(replPid, SIGTERM);

        close(PARENT_READ_FD);
        close(PARENT_WRITE_FD);
        close(PARENT_ERROR_FD);
    }
}

void Repl::start() {
    // Init pipes
    for (int i = 0; i < NUM_PIPES; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }

    // Fork process
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

        execProcess();

        //ofLogError() << "Repl: Failed to execute " << argv[0];
        ofLogError() << "Repl: Failed to execute process";

    } else { // parent
        // Close fds not required by parent
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(CHILD_ERROR_FD);
    }

    running = true;
}

void Repl::eval(string s, bool print) {
    if (!running) {
        ofLogError() << "Repl: Process is not running";
        return;
    }

    string sn = s + '\n';
    int res = write(PARENT_WRITE_FD, sn.c_str(), strlen(sn.c_str()));
    if (res == -1) perror("write");

    if (print) {
        emit(sn, INPUT);
        ofLogVerbose() << "Repl: [INPUT] " << s;
    }
}

void Repl::readAsync() {
    if (!running) {
        ofLogError() << "Repl: Process is not running";
        return;
    }

    fd_set rfds;
    struct timeval tv;

    // Watch parent stdin to see when it has input written by the child
    FD_ZERO(&rfds);
    FD_SET(PARENT_READ_FD, &rfds);
    FD_SET(PARENT_ERROR_FD, &rfds);

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
                ofLogError() << "Repl: stdout IO error";
            } else {
                buf[count] = 0;
                emit(buf, OUTPUT);
                ofLogVerbose() << "Repl: [OUTPUT] " << buf;
            }
        }

        if (FD_ISSET(PARENT_ERROR_FD, &rfds)) {
            // Read from child’s stderr
            int count = read(PARENT_ERROR_FD, buf, BUFFER_SIZE);
            if (count == -1) {
                ofLogError() << "Repl: stderr IO error";
            } else {
                buf[count] = 0;
                emit(buf, ERROR);
                ofLogVerbose() << "Repl: [ERROR] " << buf;
            }
        }
    }
}

bool Repl::isRunning() const {
    return running;
}

void Repl::setListener(ReplListener* lst) {
    listener = lst;
}

ReplListener* Repl::getListener() const {
    return listener;
}

void Repl::emit(const string &str, const EventType type) {
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

void Repl::emitLine(const string& line, const EventType type) {
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
