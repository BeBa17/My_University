from tkinter import*

def send_data():
	prev_text = regulate_entry.get()
	# dodac wysylanie do klienta ktory sie polaczyl

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

min_temp_ever = Label(top, text="Minimalna", relief = GROOVE, width = 24)
min_temp_ever_val = Label(top, text="0")

max_temp_ever = Label(top, text="Maksymalna", relief = GROOVE, width = 24)
max_temp_ever_val = Label(top, text="0")

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

min_temp_ever.grid(row = 12, column = 1)
min_temp_ever_val.grid(row = 12, column = 2)

max_temp_ever.grid(row = 13, column = 1)
max_temp_ever_val.grid(row = 13, column = 2)

# dodac top.after(5, funkcja) (co 5ms bedzie sie wywolywac funkcja)
# funkcja powinna odbierac dane ustawiac wszystkie wazne pola i wywolywac top.after na koncu
top.mainloop()
