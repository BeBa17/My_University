import socket
import sys
import sqlite3
import datetime

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Bind the socket to the port
server_address = ('192.168.1.2', 10000)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)

# Connecting to database
conn = sqlite3.connect('temp_db')
cursor = conn.cursor()
print("Connected to SQLite")

def add_temp_result (date_time, value):
    my_insert = """INSERT INTO 'temp_res' ('data', 'wartosc') VALUES (?, ?); """
    my_data = (date_time, value)

    cursor.execute(my_insert, my_data)
    conn.commit()

# number: for max -> 1, avg -> 0, min -> -1
def day_results(number):
    if number==1:
        cursor.execute(""" select max(wartosc) from temp_res where strftime('%d', data) = strftime('%d', 'now')""");
    elif number==0:
        cursor.execute(""" select avg(wartosc) from temp_res where strftime('%d', data) = strftime('%d', 'now')""");
    elif number==-1:
        cursor.execute(""" select min(wartosc) from temp_res where strftime('%d', data) = strftime('%d', 'now')""");

    rows = cursor.fetchall()
    for row in rows:
        print(row)
        
def month_results(number):
    if number==1:
        cursor.execute(""" select max(wartosc) from temp_res where strftime('%m', data) = strftime('%m', 'now')""");
    elif number==0:
        cursor.execute(""" select avg(wartosc) from temp_res where strftime('%m', data) = strftime('%m', 'now')""");
    elif number==-1:
        cursor.execute(""" select min(wartosc) from temp_res where strftime('%m', data) = strftime('%m', 'now')""");

    rows = cursor.fetchall()
    for row in rows:
        print(row)
        
def week_results(number):
    if number==1:
        cursor.execute(""" select max(wartosc) from temp_res where strftime('%W', data) = strftime('%W', 'now')""");
    elif number==0:
        cursor.execute(""" select avg(wartosc) from temp_res where strftime('%W', data) = strftime('%W', 'now')""");
    elif number==-1:
        cursor.execute(""" select min(wartosc) from temp_res where strftime('%W', data) = strftime('%W', 'now')""");

    rows = cursor.fetchall()
    for row in rows:
        print(row)

print("temperatura dzisiaj: max, avg, min : ")
day_results(1)
day_results(0)
day_results(-1)

print("temperatura w tygodniu: max, avg, min: ")
week_results(1)
week_results(0)
week_results(-1)

print("temperatura w miesiacu: max, avg, min: ")
month_results(1)
month_results(0)
month_results(-1)

while True:
    # Wait for a connection
    print >>sys.stderr, 'waiting for a connection'
    connection, client_address = sock.accept()

    try:
        print >>sys.stderr, 'connection from', client_address

        # Receive the data in small chunks and retransmit it
        while True:
            data = connection.recv(64)
            print >>sys.stderr, 'received "%s"' % data
            if data:
                print >>sys.stderr, 'sending data back to the client'
                add_temp_result(datetime.datetime.now(), data)
                connection.sendall(data)
            else:
                print >>sys.stderr, 'no more data from', client_address
                break
            
    finally:
        # Clean up the connection
        connection.close()
