#include "animation.h"
#include "coin.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <math.h>
#include <errno.h>

// Internal function declarations
static void run_terminal_animation(const char* final_result);
static void run_framebuffer_animation(const char* final_result);
static int is_framebuffer_available();
static void draw_ascii_coin(int width, int height, const char* face);
static void draw_fb_detailed_coin(char* fbp, struct fb_var_screeninfo vinfo,
                                   struct fb_fix_screeninfo finfo, int centerX,
                                   int centerY, int radius, const char* face);

void run_animation(const char* final_result) {
    if (is_framebuffer_available()) {
        run_framebuffer_animation(final_result);
    } else {
        run_terminal_animation(final_result);
    }
}

// Try opening /dev/fb0 to see if a framebuffer is available.
static int is_framebuffer_available() {
    int fb = open("/dev/fb0", O_RDWR);
    if (fb < 0) {
        return 0; // Not available.
    }
    close(fb);
    return 1;
}

// Draw an ASCII coin with “engraved” border and pattern.
// If face is nonempty, the coin’s center will show the final coin flip result.
static void draw_ascii_coin(int width, int height, const char* face) {
    for (int y = -height; y <= height; y++) {
        for (int x = -width; x <= width; x++) {
            float dx = (float)x / width;
            float dy = (float)y / height;
            if (dx*dx + dy*dy <= 1.0f) {
                // Near the edge, print a border engraving.
                float dist = dx*dx + dy*dy;
                if (dist > 0.85f)
                    printf("#");
                else {
                    // Alternate pattern to simulate fine engraving detail.
                    if (((x + y) % 2) == 0)
                        printf(".");
                    else
                        printf(" ");
                }
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    // If space allows, center the coin flip result as if engraved.
    if (strlen(face) > 0) {
        int totalWidth = width * 2 + 1;
        int textLen = strlen(face);
        int padding = (totalWidth - textLen) / 2;
        for (int i = 0; i < padding; i++) printf(" ");
        printf("[%s]\n", face);
    } else {
        printf("\n");
    }
}

// Terminal animation: simulates a coin flip by scaling the coin’s horizontal radius.
static void run_terminal_animation(const char* final_result) {
    int total_frames = 60;
    int base_width = 12;
    int base_height = 6;
    
    for (int frame = 0; frame < total_frames; frame++) {
        float factor = fabs(sin(frame * M_PI / total_frames));
        int current_width = (int)(base_width * factor);
        if (current_width < 1) current_width = 1;
        // Clear the terminal screen.
        printf("\033[H\033[J");
        draw_ascii_coin(current_width, base_height, "");
        usleep(30000);
    }
    // Final engraved coin.
    printf("\033[H\033[J");
    draw_ascii_coin(base_width, base_height, final_result);
    printf("\nFinal Result: %s\n", final_result);
    sleep(2);
}

// Helper to set a pixel in the framebuffer (supports 32bpp and 16bpp).
static void set_pixel(char* fbp, struct fb_var_screeninfo vinfo, long x, long y, unsigned int color) {
    if (x < 0 || y < 0 || x >= vinfo.xres || y >= vinfo.yres) return;
    long location = (x + y * vinfo.xres) * (vinfo.bits_per_pixel / 8);
    if (vinfo.bits_per_pixel == 32) {
        *((unsigned int*)(fbp + location)) = color;
    } else if (vinfo.bits_per_pixel == 16) {
        unsigned short col = (unsigned short)color;
        *((unsigned short*)(fbp + location)) = col;
    }
}

// Draw a detailed coin on the framebuffer with simulated engravings.
// The coin is drawn as a circle with a dark border, inner engraving line, and a subtle pattern.
// If face is nonempty, extra marks (simulating a portrait or reverse) are drawn.
static void draw_fb_detailed_coin(char* fbp, struct fb_var_screeninfo vinfo,
                                   struct fb_fix_screeninfo finfo, int centerX,
                                   int centerY, int radius, const char* face) {
    // Define colors (in 0xRRGGBB format – conversion may be needed on some systems).
    unsigned int gold = 0xFFD700;       // Base gold fill.
    unsigned int darkGold = 0xB8860B;     // Border and detail.
    unsigned int engravingColor = 0x8B7500; // For fine engraving lines.
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            float dx = (float)x / radius;
            float dy = (float)y / radius;
            if (dx*dx + dy*dy <= 1.0f) {
                int px = centerX + x;
                int py = centerY + y;
                float dist = sqrt(dx*dx + dy*dy);
                if (dist > 0.9f) {
                    set_pixel(fbp, vinfo, px, py, darkGold);
                } else if (fabs(dist - 0.5f) < 0.05f) {
                    set_pixel(fbp, vinfo, px, py, engravingColor);
                } else {
                    set_pixel(fbp, vinfo, px, py, gold);
                }
            }
        }
    }
    
    // Overlay additional detail based on coin face.
    if (strlen(face) > 0) {
        if (strcmp(face, "Heads") == 0) {
            // Simulate a simplified portrait engraving.
            for (int y = -radius/4; y <= radius/4; y++) {
                for (int x = -radius/4; x <= radius/4; x++) {
                    if ((x + y) % 3 == 0) {
                        set_pixel(fbp, vinfo, centerX + x, centerY + y, darkGold);
                    }
                }
            }
        } else {
            // Simulate reverse-side engraving.
            for (int y = -radius/4; y <= radius/4; y++) {
                for (int x = -radius/4; x <= radius/4; x++) {
                    if ((x - y) % 3 == 0) {
                        set_pixel(fbp, vinfo, centerX + x, centerY + y, darkGold);
                    }
                }
            }
        }
    }
}

// Framebuffer animation: flips the coin with a scaling effect and then renders the final engraved coin.
static void run_framebuffer_animation(const char* final_result) {
    int fb_fd = open("/dev/fb0", O_RDWR);
    if (fb_fd < 0) {
        perror("Error opening /dev/fb0");
        return;
    }
    
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    
    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo)) {
        perror("Error reading fixed information");
        close(fb_fd);
        return;
    }
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable information");
        close(fb_fd);
        return;
    }
    
    long screensize = vinfo.yres_virtual * finfo.line_length;
    char *fbp = (char *) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fbp == MAP_FAILED) {
        perror("Error: failed to map framebuffer device to memory");
        close(fb_fd);
        return;
    }
    
    int centerX = vinfo.xres / 2;
    int centerY = vinfo.yres / 2;
    int radius = (vinfo.yres < vinfo.xres ? vinfo.yres : vinfo.xres) / 8;
    int total_frames = 60;
    
    for (int f = 0; f < total_frames; f++) {
        float factor = fabs(sin(f * M_PI / total_frames));
        int currentRadius = (int)(radius * factor);
        if (currentRadius < 1) currentRadius = 1;
        // Clear the framebuffer (black).
        memset(fbp, 0, screensize);
        draw_fb_detailed_coin(fbp, vinfo, finfo, centerX, centerY, currentRadius, "");
        usleep(50000);
    }
    
    // Final coin engraving showing the actual flip result.
    memset(fbp, 0, screensize);
    draw_fb_detailed_coin(fbp, vinfo, finfo, centerX, centerY, radius, final_result);
    usleep(2000000);
    
    munmap(fbp, screensize);
    close(fb_fd);
}

