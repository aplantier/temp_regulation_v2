



void doRegulation(measurement *mes){

   float CSH=0;// curent trishold HIGH
   float CSL=0;// current treshold LOW 
 
 if(mes->photo_value<mes->photo_tresh){// set of the treshold 
     mes->day_=false;
     CSH=mes->tem_tresh_high_night;
     CSL=mes->tem_tresh_low_night;
  }
  else{
     mes->day_=true;
     CSH=mes->tem_tresh_high_day;
     CSL=mes->tem_tresh_low_day;
  }
   if(mes->tem_value > CSH){
      
      if(mes->tem_value>(CSH+mes->tem_tresh_veryhigh)){
         mes->clim_=100; 
      }
      else{
         mes->clim_=50; 
      }
   }
   else {
      mes->clim_=0; 
   }
   if(mes->tem_value < CSL){
      mes->heat_=1; 
      
   }
   else{
      mes->heat_=0; 
   }

}
/**
 * Parsing of the comand sent by serial.
 * A command is {command char}{separator:':'}{value}
 * Data is the full string command with a separator and the index of the part of the string needed
 *
 */
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
/**
 * Handling the reception of commands by serial 
 * to enable the manual mode, the m:1 must be send first, if not, no command will be interpreted
 * then the comands are c:X for clim ( x in [0;100] the % rotation of the ventilo and h:Y for the heater with Y [0;1]
 *
 *
 */
void doComand(String com, measurement * mes){
   String command=getValue(com, ':',0); 
   String value=getValue(com, ':',1); 

   if(command=="m"){
      MODE=value.toInt();
   
   }
   if(MODE==1)
      return;
   if(command=="h")
     mes->heat_=value.toInt();
   if(command=="c")
     mes->clim_=value.toInt();


}

/** 
 * return 0 : no fire suspected 
 * return 1 : fire suspected 
 * return 2 : fire 
 */
int detectFire( float temperature , float *prevMesint, int numPrevMes, float delta, float fire_treshold ){
   float tmp_measure=0.0;  
   for(int i=0 ; i < numPrevMes; i++){
      Serial.print("[");
      Serial.print(prevMesint[i]);
      Serial.print("]");
   }

      Serial.print("\n");
   for(int i =0 ; i < numPrevMes-1 ; i++ ){
      prevMesint[i]=prevMesint[i+1];
  }

   prevMesint[numPrevMes-1]=temperature;
  if( temperature >= fire_treshold)
     return 2 ; 
  if ( prevMesint[0]== 0 || prevMesint[1]== 0 ||prevMesint[2]== 0 )// if no previous 3 mesure, no fire asimption 
     return 0 ; 
  if(!( prevMesint[0]<prevMesint[1] && prevMesint[1]<prevMesint[2])) 
     return 0 ; 
  if(  (prevMesint[1] -  prevMesint[0])/  (prevMesint[2]- prevMesint[1])<delta)
     return 1;
  return 0;



}




void blinkInternalLed(){
   Serial.println("BLINK ! ") ; 
if( internalLedState)
{
   digitalWrite(LED_BUILTIN,HIGH);
}
else {
   digitalWrite(LED_BUILTIN,LOW);
}
internalLedState=!internalLedState;

}
