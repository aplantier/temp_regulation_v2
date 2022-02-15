/** 
 * Fonction for mqtt mecanism
 */

void inputCommandOnTopic(JsonObject JObj);
void inputBatimentTopic(JsonObject JObj);
void mqtt_pubcallback( char * topic, byte * message , unsigned int length ){
   Serial.print("[topic: ");
   Serial.print(topic);
   Serial.println("] : ");
   String messageTemp ;
   for(int i=0; i < length ; i ++) {
   //   Serial.print( ( char ) message [ i ] ) ;
      messageTemp += (char) message [ i ] ;
}

   StaticJsonDocument<200> JSONDoc ; 
   auto error = deserializeJson(JSONDoc, (char *) message);
   if(error){
      Serial.println(" Deserialisation failed on input json command");
      Serial.println(error.c_str());
      return;
   }
   
   if( strcmp(topic,curMes.commandTopic)==0)
   {
      inputCommandOnTopic(JSONDoc.as<JsonObject>()); 
      Serial.println(" MAIN TOPIC");

   }
   if( strcmp(topic,curMes.batTopic)==0)
   {
      inputBatimentTopic(JSONDoc.as<JsonObject>()); 
      
      Serial.println(" bat TOPIC");
   }
Serial.println();
}
void mqtt_mysubscribe(char ** topicList , int nbrTopic){
   while(!client.connected()){// Loop u n t i l we ' r er e c o n n e c t e d
   Serial.print( " Attempting ␣MQTT␣ c o n n e c t i o n . . . " ) ;
   // Attempt t o c o n n e c t => h t t p s : / / p u b s u b c l i e n t . k n o l l e a r y . n e t / a p i
   if( mqtt_client.connect( "esp32" ,NULL,NULL) ) {
      Serial.println("connected");
      // then S u b s c r i b e t o p i c
      for ( int i =0 ; i < nbrTopic; i++ ){
         Serial.print(" Subscribe to :");
         Serial.println(topicList[i]);

         mqtt_client.subscribe(topicList[i]);
      
      }

   }
   else{
      Serial.print(" f a i l e d , ␣ r c=") ;
      Serial.print (mqtt_client.state());
      Serial.println(" ␣ try ␣ again ␣ in ␣5␣ seconds") ;
      delay(5000) ; // W
   }
}
}

void inputCommandOnTopic(JsonObject JObj){
   Serial.println("Serializable Function ");
   char curentKey[128]="";
   char curentKeyValue[128]="";
   for (JsonPair keyValue : JObj) {
      strcpy(curentKey,keyValue.key().c_str());
      if(strcmp(curentKey,"fireMode")==0){
         
         strcpy(curentKeyValue,keyValue.value().as<char*>());
         if(strcmp(curentKeyValue,"on")==0){
            Serial.println("FIRE MODE ON  "); 
            MODE=3;
         }
         else if(strcmp(curentKeyValue,"off")==0){
            Serial.println("FIRE MODE OFF  "); 

            digitalWrite(LED_BUILTIN,LOW);
            MODE=1;
         }

      }
     Serial.println(keyValue.key().c_str());
   }
}
void publishstatusMQTT( measurement murMes, char * topic){ 
   StaticJsonDocument<258> ack_report; 
   ack_report["mac"]=curMes.mac;
   ack_report["location"]=String(curMes.fullTopic);
   char tempString [8] ;
   dtostrf( curMes.tem_value , 1 , 2 , tempString ) ;
   ack_report["temperature"]=String(tempString);
   char message_bufffer[258];
   strcpy(message_bufffer,curMes.fullTopic);
   switch(curMes.mode_){
      case 3:
         ack_report["mode"] = "fire";
         break;
      case 2:
         ack_report["mode"] = "fireDetect";
         break;
      case 1: 
         ack_report["mode"] = "auto";
         break;
      case 0: 
         ack_report["mode"] = "control";
         break;

   }
   Serial.print("ACK Publish ");
   Serial.print(topic);
   serializeJson(ack_report,message_bufffer);
   Serial.println(message_bufffer);
   mqtt_client.publish(topic,message_bufffer) ;

}


void publishtemperatureMQTT( measurement murMes, char * topic){ 

   StaticJsonDocument<258> temp_report; 
   char message_buffer[128];
   temp_report["who"]=curMes.mac;
  // char tempString [8] ;
  // dtostrf( curMes.tem_value , 1 , 2 , tempString ) ;
//   temp_report["temperature"]=String(tempString);
   temp_report["value"]=curMes.tem_value;
   Serial.print("temp Publish ");
   Serial.print(topic);
   serializeJson(temp_report,message_buffer);
   Serial.println(message_buffer);
   mqtt_client.publish(topic,message_buffer) ;
}


void publishLightMQTT( measurement murMes, char * topic){ 
   StaticJsonDocument<258> light_report; 
   char message_buffer[128];
   light_report["who"]=curMes.mac;
   char tempString [8] ;
   dtostrf( curMes.tem_value , 1 , 2 , tempString ) ;
   light_report["value"]=curMes.photo_value ;
   //light_report["luminosity"]=String(curMes.photo_value) ;
   Serial.print("lum Publish ");
   Serial.print(topic);
   serializeJson(light_report,message_buffer);
   Serial.println(message_buffer);
   mqtt_client.publish(topic,message_buffer) ;
}
void inputBatimentTopic(JsonObject JObj){
   char msg_mac [32]="";
   char msg_location [32]="";
   float msg_temperature =0.0;
   char msg_mode [32]="";

   char curentKey[128]="";
   char curentKeyValue[128]="";
   for (JsonPair keyValue : JObj) {
      strcpy(curentKey,keyValue.key().c_str());
      if(strcmp(curentKey,"mode")==0){
         strcpy(msg_mode,keyValue.value().as<char*>());
      }
      if(strcmp(curentKey,"mac")==0){
         strcpy(msg_mac,keyValue.value().as<char*>());
      } 
      if(strcmp(curentKey,"location")==0){
         strcpy(msg_location,keyValue.value().as<char*>());
      } 
      if(strcmp(curentKey,"temperature")==0){
         msg_temperature=keyValue.value().as<float>();
      } 

   }
   if( strcmp(msg_mac,curMes.mac.c_str()) != 0 ) // if the message is not our own 
   {
         Serial.println("MSG ON BAT DIFERENT FROM OWN ");
     if( curMes.mode_==2 && (strcmp(msg_mode,"fireDetect")==0 ||strcmp(msg_mode,"fire")==0))
     {
         Serial.println("STARTING FIRE MODE");
         fireCode =2 ;
         MODE=3;
         curMes.mode_=3;
     }
   }
}
