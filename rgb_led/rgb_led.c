#include <wiringPi.h>
#include <softPwm.h>
#include <rgb_led.h>

int color = 0;
int brightness = 0;

int red, green, blue;


int colors[3][3] =
{
    {100, 0, 0},   // RED
    {0, 100, 0},   // GREEN
    {0, 0, 100}    // BLUE
};

void RGB_Init(int R, int G, int B)
{
	red = R;
	green = G;
	blue = B;
    softPwmCreate(red, 0, 100);
    softPwmCreate(green, 0, 100);
    softPwmCreate(blue, 0, 100);
}

void RGB_SetBrightness(int value)
{
    brightness = value;

    softPwmWrite(red, colors[color][0] * brightness / 100);
    softPwmWrite(green, colors[color][1] * brightness / 100);
    softPwmWrite(blue, colors[color][2] * brightness / 100);
}
void RGB_SetColor(int value)
{
    color = value;

    RGB_SetBrightness(brightness);
}
