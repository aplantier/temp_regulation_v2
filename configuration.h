
/*
 * This was the first try to build a json, that might be imrpoved using the buildJson API 
 *
 */
String buildMeasure_JSON(measurement mes){
   String out_json="{";
  
   // mode :0 force , 1: regluation
   out_json+="\"mode\":";
   out_json+=String(mes.mode_);
   out_json+=",";
   // captor 
   out_json+="\"captor\":{";
      // temperature 
      out_json+="\"temperature\":{";
      //value
         out_json+="\"value\":";
         out_json+=String(mes.tem_value);
         out_json+=",";
         // treshold 
         out_json+="\"treshold\":{";
            
            out_json+="\"night\":{";
               out_json+="\"high\":";
               out_json+=String(mes.tem_tresh_high_night);
               out_json+=",";
               out_json+="\"low\":";
               out_json+=String(mes.tem_tresh_low_night);
            out_json+="},";
            
            out_json+="\"day\":{";
               out_json+="\"high\":";
               out_json+=String(mes.tem_tresh_high_day);
               out_json+=",";
               out_json+="\"low\":";
               out_json+=String(mes.tem_tresh_low_day);
            out_json+="}";
   
         out_json+="}";
      out_json+="},";
      out_json+="\"luminosity\":{";
      //value
         out_json+="\"value\":";
         out_json+=String(mes.photo_value);
         out_json+=",";
         // treshold 
         out_json+="\"treshold\":{";
            
            out_json+="\"day\":{";
               out_json+="\"high\":";
               out_json+=String(mes.photo_tresh);
            out_json+="}";
         out_json+="}";
      out_json+="}";
   out_json+="},";
   out_json+="\"actuator\":{";
      //value
      out_json+="\"heater\":";
      out_json+=String(mes.heat_);
      out_json+=",";
      out_json+="\"clim\":";
      out_json+=String(mes.clim_);
   out_json+="}";
out_json+="}";
return out_json;
}
/**
 * This function apply the configuration saved id the struct.
 * The main usage is to set the actuator status 
 * + Led ( heater & cooler ) 
 * + The fan speed 
 *
 */
void applyConfiguration(measurement mes){
   // heater
   if(mes.heat_>0){
      digitalWrite(pinHeater, HIGH);   // turn the LED on (HIGH is the voltage level)
   }
   else
      digitalWrite(pinHeater, LOW);   // turn the LED on (HIGH is the voltage level)

   // clim
   if(mes.clim_>0)
      digitalWrite(pinClim, HIGH);    // turn the LED off by making the voltage LOW
   else
      digitalWrite(pinClim, LOW);    // turn the LED off by making the voltage LOW
   ledcWrite(gpioVentilo,mes.clim_*255/100);
}


