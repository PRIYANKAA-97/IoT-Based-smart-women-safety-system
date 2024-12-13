import machine 
button = machine.Pin(19, machine.Pin.IN, machine.Pin.PULL_UP) 
from machine import Pin, UART 
import utime, time 
gpsModule = UART(0, baudrate=9600, tx=Pin(0), rx=Pin(1)) 
iot= UART(0, baudrate=9600, tx=machine.Pin(12), 
rx=machine.Pin(13),timeout=2000) 
uart= UART(1, baudrate=9600, tx=machine.Pin(4), 
rx=machine.Pin(9),timeout=2000) 
print(gpsModule) 
buff = bytearray(255) 
TIMEOUT = False 
FIX_STATUS = False 
latitude = "" 
longitude = "" 
satellites = "" 
GPStime = "" 
panic=0 
from machine import ADC, Pin 
import machine, onewire, ds18x20 
ds_pin = machine.Pin(21) 
ds_sensor = ds18x20.DS18X20(onewire.OneWire(ds_pin)) 
roms = ds_sensor.scan() 
adc = ADC(Pin(26)) 
import time #library for Time delay 
from lcd_lib import LCD # import Header file LCD 
led = machine.Pin(22, machine.Pin.OUT) 
# Define the pin numbers for your LCD 
rs_pin = 3 
e_pin = 5 
d4_pin = 6 
d5_pin = 7 
d6_pin = 8 
d7_pin = 9 
lcd = LCD(rs_pin, e_pin, d4_pin, d5_pin, d6_pin, d7_pin) 
heart=0; 
heart_rate=0 
def getGPS(gpsModule): 
global FIX_STATUS, TIMEOUT, latitude, longitude, satellites, GPStime 
  
    timeout = time.time() + 8  
    while True: 
        gpsModule.readline() 
        buff = str(gpsModule.readline()) 
        parts = buff.split(',') 
     
        if (parts[0] == "b'$GPGGA" and len(parts) == 15): 
            if(parts[1] and parts[2] and parts[3] and parts[4] and parts[5] and 
parts[6] and parts[7]): 
                print(buff) 
                 
                latitude = convertToDegree(parts[2]) 
                if (parts[3] == 'S'): 
                    latitude = -latitude 
                longitude = convertToDegree(parts[4]) 
                if (parts[5] == 'W'): 
                    longitude = -longitude 
                satellites = parts[7] 
                GPStime = parts[1][0:2] + ":" + parts[1][2:4] + ":" + parts[1][4:6] 
                FIX_STATUS = True 
                break 
                 
        if (time.time() > timeout): 
            TIMEOUT = True 
            break 
        utime.sleep_ms(500) 
         
def convertToDegree(RawDegrees): 
 
    RawAsFloat = float(RawDegrees) 
    firstdigits = int(RawAsFloat/100)  
    nexttwodigits = RawAsFloat - float(firstdigits*100)  
     
    Converted = float(firstdigits + nexttwodigits/60.0) 
    Converted = '{0:.6f}'.format(Converted)  
    return str(Converted) 
def map_range(value, from_low, from_high, to_low, to_high): 
    
    # Ensure the value is within the original range 
    value = max(min(value, from_high), from_low) 

    # Map the value to the target range 
    mapped_value = (value - from_low) * (to_high - to_low) / (from_high - 
from_low) + to_low 
     
    return mapped_value 
def send_sms(): 
    uart.write(str.encode('AT'+'\r\n')) 
    time.sleep(1) 
    print("welcome") 
    uart.write(str.encode('ATE0'+'\r\n')) # Disable the Echo 
    time.sleep(2) 
      
    uart.write(str.encode('AT+CMGF=1'+'\r\n')) # Select Message format as Text 
mode  
    time.sleep(2) 
      
    uart.write(str.encode('AT+CNMI=2,1,0,0,0'+'\r\n')) # New SMS Message 
Indications 
    time.sleep(2) 
 
      
    uart.write(str.encode('AT+CMGS="8946003304"'+'\r\n')) #mobile number 
receiver 
    time.sleep(2) 
      
    uart.write(str.encode('Emergency! Person is in danger, their Location below' 
+ '\n'+ 'latitude - '+ str(latitude) + 'longitude - '+ str(longitude) + '\n'+ 'heart rate - '+str(heart_rate) + 'Temperature -'+str(tempC) +'\n')) # Message 
    time.sleep(2) 
      
    uart.write(str.encode("\x1A")) # Enable to send SMS 
    time.sleep(2) 
 
    print ("scceass" ) 
 
