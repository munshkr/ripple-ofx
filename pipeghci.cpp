#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>

#define NUM_PIPES 3

#define PARENT_WRITE_PIPE   0
#define PARENT_READ_PIPE    1
#define PARENT_ERROR_PIPE   2

int pipes[NUM_PIPES][2];

// always in a pipe[], pipe[0] is for read and pipe[1] is for write
#define READ_FD  0
#define WRITE_FD 1

#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
#define PARENT_ERROR_FD ( pipes[PARENT_ERROR_PIPE][READ_FD]  )

#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )
#define CHILD_ERROR_FD  ( pipes[PARENT_ERROR_PIPE][WRITE_FD] )

#define BUFFER_SIZE 4096

using namespace std;

int child_pid = 0;

static void read_async(char* buf) {
    int count;
    fd_set rfds;
    struct timeval tv;

    // Watch parent stdin to see when it has input written by the child
    FD_ZERO(&rfds);
    FD_SET(PARENT_READ_FD, &rfds);
    FD_SET(PARENT_ERROR_FD, &rfds);

    // Timeout: 10000us, review if 0 is possible and doesn't hog the cpu
    tv.tv_sec = 0;
    tv.tv_usec = 10000;

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
                cout << "\e[31m" << buf << "\e[0m";
            }
        }
    } else {
        cerr << '.' << flush;
    }
}

static void eval(const char* s, bool print=true) {
    // Write to child’s stdin
    write(PARENT_WRITE_FD, s, strlen(s));

    if (print) cout << "\e[33m" << s << "\e[0m";
}

static void int_handler(int sig) {
    if (child_pid) kill(child_pid, SIGINT);
    close(PARENT_READ_FD);
    close(PARENT_WRITE_FD);
    close(PARENT_ERROR_FD);
    cout << "\e[0m" << endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, int_handler);

    char buffer[BUFFER_SIZE];

    for (int i = 0; i < NUM_PIPES; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return 1;
        } }

    child_pid = fork();
    if (child_pid == 0) {
        // Child
        char * argv[] = { (char*) "/usr/bin/ghci", (char*) "-XOverloadedStrings", 0 };

        dup2(CHILD_READ_FD, STDIN_FILENO);
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

        execv(argv[0], argv);
    } else {
        // Parent

        // close fds not required by parent
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(CHILD_ERROR_FD);

        cout << "Loading Tidal..." << endl;
        eval(":set prompt \"\"\n" \
             ":module Sound.Tidal.Context\n" \
             "let newStream = stream \"127.0.0.1\" 7771 dirt\n" \
             "d1 <- newStream\n" \
             "(cps, getNow) <- bpsUtils\n" \
             "let bps x = cps (x/2)\n", false);

        sleep(2);
        read_async(buffer);

        cout << "Done!" << endl;

        eval("d1 silence\n");
        sleep(1);
        read_async(buffer);

        eval("d1 $ sound \"bd\"\n");
        sleep(1);
        read_async(buffer);

        // intentional typo to see stderr printed on red
        eval("d1 $ sund \"bd sn\"\n");
        sleep(1);
        read_async(buffer);

        eval("d1 $ sound \"[bd sn, glitch:1*4]\"\n");

        for (;;) {
            read_async(buffer);
        }
    }

    return 0;
}
