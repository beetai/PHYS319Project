#!/usr/bin/python2.7
import serial # for serial port
import numpy as np # for arrays, numerical processing

#needs: python2, pyserial, numpy, 
#0) flash the serial temperature measurement program into the msp430
#1) start this program
#2) press the button the Launchpad to enter 'applcation mode'
#3) warm the chip (eg with a light bulb or your fingers)
#4) when you've seen enough, press the reset button on the launchpad
#5) exit the program by pressing 'q' or clicking on the x

#define the serial port. Pick one:
port = "/dev/ttyACM0"  #for Linux
#port = "COM5" #For Windows?
#port = "/dev/tty.uart-XXXX" #For Mac?


#start our program proper:
#open the serial port
try:
    # It seems that sometimes the port doesn't work unless 
    # you open it first with one speed, then change it to the correct value
    ser = serial.Serial(port,9600,timeout = 0.050) 
    ser.baudrate=9600
# with timeout=0, read returns immediately, even if no data
except:
    print ("Opening serial port",port,"failed")
    print ("Edit program to point to the correct port.")
    print ("Hit enter to exit")
    raw_input()
    quit()

ser.flushInput()

# initialize matrix
HEIGHT = 10
WIDTH = 10
playmat = np.zeros((HEIGHT, WIDTH)) # todo: set these as constants
startX = 4
startY = 0
currX = startX
currY = startY
playmat[startY, startX] = 1

def printPlaymat():
    np.set_printoptions(threshold=np.nan)
    print(playmat)
    #for row in np.nditer(playmat):
    #    for ele in np.nditer(row):
    #        print ele,
    #    print ''
    
def updatePlaymat(xSpinVal):
    global currX, currY
    playmat[currY, currX] = 0
    if xSpinVal == 1 and currX < WIDTH - 1:
        currX += 1
    elif xSpinVal == 2 and currX > 0:
        currX -= 1

    # if ySpinVal == something and currY < LENGTH - 1:
    #     currY += 1
    # elif ySpinVal == something and currY > 0:
    #     currY -= 1
    
    playmat[currY, currX] = 1
    
printPlaymat()

while(1): #loop forever
    data = ser.read(1) # look for a character from serial port - will wait for up to 50ms (specified above in timeout)
    if len(data) > 0: #was there a byte to read?
        TXByte = ord(data)
        #print TXByte
        if TXByte != 0:
            updatePlaymat(TXByte)
            printPlaymat()
        #print currX


#def printPlaymat():
#    for row in np.nditer(playmat):
#        for ele in np.nditer(row):
#            print(ele)


