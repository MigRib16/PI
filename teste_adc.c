/*
 * demo-adcread.c
 *
 *  Created on: 20 Jan 2016
 *
 *      compile with "gcc ADCDACPi/demos/demo-adcread.c demo-adcread.c -o demo-adcread"
 *      run with "./demo-adcread"
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "/home/projetoind/ABElectronics_C_Libraries/ADCDACPi/ABE_ADCDACPi.h"

void clearscreen ()
{
    printf("\033[2J\033[1;1H");
}

int main(int argc, char **argv){
open_adc();
return 0;
}
