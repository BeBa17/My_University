import Tkinter as TK


def printMy():
    print(E1.get())


top = TK.Tk()
L1 = TK.Label(top, text="Temperatura")
L1.pack()

E1 = TK.Entry(top)
E1.pack()

L2 = TK.Label(top, text="Najwyzsza dzisiaj: 22")
L2.pack()

B1 = TK.Button(top, text="Send", command=printMy)
B1.pack()

top.mainloop()
