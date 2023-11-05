import pyxel
import serial

ser = serial.Serial('/dev/cu.usbmodem1101', 9600)

class App:
    def __init__(self):
        pyxel.init(width=160, height=120, title="Trash Tiger", fps=15)
        pyxel.run(self.update, self.draw)
        self.received = ""
        self.payload = ""
        self.alive = False

    def update(self) -> None:
        self.payload = ""
        if pyxel.btn(pyxel.KEY_W):
            self.payload += "W"
        if pyxel.btn(pyxel.KEY_A):
            self.payload += "A"
        if pyxel.btn(pyxel.KEY_S):
            self.payload += "S"
        if pyxel.btn(pyxel.KEY_D):
            self.payload += "D"
        if pyxel.btn(pyxel.KEY_K):
            self.payload += "K"
        if pyxel.btn(pyxel.KEY_L):
            self.payload += "L"
        self.payload += "E" # E for the end of the payload
        ser.write(self.payload.encode())
        
        self.received = ""
        while ser.in_waiting > 0:
            self.received += ser.readline().decode()
        # print(self.received)
        self.alive = ("E" in self.received)


    def draw(self) -> None:
        pyxel.cls(0)
        pyxel.text(0, 0, f"Trash Tiger! Alive {self.alive}", pyxel.frame_count % 16)
        pyxel.text(0, 10, f"Sending: {self.payload}", pyxel.frame_count % 16)
        pyxel.text(0, 20, self.received, pyxel.frame_count % 16)


game = App()