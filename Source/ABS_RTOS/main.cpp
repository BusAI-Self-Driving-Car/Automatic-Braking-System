#include "mbed.h"
#include "rtos.h"

#include "motordriver.h"
#include "hcsr04.h"

DigitalOut myled(LED1);
Serial pc(USBTX,USBRX);
Serial blue(D14,D15);
HCSR04  usensor(PTA12,PTD4);


Motor A(PTA5, PTC5, PTC4 , 1); // pwm, fwd, rev, can brake 
Motor B(PTD5, PTC13, PTC12, 1); // pwm, fwd, rev, can brake

unsigned int dist, b;
//DigitalOut led1(LED1);
//InterruptIn sw2(SW2);
//uint32_t button_pressed;
//Thread *thread1;
Thread *thread2;

/*
void sw2_press(void)
{
    thread2->signal_set(0x1);
}

void led_thread(void const *argument)
{
    while (true) {
        led1 = !led1;
        Thread::wait(1000);
    }
}

void button_thread(void const *argument)
{
    while (true) {
        Thread::signal_wait(0x1);
        button_pressed++;
    }
}

int main()
{
    Thread thread(led_thread);
    thread2 = new Thread(button_thread);

    button_pressed = 0;
    sw2.fall(&sw2_press);
    while (true) {
        Thread::wait(5000);
        printf("SW2 was pressed (last 5 seconds): %d \n", button_pressed);
        fflush(stdout);
        button_pressed = 0;
    }
}

*/

void detect_obstacle();
void drive_motor();

void detect_obstacle()
{
        pc.printf("%d\r\n",dist);
        int a=0;
        while(1){
            pc.printf("%d\r\n",dist);
            usensor.start();
            wait_ms(500); 
            dist=usensor.get_dist_cm();
            while ( dist < 6 ) {
                pc.printf("%d\r\n",dist);
                a++;
                if ( a == 5 ) {
                    b = 1;
                    a = 0;
                    }
                }
                a = 0;    
                
            pc.printf("%d\r\n",dist);
        }
}

void drive_motor()
{
    unsigned char inp;
    
    while(1) {
    pc.printf("In drive motor\r\n");        
    if(blue.readable()>0) {
            inp=blue.getc();
            if ( b == 1 ) {
                inp = 'B';
                b = 0;
            }
        }
    switch(inp) {    
         
        case 'S':
            A.speed(255);
            B.speed(255);
            break;
        case 'R':
            A.speed(255);
            B.speed(0);
            break;        
        case 'L':
            A.speed(0);
            B.speed(255);
            break;        
        case 'B':
            A.speed(0);
            B.speed(0);
            wait(1);
            A.coast();
            B.coast();
            break;            
        case 'I':
            A.speed(-255);
            B.speed(-255);
            break;            
        default:
            break;
       
        }    
    }
}

int main ()
{
    blue.baud(9600);
    Thread thread(detect_obstacle);
    thread2 = new Thread(drive_motor);     
    while (true) {
        Thread::wait(5000);
        pc.printf("Obstacle detected at %d cm \r\n",dist);
        fflush(stdout);
        }
}
