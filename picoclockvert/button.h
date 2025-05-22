#ifndef button_h
#define button_h
#include "Arduino.h"
//#include "PinChangeInterrupt.h"
/*
 * Code keys v1 - 2022/8/8 Saul C.
 * button dobounce + double tap (needs work)
 * external and pinchange interupt pints only.
 * nano/micro only. not working for esp32. (tap timer only..)
  */
  
class Button{
  public: 
  
     Button(int p , bool isinterupt){
      pin = p;
//      clickCallback = callback; 
      isInterupt = isinterupt;
     }
    ~Button(){}
    
    void ini(){
      pinMode(pin, INPUT_PULLUP); 
      // pinMode(pin, INPUT_PULLDOWN); 
      //no press n hold, alternate version for 'gaming response'
//      if(isInterupt) attachInterrupt(0, clickCallback, FALLING );  
//      else attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(pin), clickCallback, FALLING); 
      }
     int getPin(){ 
        return pin;
     }
      
    //basic debounce//maybe add a double tap?
    //double tap still triggers single tap, 
    //solutions, wait for it or undo it. 
//    for now use 'non conflicting' double taps.
    int tap(){
      long long temp  = millis() - lastClick; 
      if(temp  > clickDelay){ 
        lastClick = millis();   
          // if(temp > clickDelay & temp  < doubleTapDelay)  return 2;
        
        return 1;
      }
      return 0; 
    } 
    
  private:
    int pin = 0;
    long lastClick = 0; // for 3 interupt micros.
    long clickDelay = 500;  //reduce for "gaming" response, false/extra key taps ok.
    bool isInterupt;    //3 buttons isn't enough.....this may not work
//    void   *clickCallback; //just need to save a pointer and the int lib will do the rest
    long doubleTapDelay = 3000;  
};    
#endif
