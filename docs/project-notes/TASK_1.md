# Úloha 1 - Základ projektu a vykresľovacia slučka

Autor projektu: Martin Jarabica, jar0193  
Podklady: úvodný kód z hodiny, prvý dostupný commit a následný vývoj aplikácie.  
Poznámky pripravené s pomocou AI z histórie Git, zdrojových súborov a konzultácií.

> Samostatná verzia cvičenia 1 sa v repozitári nezachovala. Prvý commit `b7704ec` z 26. 9. 2026 už obsahuje riešenie s moderným OpenGL a dvoma guľami. Táto časť preto rekonštruuje doložený základ aplikácie; netvrdí, že všetky uvedené prvky vznikli v samostatnom prvom týždni.

## 1. Týždenný prehľad

Cieľom úvodnej etapy bolo pripraviť projekt v C++, vytvoriť okno a pochopiť základný beh grafickej aplikácie. Naučili sme sa, že program potrebuje OpenGL kontext a slučku, ktorá opakovane pripravuje obraz, zobrazí ho a spracuje udalosti. Tento základ zostal zachovaný aj po presune kódu do triedy `Application`.

## 2. Čo sme spravili

- Použili sme GLFW na vytvorenie okna s rozmermi 800 × 600 a názvom ZPG.
- Pripravili sme OpenGL kontext; v zachovaných verziách aplikácia žiada OpenGL 4.3 Core Profile.
- Nastavili sme aktuálny kontext pomocou `glfwMakeContextCurrent()`.
- Zapli sme synchronizáciu výmeny obrazových bufferov pomocou `glfwSwapInterval(1)`.
- Vytvorili sme slučku, ktorá beží do zatvorenia okna.
- Pridali sme ukončenie klávesom Escape a výpis chýb GLFW.
- Prispôsobili sme vykresľovaciu oblasť veľkosti framebufferu cez `glViewport()`.
- Zabezpečili sme zrušenie okna a ukončenie GLFW pri normálnom konci aplikácie.
- Ujasnili sme si úlohy GLFW, OpenGL, GLAD a GLM, ktoré sa postupne používajú v ďalších etapách.

## 3. Ako sme to spravili (Ukážky kódu & Vysvetlenie)

### Vytvorenie okna a kontextu

Skrátená ukážka zo základu aplikácie:

```cpp
if (!glfwInit())
{
    std::exit(EXIT_FAILURE);
}

glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

GLFWwindow* window =
    glfwCreateWindow(800, 600, "ZPG", nullptr, nullptr);

if (!window)
{
    glfwTerminate();
    std::exit(EXIT_FAILURE);
}

glfwMakeContextCurrent(window);
glfwSwapInterval(1);
```

`glfwInit()` pripraví GLFW. `glfwCreateWindow()` vytvorí okno spolu s OpenGL kontextom. Kontext uchováva stav OpenGL a musí byť aktuálny vo vlákne, ktoré volá jeho funkcie.

GLFW rieši okno, klávesnicu a udalosti. OpenGL zabezpečuje grafické operácie. V ďalšej úlohe GLAD načíta adresy dostupných funkcií OpenGL a GLM neskôr poskytne vektory a matice.

### Vykresľovacia slučka

```cpp
while (!glfwWindowShouldClose(window))
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the current scene here.

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

Jeden priechod slučkou pripraví jeden snímok. `glClear()` vymaže predchádzajúce farby a hĺbku, aby po objektoch nezostávali stopy. `glfwSwapBuffers()` zobrazí pripravený snímok a `glfwPollEvents()` spracuje vstup a udalosti okna.

`glfwSwapInterval(1)` žiada synchronizáciu výmeny bufferov s obnovovaním obrazovky. Neskôr používame aj čas medzi snímkami, aby rýchlosť pohybu nezávisela od ich počtu.

### Escape a zmena veľkosti okna

```cpp
static void key_callback(
    GLFWwindow* window, int key,
    int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void framebuffer_size_callback(
    GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
```

Callback je funkcia, ktorú GLFW zavolá pri konkrétnej udalosti. Escape nastaví požiadavku na zatvorenie; slučka potom skončí pri najbližšej kontrole.

V staršom kóde sa používal callback veľkosti okna. Vo výslednej aplikácii používame veľkosť framebufferu, pretože `glViewport()` potrebuje rozmery vykresľovacej plochy v pixeloch. Tie sa pri škálovaní displeja môžu líšiť od rozmerov okna.

### Ukončenie

```cpp
glfwDestroyWindow(window);
glfwTerminate();
```

Tieto volania uvoľnia okno, jeho kontext a zdroje GLFW. Neskôr sa presunuli do deštruktora `Application`.

## 4. Code Review & Poznatky

- **Okno nie je vykreslený model.** Prázdne okno potvrdzuje len časť inicializácie; na zobrazenie geometrie neskôr potrebujeme údaje vrcholov, shader program a príkaz na kreslenie.
- **Poradie inicializácie je dôležité.** Najprv pripravíme GLFW a aktuálny kontext, až potom načítame funkcie cez GLAD a vytvárame grafické zdroje.
- **Framebuffer a okno nemusia mať rovnaké rozmery v pixeloch.** Preto výsledný kód používa `glfwSetFramebufferSizeCallback()` a po vytvorení okna aj `glfwGetFramebufferSize()`.
- **Vek knižnice alebo názov komentára neurčuje verziu kontextu.** Zachovaný komentár spomínal OpenGL 3.3, ale konkrétne nastavenia žiadajú 4.3.
- **Zdroje OpenGL potrebujú platný kontext aj pri uvoľňovaní.** Modely a shader programy sa preto musia zničiť pred oknom. Toto sme neskôr vyriešili životnosťou lokálnych objektov v `Application::run()`.
- **Úvodné riešenie nadväzovalo na učiteľov kód.** Pri poznámkach rozlišujeme dodaný základ, vlastné úpravy a pomoc AI.

Súbory, do ktorých sa tento základ neskôr presunul: `Application.h`, `Application.cpp` a jednoduchý `main.cpp`.
