import subprocess, os, string, serial

def mousemove(x, y):
	subprocess.call(["xdotool", "mousemove_relative", "--", str(x), str(y)])


def processCommand(cmd):
	if cmd[0] == "click":
		subprocess.call(["xdotool", "click", "1"])
	elif cmd[0] == "clickderecho":
		subprocess.call(["xdotool", "click", "2"])
	elif cmd[0] == "raton":
		mousemove(cmd[1], cmd[2])
	elif cmd[0] == "tecla":
		subprocess.call(["xdotool", "key", cmd[1]])
	elif cmd[0] == "apretar":
		subprocess.call(["xdotool", "mousedown"])
	elif cmd[0] == "soltar":
		subprocess.call(["xdotool", "mouseup"])		


bluetoothSerial = serial.Serial("/dev/rfcomm1", baudrate=9600)
bluetoothSerial.write("Conectando...")

while(1):
	rcv = bluetoothSerial.readline()
	rcv=rcv[:-1]
	cmd = rcv.split(":")
	processCommand(cmd)
