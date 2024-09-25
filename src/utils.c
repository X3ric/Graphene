#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>

#ifndef PI
    #define PI 3.14159265358979323846f
#endif

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    float x, y, z;
} Vec3;

// Debug

void ErrorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void ClearOutput() {
    printf("\033[H\033[J");
}

void print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

// Text manipulation

const char* text(const char* format, ...) {
    static char buffer[100];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return buffer;
}

int textint(char *str) {
    return atoi(str);
}

float textfloat(char *str) {
    return atof(str);
}

unsigned int textlength(const char *text) {
    return strlen(text);
}

const char *textsubtext(const char *text, int position, int length) {
    static char buffer[1024];
    strncpy(buffer, text + position, length);
    buffer[length] = '\0';
    return buffer;
}

char *textreplace(const char *text, const char *replace, const char *by) {
    int len = strlen(text);
    int len_replace = strlen(replace);
    int len_by = strlen(by);
    int new_len = len - len_replace + len_by;
    char *new_text = malloc(new_len + 1);
    if (!new_text) return NULL;
    const char *current = text;
    char *new_current = new_text;
    while (*current) {
        if (strstr(current, replace) == current) {
            strcpy(new_current, by);
            current += len_replace;
            new_current += len_by;
        } else {
            *new_current++ = *current++;
        }
    }
    *new_current = '\0';
    return new_text;
}

char *textinsert(const char *text, const char *insert, int position) {
    int len = strlen(text);
    int len_insert = strlen(insert);
    char *new_text = malloc(len + len_insert + 1);
    if (!new_text) return NULL;
    strncpy(new_text, text, position);
    strcpy(new_text + position, insert);
    strcpy(new_text + position + len_insert, text + position);
    return new_text;
}

const char *textjoin(const char **textList, const char *delimiter, int count) {
    static char buffer[1024];
    buffer[0] = '\0';
    for (int i = 0; i < count; ++i) {
        strcat(buffer, textList[i]);
        if (i < (count - 1)) {
            strcat(buffer, delimiter);
        }
    }
    return buffer;
}

const char **textsplit(const char *text, char delimiter, int *count) {
    *count = 1;
    for (const char *p = text; *p; p++) {
        if (*p == delimiter) (*count)++;
    }
    const char **splits = malloc(*count * sizeof(char *));
    const char *start = text;
    int idx = 0;
    for (const char *p = text; *p; p++) {
        if (*p == delimiter) {
            int len = p - start;
            splits[idx] = malloc(len + 1);
            strncpy((char *)splits[idx], start, len);
            ((char *)splits[idx])[len] = '\0';
            idx++;
            start = p + 1;
        }
    }
    int len = text + strlen(text) - start;
    splits[idx] = malloc(len + 1);
    strncpy((char *)splits[idx], start, len);
    ((char *)splits[idx])[len] = '\0';
    return splits;
}

void textappend(char *text, const char *append, int *position) {
    strcat(text, append);
    *position += strlen(append);
}

int textfindindex(const char *text, const char *find) {
    const char *found = strstr(text, find);
    return found ? found - text : -1;
}

const char *textupper(const char *text) {
    static char buffer[1024];
    int i = 0;
    for (; text[i]; i++) {
        buffer[i] = toupper(text[i]);
    }
    buffer[i] = '\0';
    return buffer;
}

const char *textlower(const char *text) {
    static char buffer[1024];
    int i = 0;
    for (; text[i]; i++) {
        buffer[i] = tolower(text[i]);
    }
    buffer[i] = '\0';
    return buffer;
}

// Random

void RandomSeed(unsigned int seed)
{
    srand(seed);
}

int RandomValue(int min, int max)
{
    if (min > max) {
        int tmp = max;
        max = min;
        min = tmp;
    }
    return (rand()%(abs(max-min)+1) + min);
}

// Utils

void OpenURL(const char *url) {
    if (url == NULL) {
        return;
    }
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "xdg-open '%s'", url);
    system(cmd);
}

void SetClipboardText(const char *text) {
    if (text == NULL) {
        return;
    }
    char *cmd = "xclip -selection clipboard";
    FILE *xclip = popen(cmd, "w");
    if (xclip == NULL) {
        return;
    }
    fputs(text, xclip);
    pclose(xclip);
}

