#ifndef buttonb_h
#define buttonb_h
#include "Arduino.h"

/*
 * Code keys v1 - 2022/8/8 Saul C.
 
  */
  
class Buttonb{
  public: 
  
     Buttonb(){ 
     }
    ~Buttonb(){}
      
    int tap(){
       long t ;
      if(BOOTSEL){
      lastClick++; 
      } else {
        t = lastClick;
        lastClick = 0;
      if( t > longDelay){
        return 2;   
      }
      else if( t > clickDelay){ 
        return 1;
        }  
      }
      return lastClick;
      
    } 
    
  private:
    int pin = 0;
    long lastClick = 0;  
    int clickDelay = 500;
    int longDelay = 3333;
   
};    
#endif
