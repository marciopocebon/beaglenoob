/* This program uses a PWM pin to play the starwars music on a buzzer
 * To use the PWM, the system clock for PWMx has to be enabled first
 * This is done in 2 steps :
 *     1/ mmaps the /dev/mem file and allows direct write access to registers
 *     2/ the appropriate system clock (0-2) is set to running (0x2)
 * Afterwards, the correct MUX mode for the pin needs to be set (6 instead of 0)
 * The PWM is then started by writing 1 to the file 'run' and the frequency can 
 * be changed in 'period_freq' and the ratio (aka duty) in 'duty_percent'
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sched.h>
#include <stdint.h>


#define c 262
#define d 294
#define e 330
#define f 349
#define g 392
#define gS 415
#define a 440
#define aS 466
#define b 494
#define cH 524
#define cSH 554
#define dH 588
#define dSH 622
#define eH 660
#define fH 698
#define fSH 740
#define gH 784
#define gSH 830
#define aH 880

#define START 0x44e00000    // see datasheet of AM335x
#define LENGTH 1024
#define OFFSET 0xcc         // offset of PWM1 clock (see datasheet of AM335x p.1018)

int fd;
int pid[2];

void msleep(x)
{
    int j=0;
	for(j=0; j<x; j++)
	usleep(1000);
}

void beep(freq, duration)
{
    FILE *pwm_period, *pwm_duty;
    
    pwm_period = fopen("/sys/class/pwm/ehrpwm.1:0/period_freq", "w");
    pwm_duty = fopen("/sys/class/pwm/ehrpwm.1:0/duty_percent", "w"); 
    fprintf(pwm_period, "%d", freq);
    fprintf(pwm_duty, "50");
    fclose(pwm_period);
    fclose(pwm_duty);
    msleep(duration);
    
    pwm_duty = fopen("/sys/class/pwm/ehrpwm.1:0/duty_percent", "w");
    fprintf(pwm_duty, "0");
    fclose(pwm_duty);
	msleep(20);
}

int main(int argc, char** argv[])
{
    FILE *pwm_run, *pwm_mux;
    int i;
    volatile uint32_t *epwmss1;
    int fd = open("/dev/mem", O_RDWR);
    
    if(fd < 0)
        {
        printf("Can't open /dev/mem\n");
        return 1;
        }

    epwmss1 = (volatile uint32_t *) mmap(NULL, LENGTH, PROT_READ|PROT_WRITE, MAP_SHARED, fd, START);
    if(epwmss1 == NULL)
        {
        printf("Can't mmap\n");
        return 1;
        }
    else
		{
		epwmss1[OFFSET / sizeof(uint32_t)] = 0x2;
		}
    close(fd);
    
	sched_setscheduler(0, SCHED_FIFO, NULL);   // get real time privileges
    pwm_mux = fopen("/sys/kernel/debug/omap_mux/gpmc_a2", "w");
    fprintf(pwm_mux, "6");                     // pwm is mux mode 6
    fclose(pwm_mux);
    
    pwm_run = fopen("/sys/class/pwm/ehrpwm.1:0/run", "w");
    fprintf(pwm_run, "1");
    fclose(pwm_run);
	
	beep(a, 500);
    beep(a, 500);
    beep(a, 500);
    beep(f, 350);
    beep(cH, 150);  
    beep(a, 500);
    beep(f, 350);
    beep(cH, 150);
    beep(a, 650);
 
    msleep(150); 
 
    beep(eH, 500);
    beep(eH, 500);
    beep(eH, 500);   
    beep(fH, 350);
    beep(cH, 150);
    beep(gS, 500);
    beep(f, 350);
    beep(cH, 150);
    beep(a, 650);
 
    msleep(150);
 
    beep(aH, 500);
    beep(a, 300);
    beep(a, 150);
    beep(aH, 400);
    beep(gSH, 200);
    beep(gH, 200); 
    beep(fSH, 125);
    beep(fH, 125);    
    beep(fSH, 250);
 
    msleep(250);
 
    beep(aS, 250); 
    beep(dSH, 400); 
    beep(dH, 200);  
    beep(cSH, 200);  
    beep(cH, 125);  
    beep(b, 125);  
    beep(cH, 250);  
 
    msleep(250);
 
    beep(f, 125);  
    beep(gS, 500);  
    beep(f, 375);  
    beep(a, 125);
    beep(cH, 500);
    beep(a, 375);  
    beep(cH, 125);
    beep(eH, 650);
 
    beep(aH, 500);
    beep(a, 300);
    beep(a, 150);
    beep(aH, 400);
    beep(gSH, 200);
    beep(gH, 200);
    beep(fSH, 125);
    beep(fH, 125);    
    beep(fSH, 250);
 
    msleep(250);
 
    beep(aS, 250);  
    beep(dSH, 400);  
    beep(dH, 200);  
    beep(cSH, 200);  
    beep(cH, 125);  
    beep(b, 125);  
    beep(cH, 250);      
 
    msleep(250);
 
    beep(f, 250);  
    beep(gS, 500);  
    beep(f, 375);  
    beep(cH, 125);
    beep(a, 500);   
    beep(f, 375);   
    beep(cH, 125); 
    beep(a, 650);   

    
    pwm_run = fopen("/sys/class/pwm/ehrpwm.1:0/run", "w");
    fprintf(pwm_run, "0");
    fclose(pwm_run);

return 0;
}
