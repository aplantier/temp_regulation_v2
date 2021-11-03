#F i c h i e r r e a d s e r i a l . py
import time
import serial
import json
from types import SimpleNamespace
ser = serial . Serial (
    port='/dev/ttyUSB0' ,
    baudrate = 9600,
    parity=serial.PARITY_NONE ,
    stopbits=serial.STOPBITS_ONE ,
    bytesize=serial.EIGHTBITS ,
    timeout=1 #h t t p s : / / p y t h o n h o s t e d . o r g / p y s e r i a l / s h o r t i n t r o . h t m l#r e a d l i n e
)
#>>>>>>>>>>>>> OBJECT <<<<<<<<<<<<<<<<<<

#>>>>>>>>>>>>> FUNCTIONS <<<<<<<<<<<<<<<<<<

def printMeasurement(js):
    print("::::::::::: Measure ::::::::::: ")
    print("mode : ", end='\r')
    if js.mode==1:
        print("Regulation")
    elif js.mode==0:
        print("manual")
    print("Captor")
    print("\tTemperature :%.3f" % js.captor.temperature.value)
    print("\tLuminosity :%d (%s)" %( js.captor.luminosity.value,"day" if js.captor.luminosity.value>js.captor.luminosity.treshold.day.high else "night"))
    print("Actuator")
    if js.actuator.clim == 0: 
        print("\tclim :off")
    else:
        print("\tclim :on (speed : %d %c) " % (js.actuator.clim,chr(37)))
    if js.actuator.heater== 0: 
        print("\theat :off")
    else:
        print("\theat :on")




    #print("\tLumens :{} {}" . format(value_json['photo']['val']) . format(value_json['photo']['unit']))
    #print("\tTemp : {} {}" . format(value_json['temp']['val']) . format(value_json['temp']['unit']))




#>>>>>>>>>>>> MAIN <<<<<<<<<<<<<<<<<<<<<<<



while True :
    try :
        x=  ser.readline ( ) # r e a dx =
        x=  x.rstrip( )
        x=  x.decode( "utf-8" )
        if x != "":
            print ( "Valeur : [{}]" . format ( x ) )
            value_dict= json.loads(format(x),object_hook=lambda d: SimpleNamespace(**d))
            printMeasurement(value_dict)
    except KeyboardInterrupt :
        print ( 'exiting' )
        break
#On e c r i t
#s e r . w r i t e ( ' 1 ' ); s e r . f l u s h ( )
# close serial
ser . flush ( )
ser . close ( )

