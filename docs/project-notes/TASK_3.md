# Úloha 3 - Transformácie rovnicami, scény, les a vlastný login

Autor projektu: Martin Jarabica, jar0193  
Obdobie doložené v Git: 1. – 3. 10. 2026.  
Podklady: commity `1894d77`, `157da02`, `6d359ea` a archivované zdroje cvičenia 3.  
Poznámky pripravené s pomocou AI z histórie Git, zdrojových súborov a konzultácií.

## 1. Týždenný prehľad

Cieľom bolo ovládať posun, mierku a rotáciu cez uniformné premenné, zatiaľ bez transformačných matíc. Rozšírili sme aplikáciu o prepínateľné scény s loginom, guľou, trojuholníkom a lesom. Vlastný 3D login sme vytvorili pomocou Blenderu a pridali ho aj ako podpis do ostatných scén.

## 2. Čo sme spravili

- Zaviedli sme uniformy `offset`, `angle` a `scaleFactor`.
- Pridali sme dve preťažené metódy `ShaderProgram::setUniform()` pre jeden `float` a tri hodnoty typu `float`.
- Pri každom nastavovaní uniformu kontrolujeme výsledok `glGetUniformLocation()` na hodnotu `-1`.
- Rotáciu okolo osi Y sme vo vertex shaderi počítali priamo pomocou sínusu a kosínusu.
- Opravili sme chybný zápis rotačnej rovnice: chýbajúce znamienko a použitie súradnice Y tam, kde mala byť Z.
- Odstránili sme závislosť základného posunu od dvoch rôznych vertex shaderov; oba programy mohli používať `basic.vert`.
- Zobrazované zložky normál sme upravili cez `abs()`.
- Do `DrawableObject` sme pridali vlastný stav transformácie a metódu `getTransformation()`.
- Pridali sme ovládanie šípkami, Q/E a Z/X, s pohybom závislým od času.
- Rozšírili sme použitie `Scene` na štyri samostatné scény, prepínané klávesmi 1 až 4.
- Vytvorili sme les s 12 stromami, 12 kríkmi a slnkom. V tejto etape bolo rozloženie pravidelné.
- Použili sme dodaný model OpenGL textu a následne vlastný model `JAR0193`.
- Vygenerovali sme vlastný login v Blenderi pomocou AI skriptu, exportovali ho ako pole vrcholov a pridali podpis do ďalších scén.
- Zapli sme test hĺbky, pripravili triedny diagram, screenshoty a súhrn odovzdania.
- Vlastné komentáre sme upravovali na angličtinu podľa zadania.

## 3. Ako sme to spravili (Ukážky kódu & Vysvetlenie)

### Dve preťažené metódy na uniformy

Deklarácie tejto etapy v `ShaderProgram.h`:

```cpp
void setUniform(const char* name, float value) const;
void setUniform(
    const char* name, float x, float y, float z) const;
```

Preťaženie znamená, že metódy majú rovnaký názov, ale odlišné parametre. Kompilátor vyberie správnu podľa toho, či posielame jednu hodnotu alebo tri.

Príklad implementácie pre trojzložkový vektor:

```cpp
void ShaderProgram::setUniform(
    const char* name, float x, float y, float z) const
{
    GLint location = glGetUniformLocation(programID, name);

    if (location == -1)
    {
        std::cerr << "Uniform not found: " << name << std::endl;
        std::exit(EXIT_FAILURE);
    }

    use();
    glUniform3f(location, x, y, z);
}
```

Jednohodnotová verzia vykoná rovnakú kontrolu a zavolá `glUniform1f(location, value)`. Pred odoslaním aktivuje príslušný shader program, pretože `glUniform*` mení uniformy aktuálne používaného programu.

`location` je identifikátor konkrétnej aktívnej uniformnej premennej v konkrétnom programe. Hodnota `-1` môže znamenať chybný názov, chýbajúcu premennú alebo premennú, ktorú kompilátor odstránil, pretože neovplyvňuje výsledok.

### Posun, mierka a rotácia bez matíc

Podstatná časť historického `basic.vert`:

```glsl
uniform vec3 offset;
uniform float angle;
uniform float scaleFactor;

void main()
{
    vertexColor = abs(color);

    vec3 rotatedPosition;
    rotatedPosition.x = cos(angle) * position.x
                      + sin(angle) * position.z;
    rotatedPosition.y = position.y;
    rotatedPosition.z = -sin(angle) * position.x
                      + cos(angle) * position.z;

    gl_Position =
        vec4(rotatedPosition * scaleFactor + offset, 1.0);
}
```

