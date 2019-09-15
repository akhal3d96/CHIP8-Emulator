#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "chip8.h"
#ifdef DEBUG
#include "debug.h"
#endif

const byte CHIP8_FontSet[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t gen_rand(uint8_t limit) {
    uint8_t divisor = RAND_MAX/(limit+1);
    uint8_t retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

static inline void _0NNN(){
  puts("0x0NNN: unimplemented opcode\n");
}
static inline void _00E0(){
  // Clears the screen
  memset(chip8.GFX,0,sizeof(chip8.GFX));
}
static inline void _00EE(){
 // Returns from a subroutine
 chip8.PC = chip8.stack[--chip8.SP];
//  chip8.PC-=2;
}
static inline void _1NNN(){
  // Jumps to address NNN
  chip8.PC = chip8.opcode & 0x0FFF;
  chip8.PC -=2;
}
static inline void _2NNN(){
    // Call subroutine
    chip8.stack[chip8.SP++] = chip8.PC;
    chip8.PC = chip8.opcode & 0x0FFF;
    chip8.PC -=2;
}
static inline void _3XNN(){
    // Skips next instruction if VX == NN
  if (chip8.V[ (chip8.opcode & 0x0F00) >> 8] == (chip8.opcode & 0x00FF))
    chip8.PC += 2;
}
static inline void _4XNN(){
     // Skips next instruction if VX != NN
    if (chip8.V[(chip8.opcode & 0x0F00) >> 8] != (chip8.opcode & 0x00FF))
    chip8.PC += 2;
}
static inline void _5XY0(){
  // Skips next instruction if Vx == Vy
  if (chip8.V[(chip8.opcode & 0x0F00) >> 8] == chip8.V[((chip8.opcode & 0x00F0)) >> 4] )
    chip8.PC += 2; 
}
static inline void _6XNN(){
  // VX = NN
  chip8.V[(chip8.opcode & 0x0F00) >> 8] = chip8.opcode & 0x00FF;
}
static inline void _7XNN(){
  // Vx += NN
  chip8.V[(chip8.opcode & 0x0F00) >> 8] += (chip8.opcode & 0x00FF);
}
static inline void _8XY0(){
  // Vx = Vy
  chip8.V[(chip8.opcode & 0x0F00) >> 8] = chip8.V[(chip8.opcode & 0x00F0) >> 4];
}
static inline void _8XY1(){
  // Vx |= Vy
  chip8.V[(chip8.opcode & 0x0F00) >> 8] |= chip8.V[(chip8.opcode & 0x00F0) >> 4];
}
static inline void _8XY2(){
  // Vx &= Vy
  chip8.V[(chip8.opcode & 0x0F00) >> 8] &= chip8.V[(chip8.opcode & 0x00F0) >> 4];
}
static inline void _8XY3(){
  // Vx^=Vy 
  chip8.V[(chip8.opcode & 0x0F00) >> 8] ^= chip8.V[(chip8.opcode & 0x00F0) >> 4];
}
static inline void _8XY4(){
  // Vx += Vy (carry)
if( ((int) chip8.V[(chip8.opcode & 0x0F00) >> 8] + (int) chip8.V[(chip8.opcode & 0x00F0) >> 4]) < 256)
    chip8.V[0xF] = 0;
else
    chip8.V[0xF] = 1;  

  chip8.V[(chip8.opcode & 0x0F00) >> 8] += chip8.V[(chip8.opcode & 0x00F0) >> 4];
}
static inline void _8XY5(){
  // Vx -= Vy (carry)
if( ((int) chip8.V[(chip8.opcode & 0x0F00) >> 8] - (int) chip8.V[(chip8.opcode & 0x00F0) >> 4]) >= 0)
    chip8.V[0xF] = 1;
else
    chip8.V[0xF] = 0;
  
  chip8.V[(chip8.opcode & 0x0F00) >> 8] -= chip8.V[(chip8.opcode & 0x00F0) >> 4];
}
static inline void _8XY6(){
  //Least significant bit of VX in VF and then shifts VX to the right by 1
  chip8.V[0xF] = chip8.V[((chip8.opcode & 0x0F00) >> 8)] & 7;
  chip8.V[((chip8.opcode & 0x0F00) >> 8)] = chip8.V[((chip8.opcode & 0x0F00) >> 8)] >> 1;
}
static inline void _8XY7(){
if( ((int) chip8.V[(chip8.opcode & 0x0F00) >> 8] - (int) chip8.V[(chip8.opcode & 0x00F0) >> 4]) > 0)
    chip8.V[0xF] = 1;
else
    chip8.V[0xF] = 0;
  
  chip8.V[(chip8.opcode & 0x0F00) >> 8] = chip8.V[(chip8.opcode & 0x00F0) >> 4] - chip8.V[(chip8.opcode & 0x0F00) >> 8];

}
static inline void _8XYE(){
  // Stores the most significant bit of VX in VF and then shifts VX to the left by 1 
  chip8.V[0xF] = chip8.V[((chip8.opcode & 0x0F00) >> 8)] & 7;
  chip8.V[((chip8.opcode & 0x0F00) >> 8)] = chip8.V[((chip8.opcode & 0x0F00) >> 8)] << 1;

}
static inline void _9XY0(){
  // Skips the next instruction if VX != VY. (Usually the next instruction is a jump to skip a code block) 
  if(chip8.V[(chip8.opcode & 0x0F00) >> 8] != chip8.V[(chip8.opcode & 0x00F0) >> 4])
    chip8.PC += 2;    
}
static inline void ANNN() {
  // Set I to NNN
  chip8.I = chip8.opcode & 0x0FFF;
}
static inline void BNNN(){
  chip8.PC = chip8.V[0] + (chip8.opcode & 0x0FFF);
  chip8.PC-=2;
}
static inline void CXNN(){
  chip8.V[(chip8.opcode & 0x0F00) >> 8] = gen_rand(UINT8_MAX) & (chip8.opcode & 0x00FF);
}
static inline void DXYN(){
  // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
  register const uint16_t X = chip8.V[(chip8.opcode & 0x0F00) >> 8];
  register const uint16_t Y = chip8.V[(chip8.opcode & 0x00F0) >> 4];
  register const uint16_t N = (chip8.opcode & 0x000F);

  register uint16_t pixel;
  chip8.V[0xF] = 0;
  
  for(uint16_t yLine = 0; yLine < N; yLine++)
  {
    pixel = chip8.memory[chip8.I + yLine];
    for(uint16_t xLine = 0; xLine < 8; xLine++)
    {
      if(pixel & (0x80 >> xLine))
      {
        if (chip8.GFX[X + xLine + ((Y+ yLine) * SCREEN_WIDTH)])
          {
            chip8.V[0xF] = 1;
          }
        chip8.GFX[X + xLine + ((Y+ yLine) * SCREEN_WIDTH)] ^= 1;
      }
    }
  }
}
static inline void EX9E(){
  // Skips the next instruction if the key stored in VX is pressed.
  if(chip8.key == chip8.V[(chip8.opcode & 0x0F00) >> 8])
    chip8.PC += 2;  
}
static inline void EXA1(){
  // Skips the next instruction if the key stored in VX isn't pressed.
    if(chip8.key != chip8.V[(chip8.opcode & 0x0F00) >> 8])
    chip8.PC += 2;
}
static inline void FX07(){
  // Set Vx to timer value
  chip8.V[(chip8.opcode & 0x0F00) >> 8] = chip8.delay_timer;
  // getchar();
}
static inline void FX0A(){
  // Wait for key
  if(chip8.key == 0xFF)
    chip8.PC -= 2;
  else
    chip8.V[(chip8.opcode & 0x0F00) >> 8] = chip8.key;
    
  chip8.key = 0xFF;
}
static inline void FX15(){
  // Set delay timer to Vx
   chip8.delay_timer = chip8.V[(chip8.opcode & 0x0F00) >> 8];
}
static inline void FX18(){
  // Set sound timer
  chip8.sound_timer = chip8.V[(chip8.opcode & 0x0F00) >> 8];
}
static inline void FX1E(){
    chip8.I += chip8.V[(chip8.opcode & 0x0F00) >> 8];
}
static inline void FX29(){
  chip8.I = chip8.V[(chip8.opcode & 0x0F00) >> 8] * 5;
}
static inline void FX33(){
  chip8.memory[chip8.I] = (chip8.V[(chip8.opcode & 0x0F00) >> 8 ]/100);     // Hundreds
  chip8.memory[chip8.I + 1] = (chip8.V[(chip8.opcode & 0x0F00) >> 8 ]/10)%10;  // Tens
  chip8.memory[chip8.I + 2] = chip8.V[(chip8.opcode & 0x0F00) >> 8 ]%10;      // Ones
}
static inline void FX55(){
  for(size_t i = 0;i <= ( (chip8.opcode & 0x0F00) >> 8);i++)
    chip8.memory[chip8.I+i] = chip8.V[i];
}
static inline void FX65(){
  for(size_t i = 0;i <= ( (chip8.opcode & 0x0F00) >> 8);i++)
    chip8.V[i] = chip8.memory[chip8.I+i];
}

static inline void opcode_error()
{
  fprintf(stderr,"Uknown chip8.opcode 0x%X\n",chip8.opcode);
  getchar();
}

void CHIP8_Init()
{
    chip8.PC = 0x200;
    chip8.SP = 0;
    chip8.I  = 0;
    chip8.key = 0xFF;
    memset(chip8.memory,0,sizeof(chip8.memory));
    memset(chip8.stack, 0,sizeof(chip8.stack));
    memset(chip8.V,     0,sizeof(chip8.V));
    memset(chip8.GFX,   0,sizeof(chip8.GFX));

    for(size_t i =0; i < sizeof(CHIP8_FontSet); i++)
      chip8.memory[i] = CHIP8_FontSet[i];

    isGFXDraw = false;
}

void CHIP8_Load(const char * ROM) {

    FILE * ROM_STREAM = fopen(ROM,"rb");
    if(ROM_STREAM == NULL) {
        perror("Couldn't open ROM file");
        exit(errno);    
    }

    int ret = fread(&chip8.memory[0x200],sizeof(chip8.memory)-0x200,1,ROM_STREAM);
    if(!ret && !feof(ROM_STREAM)) {
        perror("Error reading ROM file");
        exit(errno);
    }
}


void CHIP8_Emulate()
{
    chip8.opcode = (chip8.memory[chip8.PC] << 8) | chip8.memory[chip8.PC+1];
    switch (chip8.opcode & 0xF000)
    {
    case 0x0000:
      switch (chip8.opcode & 0x000F)
      {
      case 0x0000:
        _00E0();
        isGFXDraw = true;
        break;
      case 0x0000E:
        _00EE();
        break;
      default:
      opcode_error();
      break;
      }
      break;
    case 0x1000:
      _1NNN();
      break;
    case 0x2000:
      _2NNN();
      break;
    case 0x3000:
      _3XNN();
      break;
    case 0x4000:
      _4XNN();
      break;
    case 0x5000:
      _5XY0();
      break;
    case 0x6000:
      _6XNN();
      break;
    case 0x7000:
      _7XNN();
      break;
    case 0x8000:
        switch (chip8.opcode & 0x000F)
        {
        case 0x0000:
            _8XY0();
            break;
        case 0x0001:
            _8XY1();
            break;
        case 0x0002:
            _8XY2();
            break;
        case 0x0003:
            _8XY3();
            break;
        case 0x0004:
            _8XY4();
            break;
        case 0x0005:
            _8XY5();
            break;
        case 0x0006:
            _8XY6();
            break;
        case 0x0007:
            _8XY7();
            break;
        case 0x000E:
            _8XYE();
            break;
        default:
          opcode_error();
          break;
        }
        break;
    case 0x9000:
      _9XY0();
      break;
    case 0xA000:
      ANNN();
      break;
    case 0xB000:
      BNNN();
      break;
    case 0xC000:
      CXNN();
      break;
    case 0xD000:
      DXYN();
      isGFXDraw = true;
      break;
    case 0xE000:
      switch (chip8.opcode & 0x000F)
      {
      case 0x000E:
        EX9E();
        break;
      case 0x0001:
        EXA1();
        break;
      default:
        opcode_error();
        break;
      }
      break;
    case 0xF000:
      switch (chip8.opcode & 0x00FF)
      {
      case 0x0007:
        FX07();
        break;
      case 0x000A:
        FX0A();
        break;
      case 0x0015:
        FX15();
        break;
      case 0x0018:
        FX18();
        break;
      case 0x001E:
        FX1E();
        break;
      case 0x0029:
        FX29();
        break;
      case 0x0033:
        FX33();
        break;
      case 0x0055:
        FX55();
        break;
      case 0x0065:
        FX65();
        break;
      default:
        opcode_error();
        break;
      }
    break;
    default:
      opcode_error();
      break;
    }
    
    if(chip8.delay_timer > 0) chip8.delay_timer--;
    
    if(chip8.sound_timer > 0)
    {
      if (chip8.sound_timer == 1)
        printf("\a\n");
      chip8.sound_timer--; 
    }
    #if DEBUG
        info(chip8);
        // getchar();
    #endif
    
    chip8.PC += 2;
}