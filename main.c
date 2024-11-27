#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <X11/Xlib.h>

void print_colored(const char *text, const char *color) {
    printf("%s%s\033[0m", color, text);
}

void logo_arch() {
    printf("\n");
    print_colored("          _______         \n", "\033[1;34m");  
    print_colored("         /       \\        \n", "\033[1;34m");
    print_colored("        /  /\\  /\\ \\       \n", "\033[1;34m");  
    print_colored("       /  /  \\/  \\ \\      \n", "\033[1;34m");
    print_colored("      /  /    /\\    \\     \n", "\033[1;34m");  
    print_colored("     /  /    /  \\    \\    \n", "\033[1;34m");
    print_colored("    /  /    /    \\    \\   \n", "\033[1;34m");  
    print_colored("   /__/____/      \\____\\  \n", "\033[1;34m");
    print_colored("          Arch Linux       \n", "\033[1;34m");  
}


void get_system_vendor() {
    FILE *file = fopen("/sys/class/dmi/id/sys_vendor", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo sys_vendor");
        return;
    }
    char vendor[256];
    if (fgets(vendor, sizeof(vendor), file) != NULL) {
        printf("\033[1;34mSystem Vendor:\033[0m ");
        print_colored(vendor,  "\033[1;36m");
    } else {
        perror("Error al leer el archivo sys_vendor");
    }
    fclose(file);
}

void get_system_model() {
    FILE *file = fopen("/sys/class/dmi/id/product_name", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo product_name");
        return;
    }
    char model[256];
    if (fgets(model, sizeof(model), file) != NULL) {
        printf("\033[1;34mModel:\033[0m ");
        print_colored(model,  "\033[1;36m");
    } else {
        perror("Error al leer el archivo product_name");
    }
    fclose(file);
}

void get_uptime() {
    FILE *file = fopen("/proc/uptime", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo /proc/uptime");
        return;
    }

    double uptime, idle_time;
    if (fscanf(file, "%lf %lf", &uptime, &idle_time) != 2) {
        perror("Error al leer los datos de uptime");
    } else {
        int hours = (int)(uptime / 3600);
        int minutes = (int)((uptime - (hours * 3600)) / 60);
        printf("\033[1;34mUptime:\033[0m ");
        
        char hour[256];
        char minute[256];
        sprintf(hour, "%d hours,", hours);
        sprintf(minute, " %d minutes", minutes);
        print_colored(hour, "\033[1;36m");
        print_colored(minute, "\033[1;36m");
        printf("\n");
    }
    fclose(file);
}

void get_installed_packages() {
    FILE *fp;
    char path[1035];
    fp = popen("pacman -Q | wc -l", "r");
    if (fp == NULL) {
        perror("Error al ejecutar el comando pacman");
        return;
    }
    if (fgets(path, sizeof(path), fp) != NULL) {
        printf("\033[1;34mPackages:\033[0m ");
        print_colored(path, "\033[1;36m");
    }
    fclose(fp); 
}

const char* get_shell() {
    const char* shell = getenv("SHELL");
    if (shell != NULL) {
        return shell;
    }
    return "Undefinided";
}

void get_display_resolution() {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        printf("No se pudo conectar al servidor X.\n");
        return;
    }
    int screen_number = DefaultScreen(display);
    Screen *screen = ScreenOfDisplay(display, screen_number);
    int width = DisplayWidth(display, screen_number); 
    int height = DisplayHeight(display, screen_number); 
    char wdth[256];
    char hght[256];
    sprintf(wdth, "%dx", width);
    sprintf(hght, "%d", height);
    printf("\033[1;34mResolution:\033[0m ");
    print_colored(wdth, "\033[1;36m");
    print_colored(hght, "\033[1;36m");
    printf("\n");
    XCloseDisplay(display);
}

void get_desktop() {
    const char *desktop_env = getenv("DESKTOP_SESSION");
    if (desktop_env != NULL) {
        printf("\033[1;34mDesktop:\033[0m");
        print_colored(desktop_env, "\033[1;36m");
        printf("\n");
    } else {
        printf("No se pudo obtener el entorno de escritorio.\n");
    }
}

void get_term() {
    const char *terminal = getenv("TERM");
    if (terminal != NULL) {
        printf("\033[1;34mTerminal:\033[0m");
        print_colored(terminal, "\033[1;36m");
        printf("\n");
    } else {
        printf("No se pudo obtener el tipo de terminal.\n");
    }
}

void get_cpu_info() {
    FILE *fp;
    char linea[256];
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        perror("Error al abrir /proc/cpuinfo");
        return;
    }

    while (fgets(linea, sizeof(linea), fp) != NULL) {
        if (strncmp(linea, "model name", 10) == 0) {
            char *colon_pos = strchr(linea, ':');
            if (colon_pos != NULL) {
                colon_pos++;
                char *newline_pos = strchr(colon_pos, '\n');
                if (newline_pos != NULL) {
                    *newline_pos = '\0';
                }
                printf("\033[1;34mCPU:\033[0m");
                print_colored(colon_pos, "\033[1;36m");
                printf("\n");
            }
            break;
        }
    }

    fclose(fp);
}

void get_gpu_info() {
     FILE *fp;
    char linea[256];
    fp = popen("lspci | grep -i 'VGA compatible controller'", "r");
    if (fp == NULL) {
        perror("Error al ejecutar lspci");
        return;
    }
    while (fgets(linea, sizeof(linea), fp) != NULL) {
        printf("\033[1;34mGPU:\033[0m ");
        print_colored(linea, "\033[1;36m");
    }
    pclose(fp);
}


void print_system_info() {
    struct utsname sys_info;
    if (uname(&sys_info) != 0) {
        perror("Error al obtener la información del sistema");
        return;
    }

    logo_arch();

    printf("\n\033[1;34mOS:\033[0m ");
    print_colored(sys_info.sysname, "\033[1;36m");  
    printf("\n");

    get_system_vendor();
    get_system_model();
    get_uptime();
    get_installed_packages();

    printf("\033[1;34mShell:\033[0m ");
    print_colored(get_shell(), "\033[1;36m");  
    printf("\n");

    get_display_resolution();
    get_desktop();
    get_term();
    get_cpu_info();
    get_gpu_info();

    printf("\033[1;34mhostname:\033[0m ");
    print_colored(sys_info.nodename, "\033[1;36m");
    printf("\n");

    printf("\033[1;34mKernel:\033[0m ");
    print_colored(sys_info.release, "\033[1;36m");
    printf("\n");

    FILE *fp_meminfo = fopen("/proc/meminfo", "r");
    if (fp_meminfo == NULL) {
        perror("Error al abrir /proc/meminfo");
        return;
    }

    char line[256];
    char total_mem[256];
    while (fgets(line, sizeof(line), fp_meminfo)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %s", total_mem);
            break;
        }
    }
    fclose(fp_meminfo);

    printf("\033[1;34mMemory: \033[0m");
    print_colored(total_mem, "\033[1;36m");
    printf("\n");

    struct statvfs fs_info;
    if (statvfs("/", &fs_info) == 0) {
        unsigned long total_space = fs_info.f_blocks * fs_info.f_frsize;
        unsigned long free_space = fs_info.f_bfree * fs_info.f_frsize;
        printf("\033[1;34mFree space: \033[0m");
        printf("%lu GB of %lu\n", free_space / (1024 * 1024 * 1024), total_space / (1024 * 1024 * 1024));
    } else {
        perror("Error al obtener el espacio en disco");
    }
}

int main() {
    print_system_info();
    return 0;
}
