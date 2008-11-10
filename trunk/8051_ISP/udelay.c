#include <stdlib.h>
#include <stdio.h>


#ifdef USE_OLD_UDELAY

#include <time.h>

#else

#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#ifdef HAVE_ASM_MSR_H
#include <asm/msr.h>
#endif

#endif


#include "udelay.h"

#ifdef USE_OLD_UDELAY

unsigned long loops_per_usec;

void ndelay(const unsigned long nsec)
{
    unsigned long loop = (nsec * loops_per_usec + 999) / 1000;

  __asm__(".align 16\n" "1:\tdecl %0\n" "\tjne 1b":	/* no result */
  :"a"(loop));
}

/* adopted from /usr/src/linux/init/main.c */

void udelay_calibrate(void)
{
    clock_t tick;
    unsigned long bit;

    loops_per_usec = 1;
    while (loops_per_usec <<= 1) {
	tick = clock();
	while (clock() == tick);
	tick = clock();
	ndelay(1000000000 / CLOCKS_PER_SEC);
	if (clock() > tick)
	    break;
    }

    loops_per_usec >>= 1;
    bit = loops_per_usec;
    while (bit >>= 1) {
	loops_per_usec |= bit;
	tick = clock();
	while (clock() == tick);
	tick = clock();
	ndelay(1000000000 / CLOCKS_PER_SEC);
	if (clock() > tick)
	    loops_per_usec &= ~bit;
    }
}

#else

static unsigned int ticks_per_usec = 0;

static void getCPUinfo(int *hasTSC, double *MHz)
{
    int fd;
    char buffer[4096], *p;

    *hasTSC = 0;
    *MHz = -1;

    fd = open("/proc/cpuinfo", O_RDONLY);
    if (fd == -1) {
	printf("udelay: open(/proc/cpuinfo) failed: %s", strerror(errno));
	return;
    }
    if (read(fd, &buffer, sizeof(buffer) - 1) == -1) {
	printf("udelay: read(/proc/cpuinfo) failed: %s", strerror(errno));
	close(fd);
	return;
    }
    close(fd);

    p = strstr(buffer, "flags");
    if (p == NULL) {
	printf("udelay: /proc/cpuinfo has no 'flags' line");
    } else {
	p = strstr(p, "tsc");
	if (p == NULL) {
	    printf("udelay: CPU does not support Time Stamp Counter");
	} else {
	    printf("udelay: CPU supports Time Stamp Counter");
	    *hasTSC = 1;
	}
    }

    p = strstr(buffer, "cpu MHz");
    if (p == NULL) {
	printf("udelay: /proc/cpuinfo has no 'cpu MHz' line");
    } else {
	if (sscanf(p + 7, " : %lf", MHz) != 1) {
	    error("udelay: parse(/proc/cpuinfo) failed: unknown 'cpu MHz' format");
	    *MHz = -1;
	} else {
	    printf("udelay: CPU runs at %f MHz", *MHz);
	}
    }

}


void udelay_init(void)
{
#ifdef HAVE_ASM_MSR_H

    int tsc;
    double mhz;

    getCPUinfo(&tsc, &mhz);

    if (tsc && mhz > 0.0) {
	ticks_per_usec = ceil(mhz);
	info("udelay: using TSC delay loop, %u ticks per microsecond", ticks_per_usec);
    } else
#else
    error("udelay: The file 'include/asm/msr.h' was missing at compile time.");
    error("udelay: Even if your CPU supports TSC, it will not be used!");
    error("udelay: You *really* should install msr.h and recompile LCD4linux!");
#endif

    {
	ticks_per_usec = 0;
	printf("udelay: using gettimeofday() delay loop");
    }
}


void ndelay(const unsigned long nsec)
{

#ifdef HAVE_ASM_MSR_H

    if (ticks_per_usec) {

	unsigned int t1, t2;
	unsigned long tsc;

	tsc = (nsec * ticks_per_usec + 999) / 1000;

	rdtscl(t1);
	do {
	    rep_nop();
	    rdtscl(t2);
	} while ((t2 - t1) < tsc);

    } else
#endif

    {
	struct timeval now, end;

	gettimeofday(&end, NULL);
	end.tv_usec += (nsec + 999) / 1000;
	while (end.tv_usec > 1000000) {
	    end.tv_usec -= 1000000;
	    end.tv_sec++;
	}

	do {
	    rep_nop();
	    gettimeofday(&now, NULL);
	} while (now.tv_sec == end.tv_sec ? now.tv_usec < end.tv_usec : now.tv_sec < end.tv_sec);
    }
}

#endif
