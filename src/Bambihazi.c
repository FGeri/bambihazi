/**************************************************************************//**
 * @file
 * @brief Empty Project
 * @author Energy Micro AS
 * @version 3.20.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See 
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"  
 * for details. Before using this software for any purpose, you must agree to the 
 * terms of that agreement.
 *
 ******************************************************************************/

/*Built in libraries*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.H>
#include <math.h>

/* Device Support Library */
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_lcd.h"
#include "em_gpio.h"
#include "em_rtc.h"

/* Drivers */
#include "vddcheck.h"
#include "segmentlcd.h"
#include "caplesense.h"
#include "rtcdriver.h"
#include "InitDevice.h"
#include "segmentlcd.h"
#include "segmentlcd_spec.h"
#include "segmentlcdconfig.h"

#define BANANAS_MAX 25

typedef enum
{
  ERROR = -1,
  BEFORE_START = 0,
  IN_GAME = 1,
  GAME_OVER = 2
} GAME_STATES;

int8_t fallingBananas[4]={-1,-1,-1,-1};

static volatile GAME_STATES currentState = BEFORE_START;
static volatile int32_t difficulty=0;
SegmentLCD_SegmentData_TypeDef segments[8];

uint16_t totalBananas=0;
uint16_t caughtBananas=0;
uint8_t bucketPos=0;
int stringPos=0;

//Local prototypes of functions
void capSenseAringUpdate(int sliderPos);
void capSenseBasketUpdate(int sliderPos);
void dropNewBanana(void);
void bananaCaught(void);
void RTC_expired(RTCDRV_TimerID_t id, void *user);
void delay(void);
void displayBananas(void);
void shiftString(RTCDRV_TimerID_t id, void *user);
void clearScreen(void);

//Timer for counting time for the bananas
RTCDRV_TimerID_t xTimerForNewBanana;
RTCDRV_TimerID_t xTimerForString;
/**************************************************************************//**
 * @brief  A simple delay loop
 *****************************************************************************/

void capSenseAringUpdate(int sliderPos)
{
  int i;
  int stop;

  if (sliderPos == -1)
  {
    /* No change in ring if touch slider is not touched */
    stop = difficulty;
  }
  else
  {

    /* Map 8 segments to 48 slider positions */
    stop = (sliderPos * 8) / 48;
    difficulty=stop;
  }

  /* Draw ring */
  for (i=0; i < 8; i++)
  {
    if (i <= stop )
    {
      SegmentLCD_ARing(i, 1);
    }
    else
    {
      SegmentLCD_ARing(i, 0);
    }
  }
}
void capSenseBasketUpdate(int sliderPos)
{
	  uint8_t currentBucketPos;
	  uint8_t i;

	  if (sliderPos == -1)
	  {
	    /* No change in ring if touch slider is not touched */
		  currentBucketPos = bucketPos;
	  }
	  else
	  {

	    /* Map 4 positions to 48 slider positions */
		  currentBucketPos = (sliderPos * 4) / 49;
		  segments[bucketPos].d=0;
		  bucketPos=currentBucketPos;
	  }

	  /* Draw ring */
	  for (i=0; i < 8; i++)
	  {
		segments[i].raw=0;
	    if (i == currentBucketPos )
	    {
	      segments[i].d=1;
	    }
	  }
	  bananaCaught();
	  displayBananas();
	  displaySegmentField(segments);
}
void GPIO_EVEN_IRQHandler(void)

