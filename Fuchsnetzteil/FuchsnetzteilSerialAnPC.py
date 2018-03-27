

import serial
import matplotlib.pyplot as plt


temp = 0
amp = 0
volt = 0
fan = 0
watt = 0

plot_temp = []
plot_amp = []
plot_volt = []
plot_fan = []
plot_watt =[]
#while True:
adresse = input("Welche ttyUSB adresse hat das gerät?: ")
print('1 Volt ')
print('2 Ampere')
print('3 Watt')
print('4 Temparatur')
print('5 Lüfter zustand')
anzeigen = input("Was Davon möchtest du sehen? ")

dauer = int(input("Wie lange willst du aufzeichnen? "))

for x in range(dauer):
    ser = serial.Serial(port='/dev/ttyUSB' + adresse ,baudrate=115200)
    a = ser.readline().decode("utf-8").strip()
    b = a.split("/")

    for c in b:
        d = c.split("@")
        if d[0] == "temp":
            temp = d[1]
            plot_temp.append(temp)
        if d[0] == "amp":
            amp = d[1]
            plot_amp.append(amp)
        if d[0] == "volt":
            volt = d[1]
            plot_volt.append(volt)
        if d[0] == "fan":
            fan = d[1]
            plot_fan.append(fan)
        watt = float(volt) * float(amp)
        plot_watt.append(watt)
    print("Temparatur",temp,
        " Ampere",amp,
        " Volt",volt,
        " Watt",round(watt,2),
        " Lüfter",fan)
for auswahl in anzeigen:
    print(auswahl)
    if auswahl == '1':
        plt.plot(plot_volt)
    if auswahl == '2':
        plt.plot(plot_amp)
    if auswahl == '3':
        plt.plot(plot_watt)
    if auswahl == '4':
        plt.plot(plot_temp)
    if auswahl == '5':
        plt.plot(plot_fan)

plt.show()

