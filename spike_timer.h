//Author: Mark Gottscho <mgottscho@ucla.edu>

#ifndef SPIKE_TIMER_H
#define SPIKE_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

extern int inject_count;
extern unsigned long sdecc_count;
extern unsigned long total_elapsed;
extern double avg_elapsed;
extern unsigned long sdecc_starttick;
extern unsigned long sdecc_stoptick;

void starttick();
void stoptick();
void printtick();

#ifdef __cplusplus
}
#endif

#endif
