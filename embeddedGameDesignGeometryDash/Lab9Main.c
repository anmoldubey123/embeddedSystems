// Lab9Main.c
// Runs on MSPM0G3507
// Lab 9 ECE319K
// Your name
// Last Modified: 12/26/2024

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/ADC1.h"
#include "../inc/DAC5.h"
#include "../inc/Arabic.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"

#define screenheight 160
#define screenwidth 128
// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){
  Clock_Init80MHz(0);
}

Arabic_t ArabicAlphabet[]={
alif,ayh,baa,daad,daal,dhaa,dhaal,faa,ghayh,haa,ha,jeem,kaaf,khaa,laam,meem,noon,qaaf,raa,saad,seen,sheen,ta,thaa,twe,waaw,yaa,zaa,space,dot,null
};
Arabic_t Hello[]={alif,baa,ha,raa,meem,null};
Arabic_t WeAreHonoredByYourPresence[]={alif,noon,waaw,ta,faa,raa,sheen,null};
int main0(void){
  Clock_Init80MHz(0);
  LaunchPad_Init();
  ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(ST7735_WHITE);
  Arabic_SetCursor(0,15);
  Arabic_OutString(Hello);
  Arabic_SetCursor(0,31);
  Arabic_OutString(WeAreHonoredByYourPresence);
  Arabic_SetCursor(0,63);
  Arabic_OutString(ArabicAlphabet);
  while(1){
  }
}
uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

//global variables
typedef struct song {
  uint32_t length;
  uint32_t counter;
  uint32_t note_index;
  uint32_t* notes;
  uint32_t* durations;
} song_t;

// Declare song arrays first
uint32_t notes1[] = {
    5061, 4256, 3790, 3790, 4256, 5061, 5682, 5682,
    4776, 4256, 3790, 3790, 4256, 4776, 5061, 5061,
    6020, 5682, 5061, 5061, 5682, 6020, 6377, 6377,
    4776, 5061, 5682, 6020, 4776, 5061, 5682, 6020
};

uint32_t durations1[] = {
    10, 10, 5, 5, 10, 10, 5, 5,
    10, 10, 5, 5, 10, 10, 5, 5,
    10, 10, 5, 5, 10, 10, 5, 5,
    10, 10, 10, 10, 5, 5, 5, 5
};

uint32_t notes2[] = {
    3790, 4256, 5061, 4256, 3790, 3378, 3790, 4256,
    5061, 5682, 6377, 5682, 5061, 4776, 5061, 5682,
    5061, 4256, 3790, 3378, 3790, 4256, 5061, 5682,
    5061, 3790, 4256, 5061, 5682, 6377, 7143, 8028
};

uint32_t durations2[] = {
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 10
};

// Declare the songs but don't initialize them yet
song_t menuSong;
song_t gameSong;
song_t* currentSong;
uint32_t counter;

// Function to initialize the songs
void InitSongs(void) {
  menuSong.notes = notes1;
  menuSong.durations = durations1;
  menuSong.length = sizeof(notes1)/sizeof(notes1[0]);
  menuSong.counter = menuSong.durations[0];
  menuSong.note_index = 0;
  
  gameSong.notes = notes2;
  gameSong.durations = durations2;
  gameSong.length = sizeof(notes2)/sizeof(notes2[0]);
  gameSong.counter = gameSong.durations[0];
  gameSong.note_index = 0;
  
  currentSong = &menuSong;
  
  if (menuSong.length > 0) {
    Sound_Start(menuSong.notes[0]);
    counter = menuSong.durations[0];
  }
}

typedef struct ground {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t color;
} Ground_t;

// Adjust ground position so it's exactly at the bottom of the screen
Ground_t groundbottom = {0, 150, screenwidth, 10, ST7735_BLUE}; // Set to exact screen height - 10
Ground_t groundtop = {0, 0, screenwidth, 10, ST7735_BLUE};

