import tkinter as tk
import time

class App():
    def __init__(self):
        self.mainWindow = tk.Tk()
        self.label = tk.Label(text="")
        self.label.pack()
        self.timedUpdateButtons()
        self.mainWindow.mainloop()

    def timedUpdateButtons(self):
        now = time.strftime("%H:%M:%S")
        self.label.configure(text=now)
        self.mainWindow.after(1000, self.timedUpdateButtons)

app=App()
