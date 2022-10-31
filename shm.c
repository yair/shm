#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/vfs.h>
#include <errno.h>
#include <unistd.h>

const double load_thresh = 200;
const double ram_thresh = 90;
const double storage_thresh = 80;

double get_load();
double get_ram();
double get_storage();

void print_warnings(double load, double ram, double storage);

void read_error(char *fn);

int main() {

    double load = get_load();
    double ram = get_ram();
    double storage = get_storage();

    print_warnings(load, ram, storage);

    if (isatty(fileno(stdout))) {

        printf("Load: %.0lf%%  RAM: %.0lf%%  Storage: %.0lf%%\n",
                load, ram, storage);
    }

    return 0;
}

double get_load() {

    char *fn = "/proc/loadavg";
    double one_min_avg;

    FILE *fp = fopen(fn, "r");

    if (EOF == fscanf(fp, "%lf", &one_min_avg))
        read_error(fn);

    return 100. * one_min_avg;
}

double get_ram() {

    char *fn = "/proc/meminfo";
    long unsigned mem_total, mem_free, mem_available;
    double mem_usage;

    FILE *fp = fopen(fn, "r");

    if (EOF == fscanf(fp, "MemTotal:   %lu kB\nMemFree:    %lu kB\nMemAvailable:   %lu",
                      &mem_total, &mem_free, &mem_available))
        read_error(fn);

    return 100. * (1. - ((double) mem_available) / mem_total);
}

double get_storage() {

    struct statfs stats;

    if (statfs("/", &stats)) {

        printf("Error reading the root filesystem stats: %d", errno);
        exit(errno);
    }

    return 100. * (1. - ((double) stats.f_bavail) / stats.f_blocks);
}

void print_warnings(double load, double ram, double storage) {

    if (load > load_thresh)
        printf("System load over threshold (%.0lf%%): %.0lf%%\n", load_thresh, load);

    if (ram > ram)
        printf("RAM usage over threshold (%.0lf%%): %.0lf%%\n", ram_thresh, ram);

    if (storage > storage_thresh)
        printf("Storage usage over threshold (%.0lf%%): %.0lf%%\n", storage_thresh, storage);
}

void read_error(char *fn) {

    printf("Error reading %s for reading.", fn);
    exit(-1);
}
