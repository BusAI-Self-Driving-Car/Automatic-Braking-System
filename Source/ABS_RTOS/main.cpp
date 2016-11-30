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
Thread *thread2;



void detect_obstacle();
void drive_motor();

/*
This is thread function  Sensor module of ABS
*/
void detect_obstacle()
{
        
        static int a=0;
        while(1){
            
            usensor.start();
            wait_ms(200); 
            dist=usensor.get_dist_cm();
            while ( dist < 12 ) {
                pc.printf("dist = %d %d\r\n",dist,a);
                a++;
                if ( a == 5 ) {
                    b = 1;
                    a = 0;
                    break;
                    }
            }
            a = 0;    
                
            pc.printf("dist: %d\r\n",dist);
        }
}

/*
This is thread function for drive motor Module of ABS
It uses Bluetooth serial interface to transfer data from mobile to app
to microcontroller
*/
void drive_motor()
{
    unsigned char inp;
    
    while(1) {
    //pc.printf("interrupt %d\r\n", b);        
    if(blue.readable()>0) {
            inp=blue.getc();
            
        }
    if ( b == 1 ) {
                inp = 'E';
                b = 0;
            }        
    
   
    pc.printf("Input key : %c %d\r\n",inp,b);        
    switch(inp) {    
         
        case 'A':
            A.speed(255);
            B.speed(255);
            break;
        case 'D':
            A.speed(255);
            B.speed(0);
            break;        
        case 'C':
            A.speed(0);
            B.speed(255);
            break;        
        case 'E':
            A.speed(0);
            B.speed(0);
            wait(1);
            A.coast();
            B.coast();
            break;            
        case 'B':
            A.speed(-255);
            B.speed(-255);
            break;            
        default:
            break;
       
        }    
    }
}

/*

*/

int main ()
{
    blue.baud(9600);
    Thread thread(detect_obstacle);
    thread2 = new Thread(drive_motor);     
    while (true) {
        Thread::wait(5000);
        pc.printf("Obstacle detected at %d cm \r\n",dist);
        blue.putc(dist);
        fflush(stdout);
        }
}
