#include <stdio.h>
#include <math.h>
#include <time.h>
#include "fourierfunctions.c"

const int musical_notes[] = {
    16, 17, 17, 18, 19, 19, 21, 22, 23, 23, 24, 26, 26, 27, 29, 29, 31, 33, 35, 35, 37, 39, 39, 41, 44, 46, 46, 49, 52, 52, 55, 58, 58, 62, 65, 69, 69, 73, 78, 78, 82, 87, 93, 93, 98, 104, 104, 110, 117, 117, 123, 131, 139, 139, 147, 156, 156, 165, 175, 185, 185, 196, 208, 208, 220, 233, 233, 247, 262, 277, 277, 294, 311, 311, 330, 349, 370, 370, 392, 415, 415, 440, 466, 466, 494, 523, 554, 554, 587, 622, 622, 659, 698, 740, 740, 784, 831, 831, 880, 932, 932, 988, 1047, 1109, 1109, 1175, 1245, 1245, 1319, 1397, 1480, 1480, 1568, 1661, 1661, 1760, 1865, 1865, 1976, 2093, 2217, 2217, 2349, 2489, 2489, 2637, 2794, 2960, 2960, 3136, 3322, 3322, 3520, 3729, 3729, 3951, 4186, 4435, 4435, 4699, 4978, 4978, 5274, 5588, 5920, 5920, 6272, 6645, 6645, 7040, 7459, 7459, 7902
};
const int musical_notes_length = sizeof(musical_notes) / sizeof(musical_notes[0]);

const int samples_n = 264568;

struct audio_t
{
    volatile unsigned int control;
    volatile unsigned char rarc;
    volatile unsigned char ralc;
    volatile unsigned char warc;
    volatile unsigned char walc;
    volatile unsigned int ldata;
    volatile unsigned int rdata;
};

struct audio_t *const audiop = ((struct audio_t *)0xff203040);

void audio_playback_mono(int *samples, int n)
{
    int i;

    audiop->control = 0x8; // clear the output FIFOs
    audiop->control = 0x0; // resume input conversion
    for (i = 0; i < n; i++)
    {
        // wait till there is space in the output FIFO
        while (audiop->warc == 0)
            ;
        audiop->ldata = samples[i];
        audiop->rdata = samples[i];
    }
}

void audio_playback_note(int frequency, int duration)
{
    int samples[duration];
    for (int i = 0; i < duration; i++)
    {
        samples[i] = 100 * sin(2 * M_PI * frequency * i / 44100);
    }
    audio_playback_mono(samples, duration);
}


int main(void)
{
    // loop thru the array, group into samples of size 8192
    int i = 0;
    double real[8192] = {0};
    double imaginary[8192] = {0};
    int curr_note[8192] = {0};
    int curr_note_counter = 0;
    int music[40] = {0};
    while(i <= samples_n-8192) {
        // int curr_note[8192] = &samples[i];
        for(int j=0; j<8192; j++) {
            curr_note[j] = samples[i+j];
        }
        // fft this subset of the array
        DFT(curr_note, 8192, real, imaginary);
        // calculate magnitude spectrum
        double spectrum[8192] = {0};
        for(int j=0; j<8192; j++) {
            spectrum[j] = sqrt(real[j]*real[j] + imaginary[j]*imaginary[j]);
        }

        int maxIndex = 0;
        double maxMagnitude = spectrum[0];
        for (int i = 1; i < musical_notes_length; i++) {
            if (spectrum[musical_notes[i]] > maxMagnitude) {
                maxMagnitude = spectrum[musical_notes[i]];
                maxIndex = musical_notes[i];
            }
        }
        int normalizedFrequency = maxIndex % 12 + (4 * 12); // Get the remainder after dividing by 12
        music[curr_note_counter] = musical_notes[normalizedFrequency]; // Add the remainder to the 4th octave
        printf("Max frequency: %d\n", music[curr_note_counter]);
        i+= 8192;
        curr_note_counter++;
    }
    // play the music
    for(int i=0; i<40; i++) {
        audio_playback_note(music[i], 8192);
    }
    // audio_playback_mono(samples, samples_n);
    // while (1)
    //     ;
}
