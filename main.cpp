w#include "mbed.h"
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut distance_led(LED1);
DigitalOut no_read_led(LED2);
DigitalOut Trigger(p5);
DigitalIn  Echo(p6);
Timer time_out;
Timer echo_length;
bool found_echo;
Ticker led_flasher;

float distance = 0;
int last_range = 0;

void cycle_led()
{
    distance_led = !distance_led;
}

int main() {
    while(1) {
        Trigger = 1;    //Send a trigger pulse
        wait(0.01);
        Trigger = 0;
        
        time_out.start();
        found_echo = true;
        while(Echo == 0) //Wait for a pulse...
        {
            if(time_out.read_ms() > 500)
            {
                found_echo = false;
                break;
            }
        }
        echo_length.start();
        while(Echo == 1 && found_echo)
        {
        }
        echo_length.stop();
        
        if(found_echo)
        {
            distance = echo_length.read_us() / 58.0;
            if(distance < 1 || distance > 400)
            {
                pc.printf("Out of range!\r\n");
                found_echo = false;
            }
            else
            {
                pc.printf("Reading : %f\r\n", distance);
                //Flasher logic.  
                if(distance < 400 && distance > 30)
                {
                    led_flasher.detach();
                    distance_led = 0;
                    last_range = 0;
                }
                else if(distance <= 30 && distance > 20 && last_range != 30)
                {
                    led_flasher.detach();
                    distance_led = 1;
                    last_range = 30;
                    pc.printf("New range: 30\r\n");
                }
                else if (distance <= 20 && distance > 10 && last_range != 20)
                {   
                    led_flasher.detach();
                    led_flasher.attach(&cycle_led, 0.25);
                    last_range = 20;
                    pc.printf("New range: 20\r\n");
                }
                else if(distance <= 10 && distance > 0 && last_range != 10)
                {
                    led_flasher.detach();
                    led_flasher.attach(&cycle_led, 0.1);
                    last_range = 10;
                    pc.printf("New range: 10\r\n");
                }
            }
        }
        else
        {   
            pc.printf("No reading!\r\n");
            led_flasher.detach();
            distance_led = 0;
            distance = 0;
        }
        
        echo_length.reset();
        time_out.stop();
        time_out.reset();
        no_read_led = !found_echo;
        wait(0.25);
    }
}
