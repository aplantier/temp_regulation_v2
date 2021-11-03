
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
