#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <X11/Xlib.h>

char *get_system_vendor() 
{
    FILE *file = fopen("/sys/class/dmi/id/sys_vendor", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo sys_vendor");
        return NULL;
    }

    char vendor[256];
    if (fgets(vendor, sizeof(vendor), file) == NULL) {
        perror("Error al leer el archivo sys_vendor");
        fclose(file);
        return NULL;
    }
    fclose(file);

    char *vendor_str = (char *)malloc(strlen(vendor) * sizeof(char));
    if (vendor_str == NULL) 
    {
        perror("Error al asignar memoria");
        return NULL;
    }
    strcpy(vendor_str, vendor);
    return vendor_str;
}

char *get_system_hostname() 
{
    struct utsname sys_info;
    if (uname(&sys_info) != 0) 
    {
        perror("Error al obtener la información del sistema");
        return NULL;
    }
    char *hostname = (char *)malloc(strlen(sys_info.nodename) * sizeof(char));
    strcpy(hostname, sys_info.nodename);
    return hostname;
}


char *get_system_model() 
{
    FILE *file = fopen("/sys/class/dmi/id/product_name", "r");
    if (file == NULL) 
    {
        perror("Error al abrir el archivo product_name");
        return NULL;
    }

    char model[256];
    if (fgets(model, sizeof(model), file) == NULL) 
    {
        perror("Error al leer el archivo sys_vendor");
        fclose(file);
        return NULL;
    }
    fclose(file);

    char *model_str = (char *)malloc(strlen(model) * sizeof(char));
    if (model_str == NULL) 
    {
        perror("error al asignar memoria");
        return NULL;
    }

    strcpy(model_str, model);
    return model_str;
}

char *get_uptime()
 {
    FILE *file = fopen("/proc/uptime", "r");
    char *time = (char *)malloc(45 * sizeof(char));

    if (file == NULL) 
    {
        perror("Error al abrir el archivo /proc/uptime");
        return NULL;
    }

    double uptime, idle_time;
    if (fscanf(file, "%lf %lf", &uptime, &idle_time) != 2) 
    {
        perror("Error al leer los datos de uptime");
        return NULL;
    } 
    else 
    {
        int hours = (int)(uptime / 3600);
        int minutes = (int)((uptime - (hours * 3600)) / 60);
        char hour[10];
        char minute[12];
        sprintf(time, "%d Hours,", hours);
        sprintf(time, " %d minutes", minutes);
    }
    fclose(file);
    return time;
}

char *get_installed_packages() 
{
    FILE *fp;
    char path[255];
    char *dimanic_path = (char*)malloc(255 * sizeof(char));
    fp = popen("pacman -Q | wc -l", "r");
    if (fp == NULL) 
    {
        perror("Error al ejecutar el comando pacman");
        return NULL;
    }
    if (fgets(path, sizeof(path), fp) != NULL) 
    {
        strcpy(dimanic_path, path);
    }
    fclose(fp); 
    return dimanic_path;
}

char *get_shell() 
{
    const char *shell = getenv("SHELL");
    char * shell_pointer = (char *)malloc(45 * sizeof(char));
    if (shell != NULL) {
        strcpy(shell_pointer, shell);
    }
    return shell_pointer;
}

char *get_display_resolution() 
{
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        printf("No se pudo conectar al servidor X.\n");
        return NULL;
    }

    int screen_number = DefaultScreen(display);
    Screen *screen = ScreenOfDisplay(display, screen_number);
    int width = DisplayWidth(display, screen_number); 
    int height = DisplayHeight(display, screen_number); 

    char wdth[45];
    char hght[45];
    sprintf(wdth, "%dx", width);
    sprintf(hght, "%d", height);

    char *resolution = (char *)malloc(45 * sizeof(char));
    strcpy(resolution, wdth);
    strcat(resolution, hght);
    XCloseDisplay(display);

    return resolution;
}

char *get_desktop() 
{
    const char *desktop_env = getenv("DESKTOP_SESSION");
    char *desktop = (char *)malloc(45 * sizeof(char));
    if (desktop_env != NULL) {
        strcpy(desktop, desktop_env);
    } 
    return desktop;
}

char *get_term() 
{
    const char *terminal = getenv("TERM");
    char *term = (char *)malloc(45 * sizeof(char));
    if (terminal != NULL) 
    {
        strcpy(term, terminal);
    } 
    return term;
}

