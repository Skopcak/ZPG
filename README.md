# Cvičenie 2

Screenshoty zachytávajú postup riešenia. Aktuálna verzia zobrazuje dve gule.

## 1. Programovateľná pipeline

**Splnené**

Použil som GLAD, vertex a fragment shader a pomocou VBO a VAO som vykreslil farebný trojuholník. Výsledok je na obrázku.

![Farebný trojuholník](jar0193_01.png)

## 2. Chyba v shaderi

**Splnené**

V shaderi som schválne vymazal bodkočiarku a program vypísal chybu a ukončil sa, čo vidno v konzole na obrázku. Potom som bodkočiarku vrátil, aby program znova fungoval.

![Chybový výpis zo shaderu](jar0193_02.png)

## 3. Štvorec so žltým vrcholom

**Splnené**

Z dvoch trojuholníkov som vytvoril štvorec a štvrtému rohu som nastavil žltú farbu `(1, 1, 0)`. Na obrázku je žltý ľavý horný roh.

![Štvorec so žltým rohom](jar0193_03.png)

## 4. Priložený model

**Splnené**

Namiesto údajov štvorca som použil údaje gule zo súboru `sphere.h`. Na obrázku je guľa s prekrývajúcimi sa trojuholníkmi, pretože som pri tejto ukážke vypol test hĺbky.

![Model gule](jar0193_04.png)

## 5. Viac objektov a shader programov

**Splnené**

Guľu som vykreslil dvakrát s rôznymi shader programami – vľavo je farebná a vpravo žltá. Vedľa seba som ich posunul pripočítaním vektorov vo vertex shaderoch.

![Dve gule s rôznymi shader programami](jar0193_05.png)

## 6. Relatívne cesty

**Splnené**

Pri načítaní shaderov používam cesty ako `shaders/basic.vert`. Cesty ku knižniciam a modelom som nastavil cez `$(ProjectDir)`, čo vidno na obrázku.

![Relatívne cesty v nastavení projektu](jar0193_06.png)
