//
// Created by pepsipu on 11/3/19.
//

#ifndef OTUS_ENGINE_NOTIFY_H
#define OTUS_ENGINE_NOTIFY_H

#include <unistd.h>
#include <string>

void play_gain_points() {
    if (!fork()) {
        char *args[] = {"aplay", "./points.wav", nullptr};
        execvp(args[0], args);
    }
}

void notify_points(char *message) {
    if (!fork()) {
        char *args[] = {"notify-send", "Points scored!", message, nullptr};
        execvp(args[0], args);
    }
}

void score(char *message) {
    play_gain_points();
    notify_points(message);
}

#endif //OTUS_ENGINE_NOTIFY_H
