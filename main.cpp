#include <stdio.h>
#include <signal.h>
#include "TicTacToe.h"

TicTacToe tic;

static void handler (int sig, siginfo_t *siginfo, void *context)
{
    printf("Signal number: %d\n", sig);
    tic.stop();
}

int main() {
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &handler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGHUP, &act, NULL) < 0 ||
        sigaction(SIGINT, &act, NULL) < 0 ||
        sigaction(SIGQUIT, &act, NULL) < 0 ||
        sigaction(SIGTERM, &act, NULL) < 0) {
        perror ("sigaction");
        return 1;
    }
    tic.addPort(5555);
    tic.start();
    return 0;
}