while True: 
    lcd.clear() #clear LCD 
    lcd.write_message(" Smart Women ", 0, 0) # print String Here, Replace 
Double Quotes for String , After Comma set Cursor (Row,Column) 
    #print interger Here, if need to print float lcd.write_float() 
    lcd.write_message(" Safety System ",1, 0) 
    time.sleep(3)#delay 3 seconds 

    for i in range(10): 
        lcd.clear() #clear LCD 
        lcd.write_message(" Please Place ", 0, 0) # print String Here, Replace 
Double Quotes for String , After Comma set Cursor (Row,Column) 
        #print interger Here, if need to print float lcd.write_float() 
        lcd.write_message(" Your Finger ",1, 0) 
        led.on() 
        time.sleep(0.5) 
        heart=adc.read_u16() 
        led.off() 
        time.sleep(0.5) 
        print(heart) 
        if heart>700: 
            for i in range(10): 
                lcd.clear() #clear LCD 
                lcd.write_message(" Heart rate ", 0, 0) # print String Here, Replace 
Double Quotes for String , After Comma set Cursor (Row,Column) 
                #print interger Here, if need to print float lcd.write_float() 
                lcd.write_message(" Calibrating ",1, 0) 
                led.on() 
                time.sleep(0.1) 
                heart=adc.read_u16() 
                heart_rate=map_range(heart,800,2000,60,120) 
                led.off() 
                time.sleep(0.1) 
        else: 
            heart_rate=0; 
        if heart_rate > 0: 
            break 
    print(heart_rate) 
    lcd.clear() #clear LCD 
    lcd.write_message(" Heart Rate ", 0, 0) # print String Here, Replace Double 
Quotes for String , After Comma set Cursor (Row,Column) 
    #print interger Here, if need to print float lcd.write_float() 
    lcd.write_number(heart_rate,1, 4) 
    lcd.write_message("BPM",1,9); 
    time.sleep(3.0) 
    ds_sensor.convert_temp() 
    time.sleep_ms(750) 
    for rom in roms: 
        print(rom) 
        tempC = ds_sensor.read_temp(rom) 

        tempF = tempC * (9/5) +32 
        print('temperature (ºC):', "{:.2f}".format(tempC)) 
        lcd.write_message(" Temperature ",0,0) 
        lcd.write_float(tempC,1,4); 
        lcd.write_message(" *C",1,8) 
    time.sleep(2.0) 
    for i in range(4): 
        getGPS(gpsModule) 
        print("Printing GPS data...") 
        print(" ") 
        print("Latitude: "+latitude) 
        print("Longitude: "+longitude) 
        if latitude or longitude: 
            break 
        else: 
            #10.880332680707173, 77.0222825641159811 
            latitude="10.880332680707173" 
            longitude="77.02228256411598" 
    lcd.clear() 
    lcd.write_message("Lat - ",0,0) 
    lcd.write_message(latitude,0,5) 
    lcd.write_message("Lon - ",1,0) 
    lcd.write_message(longitude,1,5) 
    time.sleep(2.0) 
    if button.value()==0: #condition for button pressed or not 
        print("Button pressed") 
        panic=1 
        send_sms() 
    else: 
        print("Not pressed") 
        panic=0 
    final=str(heart_rate)+':'+str(tempC)+':'+str(panic)+':'+latitude+':'+longitude+'\
 n' 
    iot.write(final) 
    print(final) 
 
LCD lib code : 
 
# lcd_lib.py 
 
import utime 
import time 

 
 
from machine import Pin 
 
