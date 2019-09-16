#ifdef DEBUG

#include <stdio.h>

#include "chip8.h"

void debug(const char * msg)
{
    printf("[DEBUG] %s\n",msg);
}

void info(CHIP8 chip8)
{
    size_t i,c;
    printf("opcode: 0x%X\n",chip8.opcode);
    printf("I: 0x%x\tPC: 0x%x\n",chip8.I,chip8.PC);

    printf("VX:\n");
    for(i = 0, c = 1;i<sizeof(chip8.V);i++, c++){
        printf("[%lX] 0x%02x\t",i,chip8.V[i]);
        if(c == 3) {
            c = 0;
            puts("");
        }
    }
    
        if (c != 3) puts("");
        
        printf("Stack:\tSP: %d\n",chip8.SP);
        
        for(i = 0, c = 1;i<sizeof(chip8.stack);i++,c++){
            printf("[%02ld] 0x%x\t",i,chip8.stack[i]);
            if(c == 3) {
                c = 0;
                puts("");
            }
        }

    puts("\n");
}
#endif