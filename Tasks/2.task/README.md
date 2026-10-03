# Cvičenie 3

Martin Jarabica – jar0193

## 1. Transformácie

Splnené – posun, zmenu mierky a rotáciu počítam pomocou rovníc vo vertex shaderi a ovládam klávesmi.

![Login pred úpravou transformácií](jar0193_1a.png)
![Login po úprave transformácií s viditeľnou hĺbkou](jar0193_1b.png)

## 2. Uniformné premenné

Splnené – dve preťažené metódy `setUniform()` posielajú hodnoty do shaderu a kontrolujú, či premenná existuje.

![Preťažené metódy a kontrola location == -1](jar0193_2.png)

## 3. Prepínanie scén

Splnené – trieda `Scene` uchováva odkazy na objekty a medzi scénami prepínam klávesmi 1 až 4.

![Prepínanie aktívnej scény a ovládaného objektu](jar0193_3.png)

## 4. Testovacie scény

Splnené – vytvoril som samostatné scény s guľou, trojuholníkom, lesom a loginom.

![Scéna s guľou a podpisom](jar0193_4a.png)
![Scéna s trojuholníkom a podpisom](jar0193_4b.png)
![Scéna s lesom, slnkom a podpisom](jar0193_4c.png)
![Scéna s 3D loginom JAR0193](jar0193_4d.png)

## 5. Les

Splnené – les obsahuje 12 stromov, 12 kríkov a slnko, ktoré vidno na [obrázku lesa z bodu 4](jar0193_4c.png).

## 6. Funkcia main

Splnené – funkcia `main()` vytvorí objekt `Application` a spustí aplikáciu cez `run()`.

![Vytvorenie a spustenie aplikácie vo funkcii main](jar0193_6-.png)

## 7. Vlastný login

Splnené – [3D login JAR0193](jar0193_1b.png) som vytvoril v Blenderi pomocou AI skriptu a pridal ho ako [malý podpis do ostatných scén](jar0193_4c.png).

## 8. Triedy a diagram

Splnené – kód je rozdelený do samostatných tried a ich premenné a väzby sú znázornené v [triednom diagrame](CLASS_DIAGRAM.md).

![Triedny diagram aplikácie](jar0193_8.png)