// Game objects types
#define SPIKE_UP 1
#define SPIKE_DOWN 2
#define SPIKE_LEFT 3    // New left-facing spike
#define SPIKE_RIGHT 4   // New right-facing spike
#define PORTAL_CUBE 5   // Updated numbers to avoid conflicts
#define PORTAL_SHIP 6
#define PORTAL_WAVE 7

//game menu states
#define GAME_STATE_START 0
#define GAME_STATE_PLAYING 1
#define GAME_STATE_GAMEOVER 2
#define GAME_STATE_LEVELCOMPLETE 3
#define GAME_STATE_VICTORY 4

// Game state
uint8_t GameOver = 0;
uint8_t LevelComplete = 0;
uint32_t Score = 0;
uint32_t LevelLength = 4500; // Total length of the level
uint8_t ObjectCount = 0;
uint8_t CurrentLevel = 1;
uint8_t Semaphore = 0; // Semaphore for game loop timing
uint8_t JumpReady = 1; // Flag to prevent multiple jumps with one press
uint8_t IsInAir = 0;   // Flag to track if player is in the air
uint8_t GameState = GAME_STATE_START; // Current game state - start on instructions screen
uint8_t SelectedLevel = 1; // Current selected level
uint8_t MaxLevels = 1; // Only one level
uint8_t MenuButtonReady = 1; // Flag to prevent multiple menu button presses
uint8_t IsSpanish = 0; // Flag to track language (0 = English, 1 = Spanish)

// Player state
const uint16_t PlayerX = 30;
uint16_t PlayerY = 30;
// 1 = cube, 2 = ship, 3 = wave
uint8_t PlayerMode = 1;
int8_t PlayerYvel = 0;
int8_t PlayerYAccel = 0;
const int16_t gravity = -1;
uint8_t jumpButtonPressed = 0;
uint8_t menuButtonPressed = 0;

// Trail effect for wave mode
#define TRAIL_LENGTH 8
uint16_t waveTrailX[TRAIL_LENGTH]; // Array to store previous X positions
uint16_t waveTrailY[TRAIL_LENGTH]; // Array to store previous Y positions
uint8_t trailIndex = 0; // Current index in the trail arrays

// Variables to track previous player position for collision detection
uint16_t prevPlayerX = 30;
uint16_t prevPlayerY = 30;

// These may need to be adjusted based on your modified images
uint16_t PlayerHeight = 20; // Default height for cube
uint16_t PlayerWidth = 20;  // Default width for cube

// Sprite dimension constants - update these to match your modified images
#define CUBE_WIDTH 20
#define CUBE_HEIGHT 20
#define SHIP_WIDTH 39
#define SHIP_HEIGHT 18
#define WAVE_WIDTH 17
#define WAVE_HEIGHT 17

// Sprite offset constants - adjust these based on your modified images
#define CUBE_OFFSET 5
#define SHIP_OFFSET 8
#define WAVE_OFFSET 4

// Object dimension constants - update if you modified the object sprites
#define SPIKE_WIDTH 19
#define SPIKE_HEIGHT 19
#define PORTAL_WIDTH 7
#define PORTAL_HEIGHT 32

// Game object structure
typedef struct gameObject {
    uint8_t type;
    uint16_t x;
    uint16_t y;
    uint8_t active;
} GameObject_t;

#define MAX_OBJECTS 500
GameObject_t GameObjects[MAX_OBJECTS];

