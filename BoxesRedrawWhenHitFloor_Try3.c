#include <stdlib.h> // For using the rand() function
int pixel_buffer_start; // global variable
void plot_pixel(int x, int y, short int color);






int abs(int value){
    return value > 0? value : -value;
}



void swap(int *a, int *b) {

    int temp = *a;
    *a = *b;
    *b = temp;
}

void fill_square(int x, int y, int width, int height, short int color) {
    for (int i = y; i < y + height; ++i) {
        draw_line(x, i, x + width, i, color);
    }
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



void wait() {
    volatile int *buffer_register = (int*) 0xFF203020;
    *buffer_register = 1;
    int s = *(buffer_register + 3) & 1;
    while(s != 0) { s = *(buffer_register + 3) & 1; }
}


int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen();
    
    int x_positions[4] = {0, 80, 160, 240}; // Starting x positions for each square in 4 columns
    int y_positions[4] = {0, 0, 0, 0}; // Initial y positions for each square
    int delays[4] = {0, 30, 60, 90}; // Delays for start moving (staggered starts)
    int width = 50; // Width of each square
    int height = 50; // Height of each square

    int frame_counter = 0; // Frame counter to manage delays

    while(1) {
        wait();

        for (int i = 0; i < 4; ++i) {
            // Erase previous square by filling it in black, only if its delay is passed
            if (frame_counter >= delays[i]) {
                fill_square(x_positions[i], y_positions[i], width, height, 0); // Erase previous square

                // Check if square has hit the bottom. If yes, reset its position to the top.
                if (y_positions[i] + height >= 239) {
                    y_positions[i] = 0; // Reset to top
                } else {
                    y_positions[i]++; // Continue falling
                }
                
                // Draw new filled square
                fill_square(x_positions[i], y_positions[i], width, height, 0x07E0); // Fill new square with green color
            }
        }

        frame_counter++; // Increment frame counter
    } 
}
