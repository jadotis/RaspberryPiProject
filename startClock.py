import time
import ntpTime_v2 as NTP
import sys

args = sys.argv
clock = NTP.Clock(0, args[1], args[2])
while(True):
    x = clock.update_time()
    if x == True:
        clock.display()
        clock.spinClock()
        
    time.sleep(1)