char *get_cpu_info() 
{
    FILE *fp;
    char line[255];
    char *cpu_info = (char *)malloc(65 * sizeof(char));
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) 
    {
        perror("Error al abrir /proc/cpuinfo");
        return NULL;
    }

    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        if (strncmp(line, "model name", 10) == 0) 
        {
            char *colon_pos = strchr(line, ':');
            if (colon_pos != NULL) {
                colon_pos++;
                char *newline_pos = strchr(colon_pos, '\n');
                if (newline_pos != NULL) 
                {
                    *newline_pos = '\0';
                }
                
                size_t counter = 0;
                for (size_t i = 0; i < strlen(line) + 1; i++) 
                {
                    if (line[i] == ':') 
                    {
                        counter++;
                        break;
                    }
                    counter++; 
                }

                size_t i = 0;
                while (i < strlen(line))
                {
                    if (counter == strlen(line)) 
                    {
                        cpu_info[i] = line[counter];
                        break;
                    }
                    cpu_info[i] = line[counter];
                    i++;
                    counter++;
                }
            }
            break;
        }
    }
    fclose(fp);
    return cpu_info;
}

char *get_gpu_info() 
{
    FILE *fp;
    char line[254];
    char *gpu = (char *)malloc(255 * sizeof(char));

    if (gpu == NULL) 
    {
        perror("Error al asignar memoria");
        return NULL;
    }

    fp = popen("lspci | grep -i 'VGA compatible controller'", "r");
    if (fp == NULL) 
    {
        perror("Error al ejecutar lspci");
        free(gpu);
        return NULL;
    }
    
    gpu[0] = '\0'; 
    if (fgets(line, sizeof(line), fp) != NULL) 
    {
        size_t counter = 0;
        for (size_t i = 0; i < strlen(line) + 1; i++) 
        {
            if (line[i] == '[') 
            {
                break;
            }
            counter++; 
        }

        size_t i = 0;
        while (i < strlen(line))
        {
            if (counter == strlen(line)) 
            {
                gpu[i] = line[counter];
                break;
            }
            gpu[i] = line[counter];
            i++;
            counter++;
        }
        gpu[254] = '\0';
    }

    if (gpu[0] == '\0') {
        printf("No se encontró información sobre la GPU.\n");
        free(gpu);
        pclose(fp);
        return NULL;
    }
    pclose(fp);
    return gpu;
}

char *get_kernel() 
{
    struct utsname sys_info;
    if (uname(&sys_info) != 0) 
    {
        perror("Error al obtener la información del sistema");
        return NULL;
    }
    char *kernel = (char *)malloc(45 * sizeof(char));
    sprintf(kernel, sys_info.release);
    return kernel;
}

char *get_memory_info() 
{
    FILE *fp_meminfo = fopen("/proc/meminfo", "r");
    if (fp_meminfo == NULL) 
    {
        perror("Error al abrir /proc/meminfo");
        return NULL;
    }

    char line[256];
    char *memory = (char *)malloc(300 * sizeof(char)); 

    while (fgets(line, sizeof(line), fp_meminfo)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %s", memory);
            break;
        }
    }
    fclose(fp_meminfo);
    return memory;
}


char *get_total_space_info()
{
    struct statvfs fs_info;
    char *free_space_info = (char *)malloc(45 * sizeof(char));
    if (statvfs("/", &fs_info) == 0) {
        unsigned long total_space = fs_info.f_blocks * fs_info.f_frsize;
        unsigned long free_space = fs_info.f_bfree * fs_info.f_frsize;
        int f_space = free_space / (1024 * 1024 * 1024);
        int t_space = total_space / (1024 * 1024 * 1024);
        char ts_space[45];
        sprintf(ts_space, " %i GB", t_space);
        sprintf(free_space_info, "%i GB of", f_space);
        strcat(free_space_info, ts_space);
    } else {
        perror("Error al obtener el espacio en disco");
    }
    return free_space_info;
}

void remove_newline(char *str) 
{
    char *pos;
    if ((pos = strchr(str, '\n')) != NULL) 
    {
        *pos = '\0';
    }
}

