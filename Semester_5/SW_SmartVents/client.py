import socket
import select
import sys
import time

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 10000)
print('connecting to', server_address[0], 'port', server_address[1], file=sys.stderr)
sock.connect(server_address)
num = "1"

try:
    
    # Send data
	while True:
		message = num
		num = str(int(num)+1)
		print('sending', message, file=sys.stderr)
		if not sock.send(message.encode()):
			print("nieudane")

		# Look for the response
		amount_received = 0
		amount_expected = len(message)
		
		while amount_received < amount_expected:
			data = sock.recv(16)
			amount_received += len(data)
			print('received', data.decode(), file=sys.stderr)

finally:
    print('closing socket', file=sys.stderr)
    sock.close()