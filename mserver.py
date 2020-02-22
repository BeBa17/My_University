import socket
import sys
import sqlite3
import datetime
import select
import threading
from tkinter import*

regulate_temp_var = 21

def send_data():
    global regulate_temp_var
    regulate_temp_var = float(regulate_entry.get())

	#prev_text = regulate_entry.get()
	#connection.sendall(prev_text.encode())
    print('wysylam nowa temp regulacji', regulate_temp_var, file=sys.stderr)

top = Tk()

top.title("SmartVents Control Panel")

curr_temp = Label(top, text="Aktualna", relief = GROOVE, width = 24)
curr_temp_val = Label(top, text="0")

max_temp_td = Label(top, text="Maksymalna dzisiaj", relief = GROOVE, width = 24)
max_temp_td_val = Label(top, text="0")

min_temp_td = Label(top, text="Minimalna dzisiaj", relief = GROOVE, width = 24)
min_temp_td_val = Label(top, text="0")

regulate = Label(top, text="Regulacja", relief = GROOVE, width = 24)
regulate_entry = Entry(top)
send_button = Button(top, text="Zmien", command=send_data)

min_temp_week = Label(top, text="Minimalna tydzien", relief = GROOVE, width = 24)
min_temp_week_val = Label(top, text="0")

max_temp_week = Label(top, text="Maksymalna tydzien", relief = GROOVE, width = 24)
max_temp_week_val = Label(top, text="0")

avg_temp_week = Label(top, text="Srednia tydzien", relief = GROOVE, width = 24)
avg_temp_week_val = Label(top, text="0")

min_temp_month = Label(top, text="Minimalna miesiac", relief = GROOVE, width = 24)
min_temp_month_val = Label(top, text="0")

max_temp_month = Label(top, text="Maksymalna miesiac", relief = GROOVE, width = 24)
max_temp_month_val = Label(top, text="0")

avg_temp_month = Label(top, text="Srednia miesiac", relief = GROOVE, width = 24)
avg_temp_month_val = Label(top, text="0")


Label(top, text="Temperatura").grid(column = 2)

curr_temp.grid(row = 2, column = 1)
curr_temp_val.grid(row = 2, column = 2)

max_temp_td.grid(row = 3, column = 1)
max_temp_td_val.grid(row = 3, column = 2)

min_temp_td.grid(row = 4, column = 1)
min_temp_td_val.grid(row = 4, column = 2)

regulate.grid(row = 5, column = 1)
regulate_entry.grid(row = 5, column = 2)
send_button.grid(row = 5, column = 3)

min_temp_week.grid(row = 6, column = 1)
min_temp_week_val.grid(row = 6, column = 2)

avg_temp_week.grid(row = 7, column = 1)
avg_temp_week_val.grid(row = 7, column = 2)

max_temp_week.grid(row = 8, column = 1)
max_temp_week_val.grid(row = 8, column = 2)

min_temp_month.grid(row = 9, column = 1)
min_temp_month_val.grid(row = 9, column = 2)

avg_temp_month.grid(row = 10, column = 1)
avg_temp_month_val.grid(row = 10, column = 2)

max_temp_month.grid(row = 11, column = 1)
max_temp_month_val.grid(row = 11, column = 2)


# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Bind the socket to the port
server_address = ('192.168.43.2', 10000)
print('starting up on', server_address[0], 'port', server_address[1], file=sys.stderr)
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
        cursor.execute(""" select max(wartosc) from temp_res where strftime('%d', data) = strftime('%d', 'now')""")
    elif number==0:
        cursor.execute(""" select avg(wartosc) from temp_res where strftime('%d', data) = strftime('%d', 'now')""")
    elif number==-1:
        cursor.execute(""" select min(wartosc) from temp_res where strftime('%d', data) = strftime('%d', 'now')""")

    rows = cursor.fetchall()
    return round(rows[0][0], 2)
        
def month_results(number):
    if number==1:
        cursor.execute(""" select max(wartosc) from temp_res where strftime('%m', data) = strftime('%m', 'now')""")
    elif number==0:
        cursor.execute(""" select avg(wartosc) from temp_res where strftime('%m', data) = strftime('%m', 'now')""")
    elif number==-1:
        cursor.execute(""" select min(wartosc) from temp_res where strftime('%m', data) = strftime('%m', 'now')""")

    rows = cursor.fetchall()
    return round(rows[0][0], 2)
        
def week_results(number):
    if number==1:
        cursor.execute(""" select max(wartosc) from temp_res where strftime('%W', data) = strftime('%W', 'now')""")
    elif number==0:
        cursor.execute(""" select avg(wartosc) from temp_res where strftime('%W', data) = strftime('%W', 'now')""")
    elif number==-1:
        cursor.execute(""" select min(wartosc) from temp_res where strftime('%W', data) = strftime('%W', 'now')""")

    rows = cursor.fetchall()
    return round(rows[0][0], 2)



# Wait for a connection
print('waiting for a connection', file=sys.stderr)
print(week_results(1))
connection, client_address = sock.accept()

print('connection from', client_address, file=sys.stderr)

def myupdate(data):
	if data:
		curr_temp_val.config(text=data)
	max_temp_td_val.config(text=day_results(1))
	min_temp_td_val.config(text=day_results(-1))
	min_temp_week_val.config(text=week_results(-1))
	max_temp_week_val.config(text=week_results(1))
	avg_temp_week_val.config(text=week_results(0))
	min_temp_month_val.config(text=month_results(-1))
	max_temp_month_val.config(text=month_results(1))
	avg_temp_month_val.config(text=month_results(0))

def servloop():
	readable, writable, _ = select.select([connection], [], [])
	data = ""
	if readable != []:
		mlength = connection.recv(1)
		mlength = mlength.decode()
		data = connection.recv(int(mlength))
		data = data.decode()
		connection.sendall(str(regulate_temp_var).encode())
		print('wysylam', regulate_temp_var)
	if data:
		print('received', data, file=sys.stderr)
		#print('adding result to database', file=sys.stderr)
		add_temp_result(datetime.datetime.now(), data)
		top.after(1, myupdate, data)
	top.after(20, servloop)

top.after(1, servloop)

top.mainloop()