Pri rotácii okolo Y sa menia súradnice X a Z; Y zostáva rovnaké. Uhol je v radiánoch. Napríklad približne `1.5708` radiánu je 90 stupňov.

Najprv vypočítame otočenú polohu, potom ju násobíme mierkou a nakoniec pripočítame posun. Jednotná mierka mení všetky tri súradnice rovnako. Posun potom umiestni objekt do scény.

`abs(color)` použijeme po zložkách. Pri modeloch, ktorých druhý atribút obsahuje normálu, sa tak záporné zložky zmenia na kladné a dajú sa zobraziť ako farba. Pri bežných RGB farbách v rozsahu 0 až 1 sa hodnota nezmení.

### Každý objekt má svoj stav transformácie

V tejto etape bola `Transformation` jednoduchá trieda s hodnotami:

```cpp
class Transformation
{
public:
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float offsetZ = 0.0f;
    float angle = 0.0f;
    float scaleFactor = 1.0f;
};
```

`DrawableObject::draw()` pred každým vykreslením poslal stav svojho objektu:

```cpp
shaderProgram.setUniform(
    "offset",
    transformation.offsetX,
    transformation.offsetY,
    transformation.offsetZ);

shaderProgram.setUniform("angle", transformation.angle);
shaderProgram.setUniform("scaleFactor", transformation.scaleFactor);

model.draw();
```

Viac objektov môže zdieľať jeden shader program. Uniformy si preto nastavíme vždy pred kreslením konkrétneho objektu, aby dostal svoju polohu a veľkosť.

### Prepínanie scén

`Scene` už vznikla pri predchádzajúcom refaktoringu. Teraz sme pripravili viac jej inštancií a vyberali aktívnu:

```cpp
Scene* activeScene = &scene;
DrawableObject* activeObject = &logoObject;

if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
{
    activeScene = &sphereScene;
    activeObject = &sphereObject;
}

activeScene->draw();
```

Výber scény určuje, ktoré objekty kreslíme. `activeObject` samostatne určuje objekt ovládaný klávesnicou.

| Kláves | Scéna v úlohe 3 | Ovládaný objekt |
|---|---|---|
| 1 | Vlastný login | Login |
| 2 | Guľa | Guľa |
| 3 | Trojuholník | Trojuholník |
| 4 | Les | Prvý strom |

`Scene::addObject()` uchováva adresu objektu a `Scene::draw()` volá jeho metódu `draw()`. Scéna objekty automaticky nevlastní ani nemaže. V našom riešení sú vytvorené lokálne v `Application::run()` a existujú počas celej slučky.

### Ovládanie nezávislé od počtu snímkov

```cpp
double currentTime = glfwGetTime();
float deltaTime = static_cast<float>(currentTime - lastTime);
lastTime = currentTime;

Transformation& transform = activeObject->getTransformation();

if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
{
    transform.offsetX += speed * deltaTime;
}
```

`deltaTime` je počet sekúnd od predchádzajúceho snímku. Ak sa za jednu sekundu vykreslí viac snímkov, jednotlivé kroky sú menšie, ale celkový posun za sekundu zostáva rovnaký.

Šípky menia X/Y posun, Q/E uhol a Z/X mierku. Mierku obmedzujeme minimom `0.05f`, aby sa ovládaný objekt nezmenšil na nulu.

### Les a zdieľané modely

Základ pôvodného pravidelného rozloženia stromov:

```cpp
const int treeCount = 12;
std::vector<DrawableObject> trees;
trees.reserve(treeCount);

for (int i = 0; i < treeCount; ++i)
{
    trees.emplace_back(treeModel, colorProgram);

    DrawableObject& treeObject = trees.back();
    Transformation& t = treeObject.getTransformation();

    int column = i % 4;
    int row = i / 4;

    t.offsetX = -0.72f + 0.48f * column;
    t.offsetY = -0.85f + 0.50f * row;
    t.scaleFactor = 0.05f + 0.005f * (i % 3);
    t.angle = 0.2f * column;

    forestScene.addObject(treeObject);
}
```

Dvanásť stromov má jeden spoločný `treeModel`, ale každý vlastnú transformáciu. Podobne vytvárame 12 kríkov; slnko používa model gule so žltým shaderom.

