# Úloha 2 - Programovateľná pipeline, modely a objektový návrh

Autor projektu: Martin Jarabica, jar0193  
Obdobie doložené v Git: 26. – 27. 9. 2026.  
Podklady: commity `b7704ec`, `9d7be14`, `9223087`, `3ba8f6b`, `026f7c8` a súhrn cvičenia 2.  
Poznámky pripravené s pomocou AI z histórie Git, zdrojových súborov a konzultácií.

## 1. Týždenný prehľad

Cieľom bolo prejsť na programovateľnú pipeline s vertex a fragment shaderom a ukladať údaje modelov cez VBO a VAO. Postupovali sme od farebného trojuholníka cez štvorec k dodanému modelu gule a dvom objektom s rozdielnymi shader programami. Nakoniec sme pôvodný rozsiahly `main.cpp` rozdelili medzi triedy a pripravili riešenie na ďalšie cvičenia.

## 2. Čo sme spravili

- Pripojili sme hlavičkovú verziu GLAD 2 a načítali funkcie OpenGL.
- Načítali sme shadery zo súborov, skompilovali ich a spojili do shader programu.
- Pridali sme kontrolu kompilácie shaderov aj linkovania programu.
- Úmyselnou chybou v shaderi sme overili výpis chyby a ukončenie aplikácie.
- Vykreslili sme trojuholník s farbou pre každý vrchol.
- Opravili sme geometriu, keď sa namiesto požadovaného trojuholníka zobrazoval štvorec alebo iba jeho spodná pravá časť.
- Vytvorili sme štvorec z dvoch trojuholníkov; jeho štvrtý geometrický roh je žltý.
- Pridali sme `sphere.h` a vypočítali počet vrcholov z veľkosti poľa.
- Vykreslili sme rovnaký model gule dvakrát: farebný a žltý.
- Po úvodnom experimente s uniformom sme podľa vtedajšej látky použili pevné vektorové posuny vo vertex shaderoch.
- Nahradili sme absolútne cesty relatívnymi cestami shaderov a cestami odvodenými od `$(ProjectDir)`.
- Zaviedli sme triedy `Shader`, `ShaderProgram`, `Model`, `DrawableObject`, `Scene` a `Application`.
- Začali sme verzovať cez Git a GitHub Desktop; pripravili sme screenshoty a Markdown súhrn.

## 3. Ako sme to spravili (Ukážky kódu & Vysvetlenie)

### GLAD iba s jednou implementáciou

```cpp
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
```

Tento blok zostal v `main.cpp`. Makro umožní hlavičkovej verzii GLAD vložiť aj implementáciu; definujeme ho iba v jednom kompilačnom súbore. `#undef` následne zruší definíciu makra.

Po vytvorení aktuálneho kontextu voláme:

```cpp
if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
{
    std::cerr << "GLAD initialization failed\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
}
```

GLAD cez GLFW získa adresy funkcií podporovaných ovládačom. Samotné pridanie hlavičky nestačí na ich načítanie.

### Trojuholník, údaje vrcholov a VBO + VAO

```cpp
const float triangleVertices[] = {
     0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f
};
```

Každý vrchol má šesť hodnôt: polohu `x, y, z` a farbu `r, g, b`. Horný vrchol má červenú farbu, ľavý dolný modrú a pravý dolný zelenú.

Skrátená verzia pôvodného nahrávania:

```cpp
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices),
             triangleVertices, GL_STATIC_DRAW);

glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                      6 * sizeof(float), (void*)0);

glEnableVertexAttribArray(1);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                      6 * sizeof(float),
                      (void*)(3 * sizeof(float)));

glDrawArrays(GL_TRIANGLES, 0, 3);
```

VBO uchováva údaje v pamäti spravovanej OpenGL. VAO uchováva nastavenie atribútov a ich väzby na buffery. Krok medzi vrcholmi, čiže stride, je šesť hodnôt typu `float`; druhý atribút začína po prvých troch.

