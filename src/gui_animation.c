#include "gui_animation.h"
#include "coin.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159
#endif

// Draw a detailed coin in the X11 window using drawing primitives.
static void draw_gui_detailed_coin(Display *d, Window win, GC gc, int win_width, int win_height, int radius, const char* face) {
    // Define colors.
    unsigned long gold = 0xFFD700;
    unsigned long darkGold = 0xB8860B;
    unsigned long engravingColor = 0x8B7500;
    
    int coin_diameter = radius * 2;
    int x = (win_width - coin_diameter) / 2;
    int y = (win_height - coin_diameter) / 2;
    
    // Fill coin circle with gold.
    XSetForeground(d, gc, gold);
    XFillArc(d, win, gc, x, y, coin_diameter, coin_diameter, 0, 360*64);
    
    // Draw outer border.
    XSetForeground(d, gc, darkGold);
    XDrawArc(d, win, gc, x, y, coin_diameter, coin_diameter, 0, 360*64);
    
    // Draw an inner engraving circle.
    int inner_diameter = coin_diameter * 0.7;
    int inner_x = (win_width - inner_diameter) / 2;
    int inner_y = (win_height - inner_diameter) / 2;
    XSetForeground(d, gc, engravingColor);
    XDrawArc(d, win, gc, inner_x, inner_y, inner_diameter, inner_diameter, 0, 360*64);
    
    // Overlay text to simulate the coin’s face engraving.
    char text[50];
    if (strcmp(face, "Heads") == 0)
        snprintf(text, sizeof(text), "LIBERTY");
    else
        snprintf(text, sizeof(text), "E PLURIBUS UNUM");
    
    /* Instead of using DefaultFont (which is undefined), we load the "fixed" font */
    XFontStruct *font = XLoadQueryFont(d, "fixed");
    if (!font) {
        fprintf(stderr, "Failed to load font 'fixed'\n");
        return;
    }
    XSetFont(d, gc, font->fid);
    int text_width = XTextWidth(font, text, strlen(text));
    int text_x = (win_width - text_width) / 2;
    int text_y = win_height / 2;
    XSetForeground(d, gc, darkGold);
    XDrawString(d, win, gc, text_x, text_y, text, strlen(text));
    
    XFreeFont(d, font);
}

// GUI animation: uses X11 to animate a coin flip with a scaling (flipping) effect
// and then renders the final detailed coin with engravings.
void run_gui_animation(const char* final_result) {
    char *display_name = getenv("DISPLAY");
    if (!display_name) {
         fprintf(stderr, "No DISPLAY environment variable set. Cannot run GUI animation.\n");
         return;
    }
    
    Display *d = XOpenDisplay(NULL);
    if (d == NULL) {
        fprintf(stderr, "Unable to open X display.\n");
        return;
    }
    int screen = DefaultScreen(d);
    int win_width = 500, win_height = 500;
    Window win = XCreateSimpleWindow(d, RootWindow(d, screen), 100, 100, win_width, win_height, 1,
                                     BlackPixel(d, screen), WhitePixel(d, screen));
    XSelectInput(d, win, ExposureMask | KeyPressMask);
    XMapWindow(d, win);
    
    // Wait for an expose event.
    XEvent e;
    while (1) {
         XNextEvent(d, &e);
         if (e.type == Expose)
             break;
    }
    
    GC gc = XCreateGC(d, win, 0, 0);
    
    int total_frames = 60;
    int coin_radius = 100;
    
    for (int frame = 0; frame < total_frames; frame++) {
         XClearWindow(d, win);
         float factor = fabs(sin(frame * M_PI / total_frames));
         int current_radius = (int)(coin_radius * factor);
         if (current_radius < 1) current_radius = 1;
         draw_gui_detailed_coin(d, win, gc, win_width, win_height, current_radius, "");
         XFlush(d);
         usleep(50000);
    }
    
    // Final engraved coin.
    XClearWindow(d, win);
    draw_gui_detailed_coin(d, win, gc, win_width, win_height, coin_radius, final_result);
    XFlush(d);
    
    sleep(3);
    XCloseDisplay(d);
}

