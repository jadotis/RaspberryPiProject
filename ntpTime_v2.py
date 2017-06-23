import ntplib
import time
from time import ctime
import datetime
import lcd_16x2 as lcd
import signal
import sys
import servo as SERVO

class Clock:
    def __init__(self, lcd_m, serv_m, zone = "EST"):
        self.lcd_mode = lcd_m
        if type(self.lcd_mode) != int:
            self.lcd_mode.lower()
        self.servo_mode = serv_m
        self.ntp = ntplib.NTPClient()
        self.hours = 0
        self.minutes = 0
        self.seconds = 0
        self.servo = SERVO.ServoHand()
        lcd.lcd_init()
        self.tzone = zone.lower()
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.signal(signal.SIGTSTP, self.signal_handler)
        

    def getTimeString(self):
        response = None
        try:
            response = self.ntp.request('1.us.pool.ntp.org', version=3)
        except Exception:
            print('NTP Exception Raised and Caught')
            return None
        
        return ctime(response.tx_time)


    def parseTimeString(self, time):
        hours = int(time[10:13])
        minutes = int(time[14:16])
        seconds = int(time[17:19])
        if self.tzone != "est":
            if self.tzone == "pst":
                hours -= 3
            elif self.tzone == "cst":
                hours -= 1
            elif self.tzone == "mst":
                hours -= 2
            else:
                print(str(self.tzone) + ": TIme zone not yet implemented")
                sys.exit(0)
        if hours > 12:
            hours -= 12
        elif hours <= 0:
            hours = 12 + hours
        return hours, minutes, seconds


    def update_time(self):
        flag = False
        x = self.getTimeString()
        if x == None:
            y = str(datetime.datetime.now().time())
            self.hours = int(y[0:2])
            self.minutes = int(y[3:5])
            self.seconds = int(y[6:8])
            if self.hours > 12:
                self.hours -= 12
            flag = True
        else:
            h, m, s = self.parseTimeString(x)

            if h != self.hours:
                flag = True
                self.hours = h

            if m != self.minutes:
                flag = True
                self.minutes = m

            if s != self.seconds:
                flag = True
                self.seconds = s

        return flag
    

    def display(self):
        lcd.lcd_byte(lcd.LCD_LINE_1, lcd.LCD_CMD)

        if self.lcd_mode == 0:
            lcd.lcd_string('Time is ' + str(self.hours) + ':' + str(self.minutes) +
                           ':' + str(self.seconds), 2)
            print('Time is ' + str(self.hours) + ':' + str(self.minutes) + ':'
                  + str(self.seconds))
            
        elif self.lcd_mode == 1 or self.lcd_mode == "hours":
            lcd.lcd_string('Hours: ' + str(self.hours), 2)
            print ('Hours: ' + str(self.hours))
            
        elif self.lcd_mode == 2 or self.lcd_mode == "minutes":
            lcd.lcd_string('Minutes: ' + str(self.minutes), 2)
            print ('Minutes: ' + str(self.minutes))

        elif self.lcd_mode == 3 or self.lcd_mode == "seconds":
            lcd.lcd_string('Seconds: ' + str(self.seconds), 2)
            print ('Seconds: ' + str(self.seconds))
        lcd.lcd_byte(lcd.LCD_LINE_2, lcd.LCD_CMD)
        lcd.lcd_string("Timezone: " + self.tzone.upper(), lcd.LCD_LINE_2)
    
    def spinClock(self):
        if self.servo_mode == 0 or self.servo_mode == 'seconds':
            if self.seconds == 0 and self.servo.time != 0:
                for i in range(60 - self.servo.time):
                    self.servo.spinMin()
                    self.servo.p_width += 1
                    
            else:
                for i in range(self.seconds - self.servo.time):
                    self.servo.spinMin()
                    self.servo.p_width += 1

            self.servo.time = self.seconds

        elif self.servo_mode == 1 or self.servo_mode == 'minutes':
            if self.minutes == 0 and self.servo.time != 0:
                for i in range(60 - self.servo.time):
                    self.servo.spinMin()
                    self.servo.p_width += 1
            else:
                for i in range(self.minutes - self.servo.time):
                    self.servo.spinMin()
                    self.servo.p_width += 1

            self.servo.time = self.minutes
        
        else:
            for i in range(self.hours - self.servo.time):
                self.servo.spinHour()
                self.servo.p_width += 4
                
            self.servo.time = self.hours

        self.servo.resetPulse()
    def signal_handler(self, signal, frame):
        print ('SIGINT caught: cleaning GPIOs.')
        lcd.lcd_string('                ', 2)
        lcd.lcd_string('                ', lcd.LCD_LINE_2)
        if self.servo_mode == 0 or self.servo_mode == 'seconds' or self.servo_mode == 1 or self.servo_mode == 'minutes':
            for i in range(60 - self.servo.time):
                self.servo.spinMin()
                self.servo.p_width += 1
        else:
            for i in range(12 - self.servo.time):
                self.servo.spinHour()
                self.servo.p_width += 4
        sys.exit(0)