char *GetClipboardText(void) {
    char *cmd = "xclip -selection clipboard -o";
    FILE *xclip = popen(cmd, "r");
    if (xclip == NULL) {
        return NULL;
    }
    char *text = NULL;
    size_t size = 0;
    ssize_t nread;
    if (getline(&text, &size, xclip) == -1) {
        pclose(xclip);
        free(text);
        return NULL;
    }
    pclose(xclip);
    return text;
}

// Basic

int Clamp(int value, int min, int max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

// Smothing

float Easing(float t, const char *text) {
    if (strcmp(text, "Linear") == 0) {
        return t;
    } else if (strcmp(text, "SineIn") == 0) {
        return 1.0f - cosf((t * PI) / 2.0f);
    } else if (strcmp(text, "SineOut") == 0) {
        return sinf((t * PI) / 2.0f);
    } else if (strcmp(text, "SineInOut") == 0) {
        return -(cosf(PI * t) - 1.0f) / 2.0f;
    } else if (strcmp(text, "CubicIn") == 0) {
        return t * t * t;
    } else if (strcmp(text, "CubicOut") == 0) {
        float p = t - 1.0f;
        return p * p * p + 1.0f;
    } else if (strcmp(text, "CubicInOut") == 0) {
        if (t < 0.5f)
            return 4.0f * t * t * t;
        else {
            float f = ((2.0f * t) - 2.0f);
            return 0.5f * f * f * f + 1.0f;
        }
    } else {
        return t;
    }
}

float Motion(float speed, float intensity) {
    float sineValue = sin(glfwGetTime() * speed);
    return (sineValue + 1.0f) * 0.5f * intensity;
}

float Lerp(float start, float end, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return start + (end - start) * t;
}

// Time

double GetTime() {
    return glfwGetTime();
}

void SetTime(double time) {
    glfwSetTime(time);
}

bool Wait(double delaySeconds) {
    static double startTime = 0;
    static bool isWaiting = false;
    if (!isWaiting) {
        startTime = glfwGetTime();
        isWaiting = true;
        return false;
    }
    if (glfwGetTime() - startTime > delaySeconds) {
        isWaiting = false;
        return true;
    }
    return false;
}

// Collision Checking

bool IsInside(float x, float y, float rectX, float rectY, float rectWidth, float rectHeight) {
    return x >= rectX && x <= rectX + rectWidth &&
        y >= rectY && y <= rectY + rectHeight;
}

// Ratio resize

int Scaling(int fontsize) {
    float widthScale = (float)window.screen_width / window.width;
    float heightScale = (float)window.screen_height / window.height;
    float scale = fmin(widthScale, heightScale);
    int scaledFontSize = (int)(fontsize * scale);
    return (fontsize > 0) ? fmax(scaledFontSize, 1) : 0;
}

// File checks

bool DirExists(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

bool FileExists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

time_t GetFileModTime(const char* filePath) {
    struct stat attrib;
    if (stat(filePath, &attrib) != 0) {
        perror("Error getting file modification time");
        return -1;
    }
    return attrib.st_mtime;
}

int AddWatch(int inotifyFd, const char* filePath) {
    int wd = inotify_add_watch(inotifyFd, filePath, IN_MODIFY);
    if (wd == -1) {
        fprintf(stderr, "Error adding inotify watch for %s\n", filePath);
    }
    return wd;
}

// File Saving

char* FileLoad(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        file = fopen(path, "w");
        if (file == NULL) {
            perror("Error opening file for reading/writing");
            return NULL;
        }
        fclose(file);
        return strdup("");
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (file_size <= 0) {
        fclose(file);
        return strdup("");
    }
    char* text = (char*)malloc(file_size + 1);
    if (text == NULL) {
        perror("Error allocating memory for file content");
        fclose(file);
        return NULL;
    }
    if (fread(text, 1, file_size, file) != (size_t)file_size) {
        perror("Error reading file content");
        free(text);
        fclose(file);
        return NULL;
    }
    text[file_size] = '\0';
    fclose(file);
    return text;
}

char* FileSave(const char* path, const char* text) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return NULL;
    }
    if (fprintf(file, "%s\n", text) < 0) {
        perror("Error writing to file");
        fclose(file);
        return NULL;
    }
    fclose(file);
    return strdup(text);
}

void FileClear(const char* path) {
    if (remove(path) != 0) {
        perror("Error deleting file");
    } else {
        printf("File successfully deleted\n");
    }
}