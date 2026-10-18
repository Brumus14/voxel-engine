#ifndef STOPWATCH_H
#define STOPWATCH_H

struct stopwatch {
    double start_time;
    double end_time;
};

void stopwatch_start(struct stopwatch *stopwatch);
void stopwatch_end(struct stopwatch *stopwatch);
double stopwatch_elapsed(struct stopwatch *stopwatch);
double stopwatch_time(struct stopwatch *stopwatch);

#endif
