# BlinkyTape Firmware für RC Modelle

Firmwareversion: 1.1.1
Hardware: BlinkyTape Board


Die Firmware unterstützt ein normales PWM Servosignal, und sollte mit jedem RC-Hersteller kompatibel sein. Die Firmware ist etwas abgespeckt gegenüber der Originalen, deshalb hat der Taster und die Helligkeitseinstellung per PatternPaint keine Funktion.

Wird das BlinkyTape ohne RC-Signal betrieben, ist die Helligkeit auf 10% eingestellt. Geht während dem Betrieb mit RC-Signal plötzlich das Signal verloren, wird automatisch die letzte Helligkeitseinstellung übernommen und das BlinkyTape läuft normal weiter.


### ACHTUNG!! 

-> Beim Anschluss am Computer per USB darf das BlinkyTape auf keinen Fall mit einer anderen Spannungsquelle versorgt werden, es besteht sonst Kurzschlussgefahr !! 

-> Nur stabilisiertes BEC mit 5V verwenden und auf den Stromverbrauch achten. Bei voller Helligkeit werden pro LED ca. 60mA benötigt, bei 100 LEDs sind dies bereits 6A !!



### Anschluss & Betrieb

Anschluss Servo Signal:

	5V = Pluspol (rot) 5V 
	A9 = Servo Signal (weiss)
	GND = Minuspol (schwarz)



Zwischen +5% und +100% läuft das BlinkyTape normal, und es kann während dem Betrieb die Helligkeit stufenlos eingestellt werden. Die Servoposition in [%] entspricht genau der Helligkeit in [%]. Zwischen -40% und +5% sind alle LEDs am BlinkyTape ausgeschaltet. Unter -40% wird das Farbmuster umgeschalten.

```
       RC-Position          Funktion
          
 +100%     +---+            -+
           |   |             |
           |   |             |-- BlinkyTape ON,
           |   |             |   Helligkeitseinstellung
           |   |             |
           |   |             |
           |   |  -- +5%    -+
    0%     +---+             |-- BlinkyTape OFF
           |   |             |
           |   |  -- -40%   -+      
           |   |             |
           |   |             |-- BlinkyTape ON,
           |   |             |   Farbmuster wechseln
           |   |             |
 -100%     +---+            -+ 
```




### Releasnotes:

V1.1.1     15.04.17
- Fehler bei der Signalverarbeitung behoben

V1.1       08.03.17
- lesen des Servosignal verbessert
- Firmware abgespeckt
- Taster wird nicht mehr unterstützt
- Helligkeitseinstellung per Pattern Paint wird nicht mehr unterstützt

V1.0       15.01.17
- PWM Servosignal wird unterstütz
- Framedelay optimiert
