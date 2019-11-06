/*
Time taken by program is : 0 seconds and 35900 nanoseconds of CLOCK_PROCESS_CPUTIME_ID, with a resolution of 0 seconds and 1 nanoseconds
Time taken by program is : 1 seconds and 85081 nanoseconds of CLOCK_REALTIME, with a resolution of 0 seconds and 1 nanoseconds
Time taken by program is : 1 seconds and 85114 nanoseconds of CLOCK_MONOTONIC, with a resolution of 0 seconds and 1 nanoseconds
Time taken by program is : 1000084680 nanoseconds of std::chrono::time_point<std::chrono::high_resolution_clock>
*/
#include <bits/stdc++.h>
#include <sys/time.h>
#include <unistd.h> // sleep
#include <chrono>
#include <thread>
using namespace std;
using namespace std::chrono_literals;

/* int clock_gettime( clockid_t clock_id, struct
timespec *tp ); The clock_gettime() function gets
the current time of the clock specified by clock_id,
and puts it into the buffer pointed to by tp.tp
parameter points to a structure containing
atleast the following members:
struct timespec {
        time_t tv_sec;	 // seconds
        long	 tv_nsec;	 // nanoseconds
    };
clock id = CLOCK_REALTIME, CLOCK_PROCESS_CPUTIME_ID,
        CLOCK_MONOTONIC ...etc
CLOCK_REALTIME : clock that measures real (i.e., wall-clock) time.
CLOCK_PROCESS_CPUTIME_ID : High-resolution per-process timer
                        from the CPU.
CLOCK_MONOTONIC : High resolution timer that is unaffected
                by system date changes (e.g. NTP daemons). */

struct timespec
        CLOCK_PROCESS_CPUTIME_ID_start, CLOCK_PROCESS_CPUTIME_ID_end, CLOCK_PROCESS_CPUTIME_ID_resolution,
        CLOCK_REALTIME_start, CLOCK_REALTIME_end, CLOCK_REALTIME_resolution,
        CLOCK_MONOTONIC_start, CLOCK_MONOTONIC_end, CLOCK_MONOTONIC_resolution;

std::chrono::time_point<std::chrono::high_resolution_clock> CHRONO_start, CHRONO_end;

int main()
{

    clock_getres(CLOCK_PROCESS_CPUTIME_ID, &CLOCK_PROCESS_CPUTIME_ID_resolution);
    clock_getres(CLOCK_REALTIME, &CLOCK_REALTIME_resolution);
    clock_getres(CLOCK_MONOTONIC, &CLOCK_MONOTONIC_resolution);

    // start timer.
    clock_gettime(CLOCK_REALTIME, &CLOCK_REALTIME_start);
    clock_gettime(CLOCK_MONOTONIC, &CLOCK_MONOTONIC_start);
    CHRONO_start = chrono::high_resolution_clock::now();

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &CLOCK_PROCESS_CPUTIME_ID_start);
    sleep(1);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &CLOCK_PROCESS_CPUTIME_ID_end);

    // stop timer.
    clock_gettime(CLOCK_REALTIME, &CLOCK_REALTIME_end);
    clock_gettime(CLOCK_MONOTONIC, &CLOCK_MONOTONIC_end);
    CHRONO_end = chrono::high_resolution_clock::now();

    // Calculating total time taken by the program.
    long int
            CLOCK_PROCESS_CPUTIME_ID_time_taken_sec,
            CLOCK_REALTIME_time_taken_sec,
            CLOCK_MONOTONIC_time_taken_sec,
            CLOCK_PROCESS_CPUTIME_ID_time_taken_nano,
            CLOCK_REALTIME_time_taken_nano,
            CLOCK_MONOTONIC_time_taken_nano,
            CHRONO_time_taken_nano;
    CLOCK_PROCESS_CPUTIME_ID_time_taken_sec = (CLOCK_PROCESS_CPUTIME_ID_end.tv_sec - CLOCK_PROCESS_CPUTIME_ID_start.tv_sec);
    CLOCK_PROCESS_CPUTIME_ID_time_taken_nano = (CLOCK_PROCESS_CPUTIME_ID_end.tv_nsec - CLOCK_PROCESS_CPUTIME_ID_start.tv_nsec);
    CLOCK_REALTIME_time_taken_sec = (CLOCK_REALTIME_end.tv_sec - CLOCK_REALTIME_start.tv_sec);
    CLOCK_REALTIME_time_taken_nano = (CLOCK_REALTIME_end.tv_nsec - CLOCK_REALTIME_start.tv_nsec);
    CLOCK_MONOTONIC_time_taken_sec = (CLOCK_MONOTONIC_end.tv_sec - CLOCK_MONOTONIC_start.tv_sec);
    CLOCK_MONOTONIC_time_taken_nano = (CLOCK_MONOTONIC_end.tv_nsec - CLOCK_MONOTONIC_start.tv_nsec);
    CHRONO_time_taken_nano = chrono::duration_cast<chrono::nanoseconds>(CHRONO_end - CHRONO_start).count();

    cout << "Time taken by program is : "
             << CLOCK_PROCESS_CPUTIME_ID_time_taken_sec << " seconds and "
             << CLOCK_PROCESS_CPUTIME_ID_time_taken_nano << " nanoseconds"
             << " of CLOCK_PROCESS_CPUTIME_ID, "
             << "with a resolution of "
             << CLOCK_PROCESS_CPUTIME_ID_resolution.tv_sec << " seconds and "
             << CLOCK_PROCESS_CPUTIME_ID_resolution.tv_nsec << " nanoseconds"
             << endl;
    cout << "Time taken by program is : "
             << CLOCK_REALTIME_time_taken_sec << " seconds and "
             << CLOCK_REALTIME_time_taken_nano << " nanoseconds"
             << " of CLOCK_REALTIME, "
             << "with a resolution of "
             << CLOCK_REALTIME_resolution.tv_sec << " seconds and "
             << CLOCK_REALTIME_resolution.tv_nsec << " nanoseconds"
             << endl;
    cout << "Time taken by program is : "
             << CLOCK_MONOTONIC_time_taken_sec << " seconds and "
             << CLOCK_MONOTONIC_time_taken_nano << " nanoseconds"
             << " of CLOCK_MONOTONIC, "
             << "with a resolution of "
             << CLOCK_MONOTONIC_resolution.tv_sec << " seconds and "
             << CLOCK_MONOTONIC_resolution.tv_nsec << " nanoseconds"
             << endl;
    cout << "Time taken by program is : "
             << CHRONO_time_taken_nano << " nanoseconds"
             << " of std::chrono::time_point<std::chrono::high_resolution_clock>"
             << endl;
    return 0;
}