class LCD: 
    def __init__(self, rs_pin, e_pin, d4_pin, d5_pin, d6_pin, d7_pin): 
        self.rs = Pin(rs_pin, Pin.OUT) 
        self.e = Pin(e_pin, Pin.OUT) 
        self.d4 = Pin(d4_pin, Pin.OUT) 
        self.d5 = Pin(d5_pin, Pin.OUT) 
        self.d6 = Pin(d6_pin, Pin.OUT) 
        self.d7 = Pin(d7_pin, Pin.OUT) 
 
        self.rs.value(0) 
        self.send2LCD4(0b0011) # 8 bit 
        self.send2LCD4(0b0011) # 8 bit 
        self.send2LCD4(0b0011) # 8 bit 
        self.send2LCD4(0b0010) # 4 bit 
        self.send2LCD8(0b00101000) # 4 bit, 2 lines, 5x8 dots 
        self.send2LCD8(0b00001100) # LCD on, blink off, cursor off 
        self.send2LCD8(0b00000110) # Increment cursor, no display shift 
        self.send2LCD8(0b00000001) # Clear screen 
        utime.sleep_ms(2) # Clear screen needs a long delay 
 
    def pulseE(self): 
        self.e.value(1) 
        utime.sleep_us(40) 
        self.e.value(0) 
        utime.sleep_us(40) 
 
    def send2LCD4(self, BinNum): 
        self.d4.value((BinNum & 0b00000001) >> 0) 
        self.d5.value((BinNum & 0b00000010) >> 1) 
        self.d6.value((BinNum & 0b00000100) >> 2) 
        self.d7.value((BinNum & 0b00001000) >> 3) 
        self.pulseE() 
 
    def send2LCD8(self, BinNum): 
        self.d4.value((BinNum & 0b00010000) >> 4) 
        self.d5.value((BinNum & 0b00100000) >> 5) 
        self.d6.value((BinNum & 0b01000000) >> 6) 
        self.d7.value((BinNum & 0b10000000) >> 7) 
        self.pulseE() 
        self.d4.value((BinNum & 0b00000001) >> 0) 

        self.d5.value((BinNum & 0b00000010) >> 1) 
        self.d6.value((BinNum & 0b00000100) >> 2) 
        self.d7.value((BinNum & 0b00001000) >> 3) 
        self.pulseE() 
 
    def lcd_string(self, data): 
        for i in range(len(data)): 
            self.send2LCD8(ord(data[i])) 
 
    def clear(self): 
        self.rs.value(0) 
        self.send2LCD8(0b00000001) 
        utime.sleep_ms(2) 
 
    def set_cursor(self, row, col): 
        self.rs.value(0) 
        if row == 0: 
            self.send2LCD8(0x80 + col) 
        elif row == 1: 
            self.send2LCD8(0xC0 + col) 
        utime.sleep_us(100) 
 
    def write_message(self, message, row, col): 
        self.set_cursor(row, col) 
        self.rs.value(1) 
        self.lcd_string(message) 
 
    def scroll_message(self, message, row, delay=0.5): 
        self.set_cursor(row, 0) 
        self.rs.value(1) 
        self.lcd_string(message) 
        time.sleep(delay) 
        for i in range(len(message)): 
            self.send2LCD8(0b00011000) # Shift the display right 
            time.sleep(delay) 
    def clear_line(self, row): 
        self.set_cursor(row, 0) 
        self.rs.value(1) 
        self.lcd_string(" " * 16) # Clear the entire line 
 
    def write_number(self, number, row, col): 
        self.clear_line(row) 
        self.set_cursor(row, col) 
        self.rs.value(1) 
        self.lcd_string(str(number)) 
 
    def write_float(self, number, row, col, decimal_places=2): 
        self.clear_line(row) 
        self.set_cursor(row, col) 
        self.rs.value(1) 
        format_str = "{:.{}f}".format(number, decimal_places) 
        self.lcd_string(format_str) 
                               
 
IOT code ESP8266 : 
 
#include "Ubidots.h" 
    const char* UBIDOTS_TOKEN = "BBUS
oq95f5UJpBIfWkXwj2YGADtTpTULBV"; // Put here your Ubidots TOKEN 
    const char* WIFI_SSID = "blitztech academy_4G"; // Put here your Wi-Fi 
SSID 
    const char* WIFI_PASS = "Academy@12345"; // Put here your Wi-Fi 
password 
    Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP); 
void setup() { 
  Serial.begin(9600); 
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS); 
} 
 
void loop() { 
  if(Serial.available()){ 
    String myString=Serial.readStringUntil('\n'); 
    String val1=getValue(myString, ':', 0); 
    String val2=getValue(myString, ':', 1); 
    String val3=getValue(myString, ':', 2); 
    String val4=getValue(myString, ':', 3); 
    String val5=getValue(myString, ':', 4); 
    ubidots.add("HeartRate", val1.toInt()); 
    ubidots.add("Temperature", val2.toFloat()); 
    ubidots.add("panic", val3.toInt()); 
    ubidots.add("Latitude", val4.toFloat()); 
    ubidots.add("Longitude", val5.toFloat()); 
    bool bufferSent = false; 
 bufferSent = ubidots.send(); // Will send data to a device label that matches 
the device Id 
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
 