{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << BTN1_PIN);



  switch (currentState)
  {
  	//If the setting are accepted we start the game
    case (BEFORE_START):
			//Configuring the interrupts
		  GPIO_IntConfig(BTN1_PORT,BTN1_PIN,false,true,false);
		  //Configuring the nested vector interrupt controller
		  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
		currentState=IN_GAME;
    	RTCDRV_StartTimer( xTimerForNewBanana, rtcdrvTimerTypePeriodic, (1000/(1+difficulty)), RTC_expired, NULL);
      break;
    case (GAME_OVER):
		RTCDRV_StopTimer( xTimerForString );
    	for (int i=0;i<4;i++) fallingBananas[i]=-1;
    	totalBananas=0;
    	caughtBananas=0;
    	currentState=BEFORE_START;
    	SegmentLCD_Write("");
    	break;
    default:
    	//TODO implementáld a többi esetet is
      break;
  }

}
void RTC_expired(RTCDRV_TimerID_t id, void *user){

	dropNewBanana();
}
void dropNewBanana(void){
	for (int i=2;i>=0;i--){
		fallingBananas[i+1]=fallingBananas[i];
	}
	if (totalBananas<BANANAS_MAX){

		fallingBananas[0]=rand()%4;
		totalBananas++;

	}else{
		fallingBananas[0]=-1;
	}
	bananaCaught();
}
void bananaCaught(void){
		if (fallingBananas[3]==bucketPos){
			fallingBananas[3]=-2;
			caughtBananas++;
		}else if (fallingBananas[3]==-1 && totalBananas==BANANAS_MAX){
			currentState=GAME_OVER;
			stringPos=0;
			RTCDRV_StopTimer( xTimerForNewBanana );
			RTCDRV_StartTimer( xTimerForString, rtcdrvTimerTypePeriodic, (500), shiftString, NULL);
			GPIO_IntConfig(BTN1_PORT,BTN1_PIN,false,true,true);
			NVIC_EnableIRQ(GPIO_EVEN_IRQn);
		}
}
void displayBananas(void){
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++)
			if (fallingBananas[i]==j){
				switch(i){
					case 0: segments[j].a =1;
						break;
					case 1: segments[j].j =1;
						break;
					case 2: segments[j].p =1;
						break;
					case 3: segments[j].d =1;
						break;
				}
			}
	}
}
void clearScreen(void){
	for(int i=0;i<4;i++){
		segments[i].a =0;
		segments[i].j =0;
		segments[i].p =0;
		segments[i].d =0;
	}
}
void shiftString(RTCDRV_TimerID_t id, void *user){
	char text[8];
	char tmp[32]={"GAME OVER GAME OVER"};
	stringPos=(stringPos+1)%10;
	strncpy(text,tmp+stringPos,7);
	SegmentLCD_Write(text);
}
void delay() {
	for(int d=0;d<500000;d++);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();
  enter_DefaultMode_from_RESET();
  //Initialising the capacitive touch sensor
  CAPLESENSE_Init (false);
  CAPLESENSE_setupLESENSE(false);
  //Initialising the screen
  SegmentLCD_Init(false);
  //Configuring the interrupts
  GPIO_IntConfig(BTN1_PORT,BTN1_PIN,false,true,true);
  //Configuring the nested vector interrupt controller
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  /* Initialize RTC timer. */
    RTCDRV_Init();
    RTCDRV_AllocateTimer( &xTimerForNewBanana);
    RTCDRV_AllocateTimer( &xTimerForString);
    //RTCDRV_StartTimer( xTimerForString, rtcdrvTimerTypePeriodic, (1000), shiftString, NULL);
    int32_t sliderPos;
    int32_t slider;
  /* Infinite loop */
  while (1) {
	  switch(currentState)
	      {
	  case BEFORE_START:
	  	  	  {
	  	  	  	  sliderPos= CAPLESENSE_getSliderPosition();
	  	  	  	  capSenseAringUpdate(sliderPos);
	  	  	  	  break;
	  	  	  }
	  case IN_GAME:
		  	  	  clearScreen();
		  	  	  displayBananas();
	  	  	  	  slider=CAPLESENSE_getSliderPosition();
	  	  	  	  capSenseBasketUpdate(slider);
	  	  	      capSenseAringUpdate(-1);
	  	  	  	  SegmentLCD_Number(totalBananas*100+caughtBananas);
	  	  	  	  SegmentLCD_Symbol (LCD_SYMBOL_COL10, 1);
	  	  	  	  break;
	  case GAME_OVER:
		  slider=CAPLESENSE_getSliderPosition();
		  capSenseAringUpdate(-1);
	  	  	  	  break;
	  case ERROR:
	  	  	  	  	  break;
	      }

  }
}