// Initialize test level with a few obstacles and portals
void InitLevel(void) {
    for(int i = 0; i < MAX_OBJECTS; i++) {
        GameObjects[i].active = 0;
    }
    
    ObjectCount = 0;
    GameOver = 0;
    LevelComplete = 0;
    IsInAir = 0;
    
    for(int i = 0; i < TRAIL_LENGTH; i++) {
        waveTrailX[i] = PlayerX;
        waveTrailY[i] = PlayerY;
    }
    trailIndex = 0;
    
    if(CurrentLevel == 1) {
        GameObjects[ObjectCount].type = SPIKE_UP;
        GameObjects[ObjectCount].x = 150;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;
        
        GameObjects[ObjectCount].type = SPIKE_UP;
        GameObjects[ObjectCount].x = 250;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;
        GameObjects[ObjectCount].type = SPIKE_UP;
        GameObjects[ObjectCount].x = 269;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;

        GameObjects[ObjectCount].type = SPIKE_UP;
        GameObjects[ObjectCount].x = 360;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;
        GameObjects[ObjectCount].type = SPIKE_UP;
        GameObjects[ObjectCount].x = 480;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;
        GameObjects[ObjectCount].type = SPIKE_UP;
        GameObjects[ObjectCount].x = 509;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;

        GameObjects[ObjectCount].type = SPIKE_UP;
        GameObjects[ObjectCount].x = 600;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;
        GameObjects[ObjectCount].type = SPIKE_UP;
        GameObjects[ObjectCount].x = 619;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;

        GameObjects[ObjectCount].type = SPIKE_UP;
        GameObjects[ObjectCount].x = 720;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;

        GameObjects[ObjectCount].type = PORTAL_SHIP;
        GameObjects[ObjectCount].x = 840;
        GameObjects[ObjectCount].y = groundbottom.y-PORTAL_HEIGHT;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;

        for(uint16_t i = 0; i<40; i++) {
          GameObjects[ObjectCount].type = SPIKE_UP;
          GameObjects[ObjectCount].x = 900+i*20;
          GameObjects[ObjectCount].y = groundbottom.y;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
          GameObjects[ObjectCount].type = SPIKE_DOWN;
          GameObjects[ObjectCount].x = 900+i*20;
          GameObjects[ObjectCount].y = groundtop.y+SPIKE_HEIGHT+groundtop.height;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
        }
        for(uint16_t i = 0; i<40; i++) {
          GameObjects[ObjectCount].type = SPIKE_UP;
          GameObjects[ObjectCount].x = 1700+i*20;
          GameObjects[ObjectCount].y = groundbottom.y-groundtop.height;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
          GameObjects[ObjectCount].type = SPIKE_DOWN;
          GameObjects[ObjectCount].x = 1700+i*20;
          GameObjects[ObjectCount].y = groundtop.y+SPIKE_HEIGHT+2*groundtop.height;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
        }
        
        GameObjects[ObjectCount].type = PORTAL_WAVE;
        GameObjects[ObjectCount].x = 2500;
        GameObjects[ObjectCount].y = groundbottom.y;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;
        GameObjects[ObjectCount].type = PORTAL_WAVE;
        GameObjects[ObjectCount].x = 2500;
        GameObjects[ObjectCount].y = groundbottom.y-PORTAL_HEIGHT;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;
        GameObjects[ObjectCount].type = PORTAL_WAVE;
        GameObjects[ObjectCount].x = 2500;
        GameObjects[ObjectCount].y = groundbottom.y-PORTAL_HEIGHT*2;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;
        GameObjects[ObjectCount].type = PORTAL_WAVE;
        GameObjects[ObjectCount].x = 2500;
        GameObjects[ObjectCount].y = groundbottom.y-PORTAL_HEIGHT*3;
        GameObjects[ObjectCount].active = 1;
        ObjectCount++;

        for(uint16_t i = 0; i<10; i++) {
          GameObjects[ObjectCount].type = SPIKE_UP;
          GameObjects[ObjectCount].x = 2500+i*20;
          GameObjects[ObjectCount].y = groundbottom.y-i*6;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
          GameObjects[ObjectCount].type = SPIKE_DOWN;
          GameObjects[ObjectCount].x = 2700+i*20;
          GameObjects[ObjectCount].y = groundtop.y+SPIKE_HEIGHT+i*6;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
        }

        for(uint16_t i = 0; i<10; i++) {
          GameObjects[ObjectCount].type = SPIKE_UP;
          GameObjects[ObjectCount].x = 2900+i*20;
          GameObjects[ObjectCount].y = groundbottom.y-i*6;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
          GameObjects[ObjectCount].type = SPIKE_DOWN;
          GameObjects[ObjectCount].x = 3100+i*20;
          GameObjects[ObjectCount].y = groundtop.y+SPIKE_HEIGHT+i*6;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
        }

        for(uint16_t i = 0; i<10; i++) {
          GameObjects[ObjectCount].type = SPIKE_UP;
          GameObjects[ObjectCount].x = 3300+i*20;
          GameObjects[ObjectCount].y = groundbottom.y-i*6;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
          GameObjects[ObjectCount].type = SPIKE_DOWN;
          GameObjects[ObjectCount].x = 3500+i*20;
          GameObjects[ObjectCount].y = groundtop.y+SPIKE_HEIGHT+i*6;
          GameObjects[ObjectCount].active = 1;
          ObjectCount++;
        }
    }
}

