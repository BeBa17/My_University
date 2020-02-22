import sqlite3 as sql

db = sql.connect('temp_db')
c = db.cursor()

c.execute("""CREATE TABLE temp_res(data DATE, wartosc NUMBER(7,2))""")
