//Author: Mark Gottscho <mgottscho@ucla.edu>

#include <stdio.h>
#include "memory_due.h"
#include "spike_timer.h"

int inject_count = 0; 
unsigned long sdecc_count = 0; 
unsigned long total_elapsed = 0; 
double avg_elapsed = 0; 
unsigned long sdecc_starttick = 0;
unsigned long sdecc_stoptick = 0;
    
void starttick() { 
    sdecc_starttick = get_sim_tick_counter();
}

void stoptick() {
    sdecc_stoptick = get_sim_tick_counter();
    sdecc_count++;
    total_elapsed += (sdecc_stoptick - sdecc_starttick);
    avg_elapsed = (double)(total_elapsed) / (double)(sdecc_count);
}

void printtick() {
    printf("start: %lu\n", sdecc_starttick);
    printf("stop: %lu\n", sdecc_stoptick);
    printf("total_elapsed = %lu\n", total_elapsed);
    printf("avg_elapsed = %f\n", avg_elapsed);
    printf("Prct elapsed: %f\n", (double)(total_elapsed)/(double)(get_sim_tick_counter())*100);
}