// Check for collisions between player and game objects
void CheckCollisions(void) {
    uint16_t playerRight = PlayerX + PlayerWidth;
    uint16_t playerTop = PlayerY - PlayerHeight;
    
    for(int i = 0; i < ObjectCount; i++) {
        if(GameObjects[i].active) {
            uint16_t objectWidth = 0;
            uint16_t objectHeight = 0;
            
            switch(GameObjects[i].type) {
                case SPIKE_UP:
                case SPIKE_DOWN:
                case SPIKE_LEFT:
                case SPIKE_RIGHT:
                    objectWidth = SPIKE_WIDTH;
                    objectHeight = SPIKE_HEIGHT;
                    break;
                case PORTAL_CUBE:
                case PORTAL_SHIP:
                case PORTAL_WAVE:
                    objectWidth = PORTAL_WIDTH;
                    objectHeight = PORTAL_HEIGHT;
                    break;
            }
            
            uint16_t objectRight = GameObjects[i].x + objectWidth;
            uint16_t objectTop = GameObjects[i].y - objectHeight;
            
            if(PlayerX < objectRight && playerRight > GameObjects[i].x &&
               playerTop < GameObjects[i].y && PlayerY > objectTop) {
                
                switch(GameObjects[i].type) {
                    case SPIKE_UP:
                    case SPIKE_DOWN:
                    case SPIKE_LEFT:
                    case SPIKE_RIGHT:
                        GameOver = 1;
                        GameState = GAME_STATE_GAMEOVER;
                        break;
                    case PORTAL_CUBE:
                        PlayerMode = 1;
                        if (PlayerY == groundbottom.y) {
                          IsInAir = 0;
                        } else {
                          IsInAir = 1;
                        }
                        break;
                    case PORTAL_SHIP:
                        PlayerMode = 2;
                        break;
                    case PORTAL_WAVE:
                        PlayerMode = 3;
                        
                        for(int i = 0; i < TRAIL_LENGTH; i++) {
                            waveTrailX[i] = PlayerX;
                            waveTrailY[i] = PlayerY;
                        }
                        trailIndex = 0;
                        break;
                }
                
                if(GameObjects[i].type >= PORTAL_CUBE) {
                    GameObjects[i].active = 0;
                }
            }
        }
    }
    
    if(PlayerX > screenwidth) {
        LevelComplete = 1;
        GameState = GAME_STATE_VICTORY;
    }
    
    if(Score > LevelLength) {
        LevelComplete = 1;
        GameState = GAME_STATE_VICTORY;
    }
}

// Legacy function that combines both buttons
uint32_t Switch_In(void){
  return Switch_In1() | Switch_In2();
}

// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ 
    uint32_t jumpButton = Switch_In1();
    uint32_t menuButton = Switch_In2();
    
    if(!jumpButton) {
      JumpReady = 1;
    }
    
    if(!menuButton) {
      MenuButtonReady = 1;
    }
    
    jumpButtonPressed = (jumpButton>0);
    menuButtonPressed = (menuButton>0);
    
    if(menuButton && MenuButtonReady) {
      MenuButtonReady = 0;
      
      if(GameState == GAME_STATE_START) {
        CurrentLevel = 1;
        InitLevel();
        GameState = GAME_STATE_PLAYING;
        Score = 0;
        PlayerY = 30;
        PlayerMode = 1;
        currentSong = &gameSong;
      } 
      else if(GameState == GAME_STATE_PLAYING) {
        GameState = GAME_STATE_START;
        currentSong = &menuSong;
      }
      else if(GameState == GAME_STATE_GAMEOVER || GameState == GAME_STATE_LEVELCOMPLETE || GameState == GAME_STATE_VICTORY) {
        GameState = GAME_STATE_START;
        currentSong = &menuSong;
      }
    }
    
    if(GameState == GAME_STATE_START) {
      uint32_t Data = ADCin();
      IsSpanish = (Data > 2048) ? 1 : 0;
    }
    
    if(GameState == GAME_STATE_PLAYING) {
      prevPlayerX = PlayerX;
      prevPlayerY = PlayerY;
      
      if(PlayerMode == 1) {
        PlayerYvel += gravity;
        
        if(jumpButtonPressed && JumpReady) {
          if(PlayerY == groundbottom.y && !IsInAir) {
            PlayerYvel = 10;
            JumpReady = 0;
            IsInAir = 1;
            
            PlayerY -= 1;
          }
        }
      }
      else if(PlayerMode == 2) {
          if(jumpButtonPressed) {
            if(PlayerYvel<=4) {
              PlayerYvel += 1;
            }
          } else {
            if(PlayerYvel>=-4) {
              PlayerYvel -= 1;
            }
          }
      }
      else if(PlayerMode == 3) {
        if(PlayerYvel == 0) {
          if(jumpButtonPressed) {
            PlayerYvel = 5;
          } else {
            PlayerYvel = -1;
          }
        } else {
          if(jumpButtonPressed) {
            PlayerYvel = 5;
          } else {
            PlayerYvel = -5;
          }
        }
      }
      
      PlayerY -= PlayerYvel;
      
      if(PlayerMode == 1) {
        PlayerHeight = CUBE_HEIGHT;
        PlayerWidth = CUBE_WIDTH;
      } 
      else if(PlayerMode == 2) {
        PlayerHeight = SHIP_HEIGHT;
        PlayerWidth = SHIP_WIDTH;
      }
      else if(PlayerMode == 3) {
        PlayerHeight = WAVE_HEIGHT;
        PlayerWidth = WAVE_WIDTH;
        
        trailIndex = (trailIndex + 1) % TRAIL_LENGTH;
        waveTrailX[trailIndex] = PlayerX;
        waveTrailY[trailIndex] = PlayerY;
      }
      
      if(PlayerY > groundbottom.y) {
        if(PlayerMode == 1) {
          PlayerY = groundbottom.y;
        } 
        else if(PlayerMode == 2) {
          PlayerY = groundbottom.y;
        }
        else if(PlayerMode == 3) {
          PlayerY = groundbottom.y;
        }
        
        if(PlayerMode == 1) {
          PlayerYvel = 0;
          JumpReady = 1;
          IsInAir = 0;
        }
      }
      
      if(PlayerY - PlayerHeight < groundtop.y + groundtop.height) {
        PlayerY = groundtop.y + groundtop.height + PlayerHeight;
        if(PlayerMode == 1) {
          PlayerYvel = 0;
        }
      }
      
      CheckCollisions();
      
      for(int i = 0; i < ObjectCount; i++) {
        if(GameObjects[i].active) {
          GameObjects[i].x -= 4;
          
          if(GameObjects[i].x + 32 < 0) {
            GameObjects[i].active = 0;
          }
        }
      }
      
      Score += 4;
    }

    Semaphore = 1;
  }
}

uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};

int main1(void){
    char l;
  __disable_irq();
  PLL_Init();
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(0x0000);
  for(phrase_t myPhrase=HELLO; myPhrase<= GOODBYE; myPhrase++){
    for(Language_t myL=English; myL<= French; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }
}

int main2(void){
  __disable_irq();
  PLL_Init();
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(ST7735_BLACK);

  for(uint32_t t=500;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6);
    Clock_Delay1ms(50);
  }
  ST7735_FillScreen(0x0000);
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }
}

