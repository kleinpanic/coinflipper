#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>  // For usleep()

#define DELAY 300000 // 300 ms delay for animation

// Function prototypes
void flip_coin();
void generate_random_number(int lower, int upper);
void coin_animation(int result);
void random_number_animation(int num);
void handle_exit();

int main() {
    int choice, lower, upper;

    srand(time(NULL)); // Seed the random number generator

    // Initialize ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);  // Enable arrow keys and special keys

    int term_height, term_width;
    getmaxyx(stdscr, term_height, term_width); // Get terminal size

    while (1) {
        clear();
        mvprintw(term_height / 2 - 3, (term_width - 35) / 2, "Choose an option (Press 'q' or 'Esc' to quit):");
        mvprintw(term_height / 2 - 1, (term_width - 20) / 2, "1. Flip a coin");
        mvprintw(term_height / 2, (term_width - 20) / 2, "2. Generate a random number");
        mvprintw(term_height / 2 + 1, (term_width - 20) / 2, "3. Exit");
        mvprintw(term_height / 2 + 3, (term_width - 20) / 2, "Enter your choice: ");
        refresh();

        char input = getch();
        choice = input - '0';

        if (input == 'q' || input == 27) { // 'q' or 'Esc'
            handle_exit();
        }

        switch (choice) {
            case 1:
                clear();
                flip_coin();
                break;
            case 2:
                clear();
                mvprintw(4, 4, "Enter the lower bound: ");
                echo();
                scanw("%d", &lower);
                mvprintw(5, 4, "Enter the upper bound: ");
                scanw("%d", &upper);
                noecho();
                clear();
                generate_random_number(lower, upper);
                break;
            case 3:
                handle_exit();
                break;
            default:
                mvprintw(10, 4, "Invalid choice, please try again.");
                refresh();
                usleep(1000000); // Pause for 1 second before clearing
        }
    }

    endwin(); // End ncurses mode
    return 0;
}

void flip_coin() {
    int result = rand() % 2;
    coin_animation(result);
    if (result == 0)
        mvprintw(LINES - 2, (COLS - 7) / 2, "Heads!"); // Align below the animation
    else
        mvprintw(LINES - 2, (COLS - 7) / 2, "Tails!");
    refresh();
    usleep(2000000); // Display result for 2 seconds
}

void generate_random_number(int lower, int upper) {
    if (lower > upper) {
        mvprintw(8, 4, "Invalid range! Lower bound must be less than or equal to upper bound.");
        refresh();
        usleep(2000000); // Pause for 2 seconds
        return;
    }
    int num = (rand() % (upper - lower + 1)) + lower;
    random_number_animation(num);
    mvprintw(10, 4, "Random number: %d", num);
    refresh();
    usleep(2000000); // Display result for 2 seconds
}

void coin_animation(int result) {
    const char *flat_coin = 
        "   _______   \n"
        "  |       |  \n"
        "  |_______|  \n";

    const char *first_circle_coin = 
        "   ______    \n"
        "  /     \\   \n"
        "  |       |  \n"
        "  \\______/  \n";

    const char *second_circle_coin = 
        "    ____    \n"
        "  //    \\   \n"
        "  |      |   \n"
        "  \\____//   \n";

    const char *side_coin_vertical = 
        "     |=|     \n"
        "     |=|     \n"
        "     |=|     \n"
        "     |=|     \n";

    const char *side_coin_horizontal = 
        "   _______   \n"
        "   |||||||   \n"
        "   ¯¯¯¯¯¯¯   \n";

    int height = 5;  // Height the coin will "move" up

    // Center coin horizontally
    int x_center = (COLS - 11) / 2;

    // Animating the coin going up and down with better alignment
    for (int i = 0; i < height; i++) {
        clear();
        if (i == 2) {
            mvprintw(4 + i, x_center, "%s", first_circle_coin);  // Coin facing the user at the peak
        } else if (i == 1 || i == 3) {
            mvprintw(4 + i, x_center, "%s", side_coin_vertical);  // Vertical side coin
        } else {
            mvprintw(4 + i, x_center, "%s", flat_coin);  // Flat coin at start and end
        }
        refresh();
        usleep(DELAY);
    }

    // Display the final result as heads or tails, with proper alignment
    clear();
    mvprintw(4 + height, x_center, "%s", second_circle_coin); // Show circle coin
    if (result == 0) {
        mvprintw(6 + height, x_center + 4, "O"); // Center the "O" inside the coin
    } else {
        mvprintw(6 + height, x_center + 4, "X"); // Center the "X" inside the coin
    }
    refresh();
}

void random_number_animation(int num) {
    char *box_top = "+--------+\n";
    char *box_bottom = "+--------+\n";
    char number_display[20];

    // Handle different number lengths
    if (num < 10) {
        sprintf(number_display, "|   %d    |\n", num);
    } else if (num < 100) {
        sprintf(number_display, "|  %d    |\n", num);
    } else {
        sprintf(number_display, "|  %d   |\n", num);
    }

    int x_center = (COLS - 10) / 2;

    for (int i = 0; i < 5; i++) {
        clear();
        mvprintw(4, x_center, "%s", box_top);
        mvprintw(5, x_center, "%s", number_display);
        mvprintw(6, x_center, "%s", box_bottom);
        refresh();
        usleep(DELAY);
    }
}

void handle_exit() {
    endwin(); // End ncurses mode
    exit(0);
}
