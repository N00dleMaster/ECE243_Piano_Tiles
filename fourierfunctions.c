#include "header.h"
#include <math.h>

#define M_PI 3.14159265358979323846

void DFT(int *samples, int length, double *realOutput, double *imagOutput) {
    for (int k = 0; k < length; k++) { // Loop through each frequency component
        realOutput[k] = 0;
        imagOutput[k] = 0;
        for (int n = 0; n < length; n++) { // Loop through each sample
            double angle = 2 * M_PI * k * n / length; // Calculate angle for current frequency component
            realOutput[k] += samples[n] * cos(angle); // Update real part
            imagOutput[k] += samples[n] * sin(angle); // Update imaginary part (negative sign)
        }
    }
}