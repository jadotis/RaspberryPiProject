#This is the python script to turn the servo

import pigpio
import time
import RPi.GPIO as GPIO
import sys
import signal

class ServoHand:
  def __init__(self, time = 0.035, pulse = 1425):
    self.wait_t = time
    self.p_width = pulse
    self.cp_width = pulse
    self.time = 0
    signal.signal(signal.SIGINT, self.keyboard_interrupt)
    self.pi = pigpio.pi()    

  def spinMin(self):
    self.pi.set_servo_pulsewidth(18, self.p_width)
    time.sleep(self.wait_t) #Variable Time
    self.pi.set_servo_pulsewidth(18, 1505)

  def spinHour(self):
    for i in range(5):
      self.pi.set_servo_pulsewidth(18, self.p_width)
      time.sleep(self.wait_t) #Variable Time
      self.pi.set_servo_pulsewidth(18, 1505)

  def stop(self):
    self.pi.set_servo_pulsewidth(18, 1505)
    GPIO.cleanup()


  def resetPulse(self):
    self.p_width = self.cp_width

  def keyboard_interrupt(signal, frame):
    self.pi.set_servo_pulsewidth(18, 1505)
    GPIO.cleanup()
