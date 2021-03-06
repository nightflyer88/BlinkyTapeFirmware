# EDF-Afterburner 

Firmwareversion: 1.2
Hardware: BlinkyTape Board, Arduino pro Micro

Original Projekt von: http://www.mostfun.de/index.php/modellbau/flugzeuge-blogansicht/314-edf-afterburner2

Durch die Firmware kann das BlinkyTape und entsprechendem LED Ring mit max. 60LEDs als Nachbrenner für Impellermodelle genutzt werden. Für den Upload muss in PatternPaint einfach ein leeres Farbmuster gewählt werden, dies hat jedoch keinen Einfluss auf den Farbverlauf des Nachbrenners. Nach dem Upload kann man das BlinkyTape nicht mehr mit PatternPaint ansprechen und live Muster anzeigen. Um das ganze rückgängig zu machen, macht man einfach unter TOOLS > RESTORE FIRMWARE einen Firmware Reset.


Die EDF-Afterburner Firmware unterstütz ein normales PWM Servosignal, und sollte mit jedem RC-Hersteller kompatibel sein.


### ACHTUNG!! 

-> Beim Anschluss am Computer per USB darf das BlinkyTape auf keinen Fall mit einer anderen Spannungsquelle versorgt werden, es besteht sonst Kurzschlussgefahr !! 

-> Nur stabilisiertes BEC mit 5V verwenden und auf den Stromverbrauch achten. Bei voller Helligkeit werden pro LED ca. 60mA benötigt, bei 60 LEDs sind dies bereits 3.6A !!



### Anschluss & Betrieb

Anschluss Servo Signal:

	5V = Pluspol (rot) 5V 
	D9 = Servo Signal (weiss)
	GND = Minuspol (schwarz)


Anschluss LED Strip:

	RAW/5V = Pluspol (5V) 
	D13/D6 = LED Daten Signal (Di)
	GND = Minuspol (GND)



Der Farbverlauf ist proportional zur Motorposition:

```
Servoposition      Motorposition
+100%  +---+       +100%  |                           /
       |   |              |                         /
       |   |              |                       0     Pos4 90%
       |   |              |                     /
       |   |              |                   /
       |   |              |                 0     Pos3 70%
       |   |              |               /
   0%  +---+              |             /
       |   |              |           0     Pos2 40%
       |   |              |         /
       |   |              |       /
       |   |              |     0     Pos1 10%
       |   |              |   /
       |   |              | /
-100%  +---+          0%  +------------------------------ Farbspektrum
                              |     |     |     |     |
                             AUS   rot  orange blau  weis  
 
```



Releasnotes:

 V1.2     16.04.17
 - LED Pin für Arduino pro Micro definiert


 V1.1     04.02.17
 - LED Anzahl von 24 auf 60 angepasst


 V1.0     26.12.16
 - Original Code übernommen und für BlinkyTape Board angepasst
 - PinChangeInterrupt angepasst