`glDrawArrays()` dostáva počet **vrcholov**, nie počet hodnôt, bajtov ani trojuholníkov. Pri `GL_TRIANGLES` tvorí každá trojica vrcholov jeden trojuholník.

### Štvorec a žltý štvrtý roh

V použitej geometrii má štvorec štyri rôzne rohy, ale dva z nich zapisujeme dvakrát, pretože vykresľujeme dva samostatné trojuholníky. Posledný záznam patrí ľavému hornému rohu:

```cpp
-0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 0.0f
```

Prvé tri hodnoty sú poloha, posledné tri žltá farba. Pre celý štvorec použijeme:

```cpp
glDrawArrays(GL_TRIANGLES, 0, 6);
```

Keď sme kreslili iba prvé tri záznamy poľa štvorca, zobrazil sa jeden z jeho trojuholníkov. Na klasický trojuholník sme preto upravili aj samotné polohy vrcholov.

### Kompilácia, linkovanie a úmyselná chyba

Kľúčová kontrola pri kompilácii shaderu:

```cpp
glShaderSource(shaderID, 1, &source, nullptr);
glCompileShader(shaderID);

GLint success = GL_FALSE;
glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

if (!success)
{
    char infoLog[1024];
    glGetShaderInfoLog(shaderID, sizeof(infoLog), nullptr, infoLog);
    std::cerr << "Shader failed:\n" << infoLog << std::endl;
    glDeleteShader(shaderID);
    std::exit(EXIT_FAILURE);
}
```

Shader je samostatný program pre jednu časť pipeline. `ShaderProgram` spojí vertex a fragment shader cez `glAttachShader()` a `glLinkProgram()`; úspech kontroluje pomocou `GL_LINK_STATUS` a pri chybe vypíše `glGetProgramInfoLog()`.

Pri testovaní sme vynechali bodkočiarku. Hlásenie spomínajúce nečakané `out` a následne neznáme `FragColor` ukázalo, že prvá syntaktická chyba môže spôsobiť aj ďalšie hlásenia. Po overení sme bodkočiarku vrátili.

Fragment shader nastavuje farbu:

```glsl
FragColor = vec4(vertexColor, 1.0);
```

`vertexColor` už obsahuje tri zložky a `1.0` pridáva alfa zložku. Pokus `vec4(vertexColor, 1.0, 1.0)` dodáva päť zložiek, a preto neprejde kompiláciou.

### Guľa a dva shader programy

```cpp
const GLsizei sphereVertexCount = static_cast<GLsizei>(
    sizeof(sphere) / (6 * sizeof(float)));

Model sphereModel(sphere, sphereVertexCount);

ShaderProgram colorProgram(
    "shaders/basic.vert", "shaders/basic.frag");
ShaderProgram yellowProgram(
    "shaders/yellow.vert", "shaders/yellow.frag");

DrawableObject colorSphere(sphereModel, colorProgram);
DrawableObject yellowSphere(sphereModel, yellowProgram);
```

`sizeof(sphere)` je veľkosť celého poľa v bajtoch. Delenie veľkosťou jedného vrcholu dá počet vrcholov. Tento výpočet funguje tu, kde je `sphere` skutočné pole; pri obyčajnom ukazovateli by `sizeof` meralo ukazovateľ.

V konečnej verzii tejto etapy sa posuny nachádzali priamo vo vertex shaderoch:

```glsl
// basic.vert
gl_Position =
    vec4(position * 0.35 + vec3(-0.5, 0.0, 0.0), 1.0);

// yellow.vert
gl_Position =
    vec4(position * 0.35 + vec3(0.5, 0.0, 0.0), 1.0);
```

Oba objekty zdieľajú rovnaký `Model`, takže sa geometria nahrá iba raz. Každý však vyberie svoj shader program. Žltý fragment shader používa konštantnú farbu `vec4(1.0, 1.0, 0.0, 1.0)`.

Prvý dostupný commit ešte obsahuje uniform `offset`. Commit `9d7be14` zachytáva následný prechod na pevné vektorové posuny; uniformy sme systematicky doplnili až v úlohe 3.

