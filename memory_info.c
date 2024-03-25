#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#define BUFSIZE 512
#define DELAY_SECONDS 2 // Reduced delay time to 2 seconds

void change_color() {
    time_t current_time;
    struct tm *time_info;
    char time_str[9]; // HH:MM:SS

    time(&current_time);
    time_info = localtime(&current_time);

    // Format the current time as HH:MM:SS
    strftime(time_str, sizeof(time_str), "%H%M%S", time_info);

    // Extract the hours, minutes, and seconds
    int hours = atoi(time_str);
    int minutes = atoi(time_str + 2);
    int seconds = atoi(time_str + 4);

    // Calculate the color based on the current time
    int color = (hours * 3600 + minutes * 60 + seconds) % 6 + 31;

    // Print the ANSI escape sequence to change text color
    printf("\033[1;%dm", color);
}

void display_memory_usage(const char *pid) {
    char path[BUFSIZE];
    FILE *status_file;

    // Construct the path to the status file
    snprintf(path, BUFSIZE, "/proc/%s/status", pid);

    // Open the status file
    status_file = fopen(path, "r");
    if (status_file != NULL) {
        char line[BUFSIZE];
        // Read lines from the status file
        while (fgets(line, BUFSIZE, status_file) != NULL) {
            // Search for the line containing "VmRSS" (Resident Set Size)
            if (strstr(line, "VmRSS:") != NULL) {
                // Extract the memory usage value
                unsigned long memory_usage;
                sscanf(line, "VmRSS: %lu", &memory_usage);
                // Print the memory usage in kilobytes
                printf("Memory Usage: %lu KB\t", memory_usage);
                break;
            }
        }
        fclose(status_file);
    } else {
        perror("Failed to open status file");
    }
}

void display_cpu_usage(const char *pid) {
    char path[BUFSIZE];
    FILE *stat_file;

    // Construct the path to the stat file
    snprintf(path, BUFSIZE, "/proc/%s/stat", pid);

    // Open the stat file
    stat_file = fopen(path, "r");
    if (stat_file != NULL) {
        unsigned long utime, stime;
        // Read CPU usage values from the stat file
        fscanf(stat_file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu "
                          "%*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld "
                          "%*ld %*ld %*llu %lu %lu", &utime, &stime);
        // Calculate total CPU usage
        unsigned long total_time = utime + stime;
        // Print CPU usage percentage
        printf("CPU Usage: %.2f%%\n", (float)total_time / sysconf(_SC_CLK_TCK));
        fclose(stat_file);
    } else {
        perror("Failed to open stat file");
    }
}

void display_running_processes() {
    DIR *dir;
    struct dirent *ent;
    char path[BUFSIZE];

    // Open the /proc directory
    if ((dir = opendir("/proc")) != NULL) {
        // Iterate through each entry in /proc
        while ((ent = readdir(dir)) != NULL) {
            // Check if the entry is a directory and its name consists entirely of digits (process IDs)
            if (ent->d_type == DT_DIR && atoi(ent->d_name) != 0) {
                // Display process ID
                printf("\033[1;34mProcess ID: %s\t\033[0m", ent->d_name);

                // Construct the path to the cmdline file
                snprintf(path, BUFSIZE, "/proc/%s/cmdline", ent->d_name);

                // Open the cmdline file
                FILE *cmdline_file = fopen(path, "r");
                if (cmdline_file != NULL) {
                    // Read the program name from the cmdline file
                    char cmdline[BUFSIZE];
                    fgets(cmdline, BUFSIZE, cmdline_file);

                    // Print the program name
                    printf("\033[1;32mProgram Name: %s\t\033[0m", cmdline);

                    fclose(cmdline_file);
                }

                // Display memory usage
                display_memory_usage(ent->d_name);

                // Display CPU usage with a different color
                printf("\033[1;33m");
                display_cpu_usage(ent->d_name);
                printf("\033[0m");
            }
        }
        closedir(dir);
    } else {
        perror("Failed to open /proc directory");
    }
}


// Function to display memory info
void display_memory_info() {
    struct sysinfo info;

    // Retrieve system information
    if (sysinfo(&info) == 0) {
        // Print memory statistics
        printf("Total RAM: %lu MB\n", info.totalram / (1024 * 1024));
        printf("Free RAM: %lu MB\n", info.freeram / (1024 * 1024));
        printf("Used RAM: %lu MB\n", (info.totalram - info.freeram) / (1024 * 1024));
    } else {
        perror("Failed to retrieve system information");
    }
}

int main() {
    while (1) {
        system("clear"); // Clear screen before displaying updated information
        change_color();
        printf(" ____  _   _   _   ____    _    ____  _   _ ____   ___    _    ____  ____  \n");
printf("/ ___|  _ \\| | | | |  _ \\  / \\  / ___|| 	| | | __ ) / _ \\  / \\  |  _ \\|  _ \\ \n");
printf("| |   | |_) | | | | | | | |/ _ \\ \\___ \\| |_| |  _ \\| | | |/ _ \\ | |_) | | | |\n");
printf("| |___|  __/| |_| | | |_| / ___ \\ ___) |  _  | |_) | |_| / ___ \\|  _ <| |_| |\n");
printf(" \\____|_|    \\___/  |____/_/   \\_\\____/|_| |_|____/ \\___/_/   \\_\\_| \\_\\____/\n");

        printf("==== Ongoing Processes ====\n");
        display_running_processes();

        printf("\n==== Memory Dashboard ====\n");
        display_memory_info();

        // Delay before refreshing information
        sleep(DELAY_SECONDS);
    }

    return 0;
}