int main3(void){
  __disable_irq();
  PLL_Init();
  LaunchPad_Init();
  Switch_Init();
  DAC5_Init();
  Sound_Init();
  Sound_Start(6000);
}

//sound interrupt handler
void TIMG0_IRQHandler(void) {
    if ((TIMG0->CPU_INT.IIDX) == 1) {
        if (counter > 0) {
            counter--;
        } else {
            if (currentSong != NULL) {
                currentSong->note_index++;
                if (currentSong->note_index < currentSong->length) {
                    uint32_t period = currentSong->notes[currentSong->note_index];
                    if (period == 0) {
                        Sound_Stop();
                    } else {
                        Sound_Start(period);
                    }
                    counter = currentSong->durations[currentSong->note_index];
                } else {
                    currentSong->note_index = 0;
                    if (currentSong->length > 0) {
                        uint32_t period = currentSong->notes[0];
                        Sound_Start(period);
                        counter = currentSong->durations[0];
                    }
                }
            }
        }
    }
}

// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){
  __disable_irq();
  PLL_Init();
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(ST7735_BLACK);
  
  ADCinit();
  Switch_Init();
  LED_Init();
  DAC5_Init();
  Sound_Init();
  InitSongs();
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26);
  
  InitLevel();
  
  TimerG12_IntArm(80000000/30,2);
  
  TimerG0_IntArm(10000, 40, 2);
  
  IsInAir = 0;
  GameState = GAME_STATE_START;
  __enable_irq();

  uint16_t prevPlayerX = PlayerX;
  uint16_t prevPlayerY = PlayerY;
  uint8_t prevPlayerMode = PlayerMode;
  GameObject_t prevObjects[MAX_OBJECTS];
  for(int i = 0; i < MAX_OBJECTS; i++) {
    prevObjects[i] = GameObjects[i];
  }
  
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_FillRect(groundbottom.x, groundbottom.y, groundbottom.width, groundbottom.height, groundbottom.color);
  ST7735_FillRect(groundtop.x, groundtop.y, groundtop.width, groundtop.height, groundtop.color);

  while(1){
    while(!Semaphore) {}
    Semaphore = 0;
    
    ST7735_FillScreen(ST7735_BLACK);
    
    if(GameState == GAME_STATE_START) {
      ST7735_SetCursor(5, 2);
      if(IsSpanish) {
        ST7735_OutString("GEOMETRY DASH");
        
        ST7735_SetCursor(2, 5);
        ST7735_OutString("INSTRUCCIONES:");
        
        ST7735_SetCursor(1, 7);
        ST7735_OutString("Boton 1: Saltar");
        
        ST7735_SetCursor(1, 8);
        ST7735_OutString("Boton 2: Iniciar/Salir");
        
        ST7735_SetCursor(1, 9);
        ST7735_OutString("Deslizador: Idioma");
        
        ST7735_SetCursor(1, 11);
        ST7735_OutString("Evita los pinchos");
        ST7735_SetCursor(1, 12);
        ST7735_OutString("Portales cambian");
        ST7735_SetCursor(1, 13);
        ST7735_OutString("tu modo de juego");
        
        ST7735_SetCursor(1, 15);
        ST7735_OutString("Presiona B2 para iniciar");
      } else {
        ST7735_OutString("GEOMETRY DASH");
        
        ST7735_SetCursor(3, 5);
        ST7735_OutString("INSTRUCTIONS:");
        
        ST7735_SetCursor(1, 7);
        ST7735_OutString("Button 1: Jump");
        
        ST7735_SetCursor(1, 8);
        ST7735_OutString("Button 2: Start/Exit");
        
        ST7735_SetCursor(1, 9);
        ST7735_OutString("Slider: Change language");
        
        ST7735_SetCursor(1, 11);
        ST7735_OutString("Avoid the spikes");
        ST7735_SetCursor(1, 12);
        ST7735_OutString("Portals change your");
        ST7735_SetCursor(1, 13);
        ST7735_OutString("game mode");
        
        ST7735_SetCursor(1, 15);
        ST7735_OutString("Press B2 to start");
      }
      
    } else if(GameState == GAME_STATE_PLAYING) {
      ST7735_FillRect(groundbottom.x, groundbottom.y, groundbottom.width, groundbottom.height, groundbottom.color);
      ST7735_FillRect(groundtop.x, groundtop.y, groundtop.width, groundtop.height, groundtop.color);
      
      for(int i = 0; i < ObjectCount; i++) {
        if(GameObjects[i].active) {
          switch(GameObjects[i].type) {
            case SPIKE_UP:
              ST7735_DrawBitmap(GameObjects[i].x, GameObjects[i].y, Spikeup, SPIKE_WIDTH, SPIKE_HEIGHT);
              break;
            case SPIKE_DOWN:
              ST7735_DrawBitmap(GameObjects[i].x, GameObjects[i].y, Spikedown, SPIKE_WIDTH, SPIKE_HEIGHT);
              break;
            case SPIKE_LEFT:
              ST7735_DrawBitmap(GameObjects[i].x, GameObjects[i].y, Spikeleft, SPIKE_WIDTH, SPIKE_HEIGHT);
              break;
            case SPIKE_RIGHT:
              ST7735_DrawBitmap(GameObjects[i].x, GameObjects[i].y, Spikeright, SPIKE_WIDTH, SPIKE_HEIGHT);
              break;
            case PORTAL_CUBE:
              ST7735_DrawBitmap(GameObjects[i].x, GameObjects[i].y, cportal, PORTAL_WIDTH, PORTAL_HEIGHT);
              break;
            case PORTAL_SHIP:
              ST7735_DrawBitmap(GameObjects[i].x, GameObjects[i].y, sportal, PORTAL_WIDTH, PORTAL_HEIGHT);
              break;
            case PORTAL_WAVE:
              ST7735_DrawBitmap(GameObjects[i].x, GameObjects[i].y, wportal, PORTAL_WIDTH, PORTAL_HEIGHT);
              break;
          }
        }
        
        prevObjects[i] = GameObjects[i];
      }
      
      if(PlayerMode==1) {
        ST7735_DrawBitmap(PlayerX, PlayerY, Cube, PlayerWidth, PlayerHeight);
      }
      else if(PlayerMode==2) {
        ST7735_DrawBitmap(PlayerX, PlayerY, Ship, PlayerWidth, PlayerHeight);
      }
      else if(PlayerMode==3) {
        for(int i = 0; i < TRAIL_LENGTH; i++) {
          int idx = (trailIndex - i + TRAIL_LENGTH) % TRAIL_LENGTH;
          if(i > 0) {
            uint16_t trailSize = WAVE_HEIGHT - (i * 2);
            if(trailSize >= 4) {
              uint16_t trailColor;
              if(i < 3) {
                trailColor = ST7735_CYAN;
              } else {
                trailColor = ST7735_BLUE;
              }
              
              int16_t offsetX = (WAVE_WIDTH - trailSize) / 2;
              int16_t offsetY = (WAVE_HEIGHT - trailSize) / 2;
              ST7735_FillRect(waveTrailX[idx] + offsetX, waveTrailY[idx] - trailSize + offsetY, 
                            trailSize, trailSize, trailColor);
            }
          }
        }
        
        ST7735_DrawBitmap(PlayerX, PlayerY, Wave, PlayerWidth, PlayerHeight);
      }
      
      prevPlayerX = PlayerX;
      prevPlayerY = PlayerY;
      prevPlayerMode = PlayerMode;

      uint8_t percentage = (Score * 100) / LevelLength;
      if (percentage > 100) percentage = 100;

      ST7735_SetCursor(5, 1);
      if(IsSpanish) {
        ST7735_OutString("Progreso: ");
        ST7735_OutUDec(percentage);
        ST7735_OutString("%");
      } else {
        ST7735_OutString("Progress: ");
        ST7735_OutUDec(percentage);
        ST7735_OutString("%");
      }

      ST7735_SetCursor(9, 15);
      if(IsSpanish) {
        ST7735_OutString("B2:Salir");
      } else {
        ST7735_OutString("B2:Exit");
      }
      
    } else if(GameState == GAME_STATE_GAMEOVER) {
      ST7735_FillRect(30, 64, 80, 24, ST7735_BLACK);
      
      ST7735_SetCursor(5, 8);
      if(IsSpanish) {
        ST7735_OutString("JUEGO TERMINADO");
        ST7735_SetCursor(2, 9);
        ST7735_OutString("B1: Reiniciar");
        ST7735_SetCursor(2, 10);
        ST7735_OutString("B2: Salir");
      } else {
        ST7735_OutString("GAME OVER");
        ST7735_SetCursor(2, 9);
        ST7735_OutString("B1: Restart");
        ST7735_SetCursor(2, 10);
        ST7735_OutString("B2: Exit");
      }
      
      if(jumpButtonPressed) {
        InitLevel();
        Score = 0;
        PlayerY = 30;
        PlayerMode = 1;
        IsInAir = 0;
        GameState = GAME_STATE_PLAYING;
      }
      
    } else if(GameState == GAME_STATE_LEVELCOMPLETE) {
      ST7735_FillRect(24, 64, 90, 24, ST7735_BLACK);
      
      if(IsSpanish) {
        ST7735_SetCursor(2, 8);
        ST7735_OutString("NIVEL COMPLETADO!");
        ST7735_SetCursor(2, 9);
        ST7735_OutString("B1: Reiniciar");
        ST7735_SetCursor(2, 10);
        ST7735_OutString("B2: Salir");
      } else {
        ST7735_SetCursor(3, 8);
        ST7735_OutString("LEVEL COMPLETE!");
        ST7735_SetCursor(2, 9);
        ST7735_OutString("B1: Restart");
        ST7735_SetCursor(2, 10);
        ST7735_OutString("B2: Exit");
      }
      
      if(jumpButtonPressed) {
        InitLevel();
        Score = 0;
        PlayerY = 30;
        PlayerMode = 1;
        IsInAir = 0;
        GameState = GAME_STATE_PLAYING;
      }
    } else if(GameState == GAME_STATE_VICTORY) {
      ST7735_FillScreen(ST7735_BLACK);
      
      for(int i = 0; i < 10; i++) {
        uint16_t x = Random(screenwidth);
        uint16_t y = Random(screenheight);
        ST7735_DrawCircle(x, y, ST7735_YELLOW);
      }
      
      ST7735_SetCursor(4, 6);
      if(IsSpanish) {
        ST7735_OutString("¡VICTORIA!");
        ST7735_SetCursor(5, 8);
        ST7735_OutString("¡NIVEL");
        ST7735_SetCursor(4, 9);
        ST7735_OutString("SUPERADO!");
        ST7735_SetCursor(2, 11);
        ST7735_OutString("Completado: 100%");
        ST7735_SetCursor(2, 13);
        ST7735_OutString("B1: Reiniciar");
        ST7735_SetCursor(2, 14);
        ST7735_OutString("B2: Menu Principal");
      } else {
        ST7735_SetCursor(5, 6);
        ST7735_OutString("VICTORY!");
        ST7735_SetCursor(5, 8);
        ST7735_OutString("LEVEL");
        ST7735_SetCursor(5, 9);
        ST7735_OutString("COMPLETE");
        ST7735_SetCursor(2, 11);
        ST7735_OutString("Completion: 100%");
        ST7735_SetCursor(2, 13);
        ST7735_OutString("B1: Restart");
        ST7735_SetCursor(2, 14);
        ST7735_OutString("B2: Main Menu");
      }
      
      if(jumpButtonPressed) {
        InitLevel();
        Score = 0;
        PlayerY = 30;
        PlayerMode = 1;
        IsInAir = 0;
        GameState = GAME_STATE_PLAYING;
      }
    }
  }
}