### Rozdelenie do tried a krátky main

| Trieda | Zodpovednosť v tejto etape |
|---|---|
| `Application` | Okno, inicializácia, hlavná slučka a ukončenie. |
| `Scene` | Zoznam odkazov na objekty a ich spoločné vykreslenie. |
| `DrawableObject` | Spojenie konkrétneho modelu so shader programom. |
| `Model` | VBO, VAO a počet vrcholov. |
| `ShaderProgram` | Spojenie shaderov, aktivácia a životnosť programu. |
| `Shader` | Načítanie, kompilácia a životnosť jedného shaderu. |

```cpp
int main()
{
    Application app;
    app.run();
    return 0;
}
```

Presun kódu nezmenil základ vykresľovania. `scene.draw()` zavolá kreslenie objektov, objekt zvolí shader a model zavolá `glDrawArrays()`.

Objekt `Application` vzniká na zásobníku. Modely a programy vytvorené lokálne v `run()` sa zničia pri návrate z tejto metódy, ešte pred deštruktorom aplikácie a zrušením okna.

### Relatívne cesty a verzovanie

Shadery načítavame cez cesty ako `shaders/basic.vert`. V nastaveniach Visual Studia sú cesty ku knižniciam odvodené od `$(ProjectDir)`, takže neobsahujú konkrétne používateľské umiestnenie.

Relatívna cesta súboru shaderu sa vyhodnocuje voči pracovnému adresáru spusteného programu, nie automaticky voči umiestneniu `.cpp`. Pridanie shaderu do Solution Explorer slúži na jeho evidenciu a editovanie, ale samo osebe nemení pracovný adresár.

V GitHub Desktop rozlišujeme uloženie súboru, lokálny commit a odoslanie cez push. Úvodné publikovanie repozitára a ďalšie pushovanie sú samostatné kroky. `.gitignore` vylučuje napríklad `.vs`, `Debug` a `x64`.

## 4. Code Review & Poznatky

- **Refaktoring zachoval správanie.** Commit `9223087` presunul shadery do tried a `3ba8f6b` dokončil základné OOP rozdelenie. Nárast počtu riadkov súvisel aj s deklaráciami, kontrolami chýb a správou zdrojov.
- **Referencia na model je zámerná.** `DrawableObject` si model požičiava; nevlastní ho. Umožňuje to vykresliť jednu geometriu viackrát bez duplikácie VBO. Model musí existovať počas každého vykreslenia objektu.
- **Definície metód patria mimo telo konštruktora.** Pri tvorbe `Model.cpp` sme opravili nesprávne vnorené definície `draw()` a deštruktora.
- **Deštruktory zostali potrebné.** `Model` uvoľňuje VAO a VBO, `ShaderProgram` program a `Shader` samostatný shader. Kopírovanie tried vlastniacich tieto identifikátory je zakázané, aby sa rovnaký zdroj neuvoľnil dvakrát.
- **Farba a normála sú rôzne údaje.** Trojuholník má na druhom atribúte RGB farbu. Dodaná guľa vo formáte `x, y, z, nx, ny, nz` má normálu, ktorej zložky sme dočasne zobrazovali ako farbu.
- **Počítame vrcholy, až potom trojuholníky.** Pri poli so 17 280 hodnotami a šiestimi hodnotami na vrchol vychádza 2 880 vrcholov, teda 960 samostatných trojuholníkov.
- **Test hĺbky a drôtový model sú rozdielne veci.** Vypnutý test hĺbky v historickej ukážke spôsoboval prekrývanie plôch. Skutočné hrany trojuholníkov by sme zobrazili cez `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)`.
- **Podoba posledného snímku nemusí ukazovať všetky staršie kroky.** Trojuholník, chyba shaderu a štvorec sú doložené samostatnými screenshotmi, hoci konečný program tejto etapy zobrazuje dve gule.

Historické screenshoty a súhrn sú uložené v priečinku `Tasks/1.task`. Číslo priečinka označuje prvé odovzdanie na Kelvin, jeho obsah však patrí ku cvičeniu 2.