`reserve(treeCount)` vopred zabezpečí kapacitu pre plánovaný počet objektov. Je to potrebné, pretože scéna uchováva ich adresy a bežné zväčšenie vektora by mohlo objekty presunúť. Táto ochrana platí pre plánovaný počet; pridanie ďalších objektov nad rezervovanú kapacitu si vyžaduje novú úvahu o životnosti adries.

### Vlastný 3D login a podpis

Pomocou AI sme pripravili Blender skript na vytvorenie textu `JAR0193`, pridanie hĺbky, prevod na mesh, trianguláciu a export polohy a normály každého vrcholu. Model sa normalizoval do rozsahu približne -1 až 1 so zachovaním pomerov.

Výsledný `Login.h` obsahuje pole `loginVertices`, 6 276 vrcholov a 2 092 trojuholníkov. Hlavička uvádza autora, login, formát aj použitie AI.

```cpp
const GLsizei logoVertexCount = static_cast<GLsizei>(
    sizeof(loginVertices) / (6 * sizeof(float)));

Model logoModel(loginVertices, logoVertexCount);
DrawableObject logoObject(logoModel, colorProgram);
DrawableObject signatureObject(logoModel, yellowProgram);

Transformation& t = signatureObject.getTransformation();
t.offsetX = 0.70f;
t.offsetY = -0.90f;
t.offsetZ = -0.80f;
t.scaleFactor = 0.20f;

sphereScene.addObject(signatureObject);
triangleScene.addObject(signatureObject);
forestScene.addObject(signatureObject);
```

Hlavný login a malý podpis zdieľajú geometriu. Podpis má vlastný shader a transformáciu; ten istý podpisový objekt môže byť pridaný do viacerých scén.

Pre správne zakrývanie povrchov používame `glEnable(GL_DEPTH_TEST)` a pri každom snímku čistíme aj depth buffer. Viditeľná hĺbka písmen vychádza z modelu, nie z hotového osvetľovacieho systému.

## 4. Code Review & Poznatky

- **Preťaženie nie je duplicitná definícia.** Dve metódy s rovnakým názvom a rôznymi parametrami sú zámerné; tú istú signatúru však nemôžeme definovať dvakrát.
- **Uniform nie je súčasť každého vrcholu.** Je to hodnota spoločná pre dané kreslenie, napríklad posun celého objektu. Polohy jednotlivých vrcholov prichádzajú ako atribúty z VBO.
- **Shader musí uniform skutočne používať.** Samotná deklarácia nemusí zaručiť platné `location`. Po zmene názvov alebo shaderu kontrolujeme, čo aplikácia posiela.
- **Nulová mierka môže skryť celý model.** Pri diagnostike prázdneho okna je preto dôležité overiť nastavenie mierky, údaje modelu, pridanie do scény, aktívny shader a volanie kreslenia.
- **Rotácia modelu nie je pohyb kamery.** V tejto etape prepočítavame samotné vrcholy, nie pohľad na scénu.
- **Volanie `scene.draw()` nenahradilo model novou geometriou.** Len presunulo iterovanie cez objekty do triedy zodpovednej za scénu.
- **`glUseProgram(0)` zruší výber aktuálneho programu.** Nevymaže program ani hodnoty uniformov. Pri kreslení v Core Profile však potrebujeme znovu aktivovať platný program.
- **Dôležité je skutočné vlastníctvo.** `Application` vlastní životnosť okna; modely, programy, objekty a scény vytvára v `run()`. `Scene` ukladá ukazovatele a `DrawableObject` referencie. Diagram má opisovať tieto skutočné väzby.
- **Historický les bol pravidelný.** Náhodné rozloženie patrí až do úlohy 4; v poznámkach tieto verzie nezamieňame.
- **Podmienky odovzdania sa týkajú aj pôvodu kódu.** Učiteľove modely neupravujeme; vlastné súbory majú mať autora a login a pomoc AI musí byť uvedená. V aktuálnom projekte ešte nie sú tieto hlavičky doplnené všade.

Postup je doložený commitom `1894d77` pre rovnice a prvé prepínanie, `157da02` pre rozšírené scény a login a `6d359ea` pre súhrn, screenshoty a diagram.

Archivované zdroje a screenshoty sú v `Tasks/2.task`, teda v druhom odovzdaní na Kelvin. Súhrn označuje túto etapu ako cvičenie 3.
