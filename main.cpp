#include <stdlib.h>
#include <stdio.h>

volatile int pixel_buffer_start = (int) 0xFF203020; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

int abs(int value){
	return value > 0? value : -value;
}
void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}
void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = line_color;
}
void clear_screen() {
    for (int x = 0; x <= 319; ++x) {
        for (int y = 0; y <= 239; ++y) {
            plot_pixel(x, y, 0x0); //black color
        }
    }
}
void draw_line(int x0, int y0, int x1, int y1, short int color){
	int is_steep = abs(y1 - y0) > abs(x1 - x0);
	if(is_steep){
		swap(&x0, &y0);
		swap(&x1, &y1);
	}
	if(x0 > x1){
		swap(&x0, &x1);
		swap(&y0, &y1);
	}
	
	int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax/2);
    int y = y0;

    int y_step = y0 < y1? 1 : -1;

    for (int x = x0; x <= x1; ++x) {
        if (is_steep) {
            plot_pixel(y, x, color);
        } else {
            plot_pixel(x, y, color);
        }
        error = error + deltay;
        if (error > 0) {
            y = y + y_step;
            error = error - deltax;
        }
    }
}
void draw_rectangle(int x, int y, short int color){
    for(int i = 0; i < 10; i++){
        draw_line(x, y + i, x + 10, y + i, color);
    }
}

int main(void) {
    // Initialize pixel buffer start address
    pixel_buffer_start = 0xC8000000;

    // Clear the screen
    clear_screen();

    // Randomly generate boxes in the lanes
    int lane1 = rand() % 80;
    int lane2 = 100 + rand() % 80;
    int lane3 = 200 + rand() % 80;
    int lane4 = 300 + rand() % 80;

    // Game loop
    while (1) {
        // Move boxes down at a constant rate
        lane1 += 1;
        lane2 += 1;
        lane3 += 1;
        lane4 += 1;

        // Check if the player presses the button when the box is in the lane
        // if (button_pressed() && (lane1 >= 0 && lane1 <= 79)) {
        //     // Player gets a point
        //     score += 1;
        // } else if (button_pressed() && (lane2 >= 100 && lane2 <= 179)) {
        //     // Player gets a point
        //     score += 1;
        // } else if (button_pressed() && (lane3 >= 200 && lane3 <= 279)) {
        //     // Player gets a point
        //     score += 1;
        // } else if (button_pressed() && (lane4 >= 300 && lane4 <= 379)) {
        //     // Player gets a point
        //     score += 1;
        // } else if (button_pressed()) {
        //     // Player pressed the button when the box is not in the lane, they lose
        //     break;
        // }

        // Check if the boxes reach the bottom of the screen
        if (lane1 >= 240 || lane2 >= 240 || lane3 >= 240 || lane4 >= 240) {
            // Player misses the box, they lose
            break;
        }

        // Clear the screen
        clear_screen();

        // Draw the boxes in the lanes
        draw_rectangle(lane1, 0, 0xFFFF); // White color
        draw_rectangle(lane2, 0, 0xFFFF); // White color
        draw_rectangle(lane3, 0, 0xFFFF); // White color
        draw_rectangle(lane4, 0, 0xFFFF); // White color
    }

    // Game over
    // clear_screen();
    // draw_text("Game Over", 120, 100, 0xFFFF); // White color
    // draw_text("Score: " + std::to_string(score), 120, 120, 0xFFFF); // White color

    // // End the game
    // while (1) {
    //     // Wait for reset button to be pressed
    //     if (reset_button_pressed()) {
    //         break;
    //     }
    // }

    return 0;
}