int main() 
{
    char *vendor = get_system_vendor();
    char *hostname = get_system_hostname();
    char *model = get_system_model();
    char *uptime = get_uptime();
    char *installed_packages = get_installed_packages();
    char *shell = get_shell();
    char *display_resolution = get_display_resolution();
    char *desktop = get_desktop();
    char *terminal = get_term();
    char *cpu = get_cpu_info();
    char *gpu = get_gpu_info();
    char *kernel = get_kernel();
    char *memory = get_memory_info();
    char *space = get_total_space_info();

    char *os_name_line = (char *)malloc(45 * sizeof(char));
    char *host_name_line = (char *) malloc(45 * sizeof(char));
    char *vendor_line = (char *)malloc(45 * sizeof(char));
    char *model_line = (char *)malloc(45 * sizeof(char));
    char *uptime_line = (char *)malloc(45 * sizeof(char));
    char *installed_packages_line = (char *)malloc(45 * sizeof(char));
    char *shell_line = (char *)malloc(45 * sizeof(char));
    char *display_line = (char *)malloc(45 * sizeof(char));
    char *desktop_line = (char *)malloc(45 * sizeof(char));
    char *terminal_line = (char *)malloc(45 * sizeof(char));
    char *cpu_info_line = (char *)malloc(45 * sizeof(char));
    char *gpu_info_line = (char *)malloc(300 * sizeof(char));
    char *kernel_line = (char *)malloc(45 * sizeof(char));
    char *memory_line = (char *)malloc(310 * sizeof(char));
    char *space_line = (char *)malloc(45 * sizeof(char));

    if (vendor == NULL) 
    {
        printf("vendor is null \n");
        return 1;
    }

    if (hostname == NULL) 
    {
        printf("hostname is null \n");
        return 1;
    }

    if (model == NULL) 
    {
        printf("model is null \n");
        return 1;
    }

    if (uptime == NULL) 
    {
        printf("uptime is null \n");
        return 1;
    }

    if (installed_packages == NULL) 
    {
        printf("installed_packages is null\n");
        return 1;
    }

    if (shell == NULL) 
    {
        printf("shell is null");
        return 1;
    }

    if (display_resolution == NULL) 
    {
        printf("display resolution is null");
        return 1;
    }

    if (desktop == NULL) 
    {
        printf("desktop is null");
        return 1;
    }

    if (terminal == NULL)
    {
        printf("terminal is null");
        return 1;
    }

    if (cpu == NULL) 
    {
        printf("cpu info is null");
        return 1;
    }

    if (gpu == NULL) 
    {
        printf("gpu info is null");
        return 1;
    }

    if (kernel == NULL) 
    {
        printf("kernel info is null");
        return 1;
    }

    if (memory == NULL) 
    {
        printf("memory info is null");
        return 1;
    }

    if (space == NULL)
    {
        printf("space info is null");
        return 1;
    }

    remove_newline(vendor);
    remove_newline(model);
    remove_newline(uptime);
    remove_newline(installed_packages);
    remove_newline(shell);
    remove_newline(display_resolution);
    remove_newline(desktop);
    remove_newline(terminal);
    remove_newline(cpu);
    remove_newline(gpu);
    remove_newline(kernel);
    remove_newline(memory);
    remove_newline(space);

    const char *static_logo[] = {
      "                             ..                    ",  
      "                   .oK0l                           ",
      "                 :0KKKKd.                          ",
      "                .xKO0KKKKd                         ",
      "              ,Od' .d0000l                         ",
      "             .c;.   .'''...                        ",
      ".,:cloddxxxkkkkOOOOkkkkkkkkxxxxxxxxxkkkx:          ",
      ";kOOOOOOOkxOkc'...',;;;;,,,'',;;:cllc:,.           ",
      " .okkkkd,.lko  .......',;:cllc:;,,'''''.           ",
      "   .cdo. :xd' cd:.  ..';'',,,'',,;;;,'.            ",
      "     . .ddl.;doooc'..;oc;'..';::;,'.               ",
      "        coo;.oooolllllllcccc:'.  .                 ",
      "       .ool''lllllccccccc:::::;.                   ",
      "       ;lll. .':cccc:::::::;;;;'                   ",
      "      :lcc:'',..';::::;;;;;;;,,.                   ",
      "       :cccc::::;...';;;;;,,,,,,.                  ",
      "       ,::::::;;;,'.  ..',,,,'''.                  ",
      "  ```                                              ",
      "  ........          ......                         ",
      "                                                   ",
      "                                                   ",
      "                                                   ",
      NULL
    };

    int lines = 0;
    while (static_logo[lines] != NULL)
    {
        lines++;
    }

    char **logo = (char **)malloc((lines + 1) * sizeof(char *));
    for (size_t i = 0; i < lines; i++)
    {
        logo[i] = (char *)malloc(strlen(static_logo[i] + 1) * sizeof(char));
        strcpy(logo[i], static_logo[i]);
    }
    logo[lines] = NULL;

    const char *fecth_line = "  ╔═════════════════════════════════════════════════════╗";
    const char *second_fetch_line = "    ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
    const char *third_fetch_line = "  ═══════════════════════════════════════════════════════"; 

    sprintf(os_name_line, "    ¤ OS: %s", "GNU/Linux");
    sprintf(host_name_line, "    ¤ HostName: %s", hostname);
    sprintf(vendor_line, "    ¤ Vendor: %s", vendor);
    sprintf(model_line, "    ¤ Model: %s", model);
    sprintf(uptime_line, "    ¤ Uptime: %s", uptime);
    sprintf(installed_packages_line, "    ■ Installed Packages: %s", installed_packages);
    sprintf(shell_line, "    ■ Shell: %s", shell);
    sprintf(display_line, "    ■ Display Resolution: %s", display_resolution);
    sprintf(desktop_line, "    ■ Desktop: %s", desktop);
    sprintf(terminal_line, "    ■ Terminal: %s", terminal);
    sprintf(cpu_info_line, "    ¤ CPU:%s", cpu);
    sprintf(gpu_info_line, "    ¤ GPU:%s", gpu);
    sprintf(kernel_line, "    ¤ Kernel: %s", kernel);
    sprintf(memory_line, "    ¤ Total Memory: %s GB", memory);
    sprintf(space_line, "    ¤ Free Space: %s", space);

    logo[1] = (char *)realloc(logo[1], strlen(logo[1]) + strlen(fecth_line) + 1);
    logo[2] = (char *)realloc(logo[2], strlen(logo[2]) + strlen(os_name_line) + 1);
    logo[3] = (char *)realloc(logo[3], strlen(logo[3]) + strlen(host_name_line) + 1);
    logo[4] = (char *)realloc(logo[4], strlen(logo[4]) + strlen(vendor_line) + 1);
    logo[5] = (char *)realloc(logo[5], strlen(logo[5]) + strlen(model_line) + 1);
    logo[6] = (char *)realloc(logo[6], strlen(logo[6]) + strlen(uptime_line) + 1);
    logo[8] = (char *)realloc(logo[8], strlen(logo[8]) + strlen(second_fetch_line) + 1);
    logo[9] = (char *)realloc(logo[9], strlen(logo[9]) + strlen(installed_packages_line) + 1);
    logo[10] = (char *)realloc(logo[10], strlen(logo[10]) + strlen(shell_line) + 1);
    logo[11] = (char *)realloc(logo[11], strlen(logo[11]) + strlen(display_line) + 1);
    logo[12] = (char *)realloc(logo[12], strlen(logo[12]) + strlen(desktop_line) + 1);
    logo[13] = (char *)realloc(logo[13], strlen(logo[13]) + strlen(terminal_line) + 1);
    logo[15] = (char *)realloc(logo[15], strlen(logo[15]) + strlen(second_fetch_line) + 1);
    logo[16] = (char *)realloc(logo[16], strlen(logo[16]) + strlen(cpu_info_line) + 1);
    logo[17] = (char *)realloc(logo[17], strlen(logo[17]) + strlen(gpu_info_line) + 1);
    logo[18] = (char *)realloc(logo[18], strlen(logo[18]) + strlen(kernel_line) + 1);
    logo[19] = (char *)realloc(logo[19], strlen(logo[19]) + strlen(memory_line) + 1);
    logo[20] = (char *)realloc(logo[20], strlen(logo[20]) + strlen(space_line) + 1);
    logo[21] = (char *)realloc(logo[21], strlen(logo[21]) + strlen(third_fetch_line) + 1);

    strcat(logo[1], fecth_line);
    strcat(logo[2], os_name_line);
    strcat(logo[3], host_name_line);
    strcat(logo[4], vendor_line);
    strcat(logo[5], model_line);
    strcat(logo[6], uptime_line);
    strcat(logo[8], second_fetch_line);
    strcat(logo[9], installed_packages_line);
    strcat(logo[10], shell_line);
    strcat(logo[11], display_line);
    strcat(logo[12], desktop_line);
    strcat(logo[13], terminal_line);
    strcat(logo[15], second_fetch_line);
    strcat(logo[16], cpu_info_line);
    strcat(logo[17], gpu_info_line);
    strcat(logo[18], kernel_line);
    strcat(logo[19], memory_line);
    strcat(logo[20], space_line);
    strcat(logo[21], third_fetch_line);

    for (int i = 0; logo[i] != NULL; i++) 
    {
        printf("%s\n", logo[i]);
    } 

    for (int i = 0; i < lines; i++) 
    {
        free(logo[i]);
    }
    free(logo);
    
    free(vendor);
    free(hostname);
    free(model);
    free(uptime);
    free(installed_packages);
    free(shell);
    free(display_resolution);
    free(desktop);
    free(terminal);
    free(cpu);
    free(gpu);
    free(kernel);
    free(memory);
    free(space);

    free(os_name_line);
    free(host_name_line);
    free(vendor_line);
    free(model_line);
    free(uptime_line);
    free(installed_packages_line);
    free(shell_line);
    free(display_line);
    free(terminal_line);
    free(cpu_info_line);
    free(gpu_info_line);
    free(kernel_line);
    free(memory_line);
    free(space_line);

    return 0;
}
