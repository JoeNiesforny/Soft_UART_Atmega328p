import serial
import time

test_pattern = 'AABBCCDDEEFFGGHHIIJJKKLLMMNNOOPPQQRRSSTT\0'
#test_pattern = 'ABCDE'

s = serial.Serial(port='/dev/ttyS0')

for i in range(0,100):
    s.write(test_pattern)
    time.sleep(0.1)
    p = s.read(s.in_waiting)
    print str(i) + ": p = " + str(p)
    if test_pattern != str(p):
        print "######Wrong Pattern#######"
        break
