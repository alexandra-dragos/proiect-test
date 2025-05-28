#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <conio.h>
#include <math.h>
#include <SDL.h>

#define _USE_MATH_DEFINES
#define N 21             //nr de linii
#define M 21               //nr de coloane
#define NR_OBSTACOLE 45       //nr de obstacole
#define NR_PUNCTE 20       //nr de puncte
#define NR_FANTOME 4       //nr fantomelor
#define CELL_SIZE 30         //dimensiune patrat pe ecran
#define SPATIU_SCOR 40       // spatiul necesar pt scor
#define VITEZA_FANTOME 15     //frecventa de miscare a fantomelor 



SDL_Window* window = NULL;      //pointer catre fereastra jocului
SDL_Renderer* renderer = NULL;       //pointer catre renderer SDL (deseneaza pe fereastra)

int counter = 0;
typedef struct
{
    int x;        //coordonata fantomei pe linii
    int y;        //coordonata fantomei pe coloana
    char simbol;   //simbol pt fantoma
}Fantoma;



typedef  struct
{
    int x;                    //coordonata lui Pacman pe linie
    int y;                    // coordonata lui Pacman pe coloana
    char simbol;               //simbolul cu care este reprezentat Pacman
    int puncte;              //scorul
}Pacman;


void initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Eroare la initializare SDL:%s\n", SDL_GetError());
        exit(1);
    }
    window = SDL_CreateWindow("Paxman SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, M * CELL_SIZE, N * CELL_SIZE + SPATIU_SCOR, 0);
    if (!window)
    {
        printf("Eroare la crearea ferestrei:%s\n", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Eroarea la crearea renderer-ului:%s\n", SDL_GetError());
        exit(1);
    }
}

void closeSDL() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}


void formare_contur(char labirint[N][M])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (i == 0 || j == 0 || i == N - 1 || j == M - 1)
            {
                labirint[i][j] = '#';                     // formam peretii labirintului 
            }
            else
            {
                labirint[i][j] = ' ';                       // in interiorul sau punem spatii libere
            }
        }
    }
    srand(time(NULL));
    for (int k = 0; k < NR_OBSTACOLE; k++)
    {
        int rand_i;                       // variabila care ne da coordonata pt linie
        int rand_j;                         // variabila care ne da coordonata pt coloana
        do
        {
            rand_i = rand() % (N - 2) + 1;                    //evitam marginile si umplem labirintul cu obstacole
            rand_j = rand() % (M - 2) + 1;

        } while (labirint[rand_i][rand_j] == '#');
        labirint[rand_i][rand_j] = '#';                        //punem obstacolul

    }
    for (int i = 0; i < NR_PUNCTE; i++)
    {
        int rand_x;
        int rand_y;
        do
        {
            rand_x = rand() % (N - 2) + 1;                     //coordonata pe linie
            rand_y = rand() % (M - 2) + 1;                      //coordonata pe coloana

        } while (labirint[rand_x][rand_y] == '#');
        labirint[rand_x][rand_y] = 'o';                       //simbol pt puncte


    }
    //punem pozitiile din jurul lui pacman sa fie libere
    labirint[1][1] = ' ';
    labirint[1][2] = ' ';
    labirint[2][1] = ' ';

}

void plaseaza_pacman(Pacman* pacman, char labirint[N][M], int x, int y)
{
    if (x >= 1 && x < N - 1 && y >= 1 && y < M - 1 && (labirint[x][y] == ' ' || labirint[x][y] == 'o'))
    {
        pacman->x = x;           //actualizam pozitia 
        pacman->y = y;
        pacman->simbol = 'P';
        labirint[x][y] = pacman->simbol;      //punem in labirint 

    }

}

void plaseaza_fantoma(Fantoma fantome[], char labirint[N][M])
{
    for (int i = 0; i < NR_FANTOME; i++)
    {
        int fantoma_x, fantoma_y;                  //coordonatele pt pozitia unei fantome
        do
        {
            fantoma_x = rand() % (N - 2) + 1;
            fantoma_y = rand() % (M - 2) + 1;            //evitam marginile
        } while (labirint[fantoma_x][fantoma_y] != ' ');                  //punem fantoma pe spatiile libere
        fantome[i].x = fantoma_x;                        //coordonatele unei fantome
        fantome[i].y = fantoma_y;
        fantome[i].simbol = 'F';                     //simbolul fantomei
        labirint[fantoma_x][fantoma_y] = fantome[i].simbol;               //se pune fantoma pe pozitie in labirint

    }
}

void misca_pacman(char labirint[N][M], Pacman* pacman, char directie)
{

    labirint[pacman->x][pacman->y] = ' ';                 //stergem pozitia curenta
    int x_nou = pacman->x;                              //variabila pt pozitia lui Pacman pt coordonata de pe linie 
    int y_nou = pacman->y;                             //variabila pt pozitia lui Pacman pt coordonata de pe coloana

    //ACTUALIZARE POZITIE PACMAN 
    if ((directie == 'W' || directie == 'w') && labirint[x_nou - 1][y_nou] != '#')          //miscare in sus
    {
        x_nou--;
    }
    else if ((directie == 'S' || directie == 's') && labirint[x_nou + 1][y_nou] != '#')     //miscare in jos
    {
        x_nou++;
    }
    else if ((directie == 'D' || directie == 'd') && labirint[x_nou][y_nou + 1] != '#')     //miscare la dreapta
    {
        y_nou++;
    }
    else if ((directie == 'A' || directie == 'a') && labirint[x_nou][y_nou - 1] != '#')             //miscare la stanga
    {
        y_nou--;
    }


    if (labirint[x_nou][y_nou] == 'o')       //verificam daca pozitia lui pacman este aceiasi cu pozitia punctului
    {
        pacman->puncte++;                            //crestem scorul
        labirint[x_nou][y_nou] = ' ';          //luam punctul
    }
    pacman->x = x_nou;
    pacman->y = y_nou;
    labirint[pacman->x][pacman->y] = 'P';                                 // plasam in noua pozitie
}



void urmareste_pacman(Fantoma* fantoma, char labirint[N][M], Pacman* pacman)
{
    int x_actual = fantoma->x;       //pozitia actuala pe linie
    int y_actual = fantoma->y;        //pozitia actuala pe coloana
    int x_apropiere = x_actual;        // cea mai buna pozitie a fantomei pt a se apropia de Pacman
    int y_apropiere = y_actual;
    int min = 9999;
    int directie_x[] = { -1, 1, 0, 0 };     //vector pt directie -1 sus si 1 jos 
    int directie_y[] = { 0, 0, -1, 1 };     //vector pt directie -1 stanga si 1 dreapta 

    for (int i = 0; i < 4; i++)
    {
        int pozVecina_x = x_actual + directie_x[i];          //pt fiecare directie se calculeaza pozitia vecina 
        int pozVecina_y = y_actual + directie_y[i];

        if (labirint[pozVecina_x][pozVecina_y] == ' ' || labirint[pozVecina_x][pozVecina_y]=='o')
        {
            int dist = abs(pozVecina_x - (pacman->x)) + abs(pozVecina_y - (pacman->y));  //calculam cat de departe ar fi fantoma de pacman daca ar merge in acea directie
            if (dist < min)
            {
                min = dist;
                x_apropiere = pozVecina_x;            //se actualizeaza pozitia fantomei
                y_apropiere = pozVecina_y;
            }
        }
    }
    if (x_apropiere != x_actual || y_apropiere != y_actual)
    {
        labirint[x_actual][y_actual] = ' ';      //se sterge simbolul vechi din labirint 
        fantoma->x = x_apropiere;              //se actualizeaza pozitia fantomei 
        fantoma->y = y_apropiere;
        labirint[fantoma->x][fantoma->y] = fantoma->simbol;            //se plaseaza fantoma pe cea mai buna pozitie 

    }
    //verificare pozitiei fantomei si a lui pacman 
    if (fantoma->x == pacman->x && fantoma->y == pacman->y)
    {

        printf("GAME OVER! SCORUL FINAL ESTE:%d\n", pacman->puncte);
        closeSDL();
        exit(0);
    }

}

void misca_fantoma(Fantoma fantome[], char labirint[N][M], Pacman* pacman)
{
    for (int i = 0; i < NR_FANTOME; i++)
    {
        if (i < 2)                           //primele 2 fantome il urmaresc pe pacman 
        {
            urmareste_pacman(&fantome[i], labirint, pacman);
        }
        else
        {
            int directie = rand() % 4;        //0-sus 1-jos 2-stanga 3-dreapta 
            int x_nou = fantome[i].x;       //variabila pt coordonata fantomei
            int y_nou = fantome[i].y;

            //stabilim pozitia
            if (directie == 0)
                x_nou--;
            else if (directie == 1)
                x_nou++;
            else if (directie == 2)
                y_nou--;
            else if (directie == 3)
                y_nou++;

            //verificam daca pozitia este valida
            if (labirint[x_nou][y_nou] == ' ' || labirint[x_nou][y_nou] == 'o')
            {
                labirint[fantome[i].x][fantome[i].y] = ' ';         //stergem vechea pozitie 
                fantome[i].x = x_nou;
                fantome[i].y = y_nou;                               //actualizam noua pozitie 
                labirint[fantome[i].x][fantome[i].y] = fantome[i].simbol;        //punem fantoma pe pozitie 

            }
            else if ((fantome[i].x == pacman->x) && (fantome[i].y == pacman->y))         //verificam daca pozitia fantomei este aceiasi cu cea a lui pacman
            {
                printf("GAME OVER! SCORUL FINAL ESTE:%d", pacman->puncte);
                closeSDL();
                exit(0);
            }
        }
    }
}

void deseneaza_cifra(int x, int y, int cifra)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);    // alb

    // definim fiecare cifra ca o serie de linii
    switch (cifra)
    {
    case 0:
        SDL_RenderDrawLine(renderer, x, y, x + 10, y);       // sus
        SDL_RenderDrawLine(renderer, x, y, x, y + 15);       // stanga
        SDL_RenderDrawLine(renderer, x + 10, y, x + 10, y + 15); // dreapta
        SDL_RenderDrawLine(renderer, x, y + 15, x + 10, y + 15); // jos
        break;
    case 1:
        SDL_RenderDrawLine(renderer, x + 5, y, x + 5, y + 15);   // verticala
        break;
    case 2:
        SDL_RenderDrawLine(renderer, x, y, x + 10, y);           // sus
        SDL_RenderDrawLine(renderer, x + 10, y, x + 10, y + 7);   // dreapta sus
        SDL_RenderDrawLine(renderer, x, y + 7, x + 10, y + 7);     // mijloc
        SDL_RenderDrawLine(renderer, x, y + 7, x, y + 15);       // stgnga jos
        SDL_RenderDrawLine(renderer, x, y + 15, x + 10, y + 15);  // jos
        break;
    case 3:
        SDL_RenderDrawLine(renderer, x, y, x + 10, y);       // sus
        SDL_RenderDrawLine(renderer, x + 10, y, x + 10, y + 15); // dreapta
        SDL_RenderDrawLine(renderer, x, y + 7, x + 10, y + 7);   // mijloc
        SDL_RenderDrawLine(renderer, x, y + 15, x + 10, y + 15); // jos
        break;
    case 4:
        SDL_RenderDrawLine(renderer, x, y, x, y + 7);        // stanga sus
        SDL_RenderDrawLine(renderer, x + 10, y, x + 10, y + 15); // dreapta
        SDL_RenderDrawLine(renderer, x, y + 7, x + 10, y + 7);   // mijloc
        break;
    case 5:
        SDL_RenderDrawLine(renderer, x + 10, y, x, y);       // sus
        SDL_RenderDrawLine(renderer, x, y, x, y + 7);        // stanga sus
        SDL_RenderDrawLine(renderer, x, y + 7, x + 10, y + 7);   // mijloc
        SDL_RenderDrawLine(renderer, x + 10, y + 7, x + 10, y + 15);// dreapta jos
        SDL_RenderDrawLine(renderer, x, y + 15, x + 10, y + 15); // jos
        break;
    case 6:
        SDL_RenderDrawLine(renderer, x + 10, y, x, y);       // sus
        SDL_RenderDrawLine(renderer, x, y, x, y + 15);       // stanga
        SDL_RenderDrawLine(renderer, x, y + 7, x + 10, y + 7);   // mijloc
        SDL_RenderDrawLine(renderer, x + 10, y + 7, x + 10, y + 15);// dreapta jos
        SDL_RenderDrawLine(renderer, x, y + 15, x + 10, y + 15); // jos
        break;
    case 7:
        SDL_RenderDrawLine(renderer, x, y, x + 10, y);       // sus
        SDL_RenderDrawLine(renderer, x + 10, y, x + 10, y + 15); // dreapta
        break;
    case 8:
        SDL_RenderDrawLine(renderer, x, y, x + 10, y);       // sus
        SDL_RenderDrawLine(renderer, x, y, x, y + 15);       // stanga
        SDL_RenderDrawLine(renderer, x + 10, y, x + 10, y + 15); // dreapta
        SDL_RenderDrawLine(renderer, x, y + 7, x + 10, y + 7);   // mijloc
        SDL_RenderDrawLine(renderer, x, y + 15, x + 10, y + 15); // jos
        break;
    case 9:
        SDL_RenderDrawLine(renderer, x, y, x + 10, y);       // sus
        SDL_RenderDrawLine(renderer, x, y, x, y + 7);        // stanga sus
        SDL_RenderDrawLine(renderer, x + 10, y, x + 10, y + 15); // dreapta
        SDL_RenderDrawLine(renderer, x, y + 7, x + 10, y + 7);   // mijloc
        break;
    }
}

void deseneaza_scor(Pacman* pacman)
{
    // Fundal scor
    SDL_Rect fundal = { 0, 0, M * CELL_SIZE, SPATIU_SCOR };        //formeaza un spatiu negru 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fundal);

    // Text "Scor:"
    //desenam S
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, 10, 15, 30, 15); // S sus
    SDL_RenderDrawLine(renderer, 10, 15, 10, 25); // S stanga sus
    SDL_RenderDrawLine(renderer, 10, 25, 30, 25); //S mijloc
    SDL_RenderDrawLine(renderer, 30, 25, 30, 35); // S dreapta jos
    SDL_RenderDrawLine(renderer, 10, 35, 30, 35); // S jos


    // DESENAM C
    SDL_RenderDrawLine(renderer, 40, 15, 60, 15); // C sus
    SDL_RenderDrawLine(renderer, 40, 15, 40, 35); // C stanga 
    SDL_RenderDrawLine(renderer, 40, 35, 60, 35); // C jos

    //DESENAM O
    SDL_RenderDrawLine(renderer, 70, 15, 90, 15);  //O sus
    SDL_RenderDrawLine(renderer, 70, 15, 70, 35);   //O stanga
    SDL_RenderDrawLine(renderer, 90, 15, 90, 35);   //O DREAPTA
    SDL_RenderDrawLine(renderer, 70, 35, 90, 35);   //O JOS

    //desenam R
    SDL_RenderDrawLine(renderer, 100, 15, 100, 35);  //R stanga
    SDL_RenderDrawLine(renderer, 100, 15, 120, 15);   //R sus
    SDL_RenderDrawLine(renderer, 120, 15, 120, 25); //R dreapta sus
    SDL_RenderDrawLine(renderer, 100, 25, 120, 25);   //mijloc
    SDL_RenderDrawLine(renderer, 100, 25, 120, 35);   //diagonala

    //desenam scorul actual 
    int start_x = 130;

    //scor curent
    if (pacman->puncte >= 10)
    {
        deseneaza_cifra(start_x, 20, pacman->puncte / 10);            //se deseneaza cifra zecilor
        start_x = start_x + 15;
    }
    deseneaza_cifra(start_x, 20, pacman->puncte % 10);            //se deseneaza cifra unitatilor
    start_x = start_x + 20;

    //bara de fractie
    SDL_RenderDrawLine(renderer, start_x, 25, start_x + 10, 25);
    start_x = start_x + 15;

    //scor maxim
    deseneaza_cifra(start_x, 20, NR_PUNCTE / 10);
    deseneaza_cifra(start_x + 15, 20, NR_PUNCTE % 10);

}

void deseneaza_fantoma(int x, int y)
{
    int centru_x = x * CELL_SIZE + CELL_SIZE / 2;
    int centru_y = y * CELL_SIZE + SPATIU_SCOR + CELL_SIZE / 2;
    int raza = CELL_SIZE / 2 - 2;         //raza pt capul fantomei

    //formam capul fanntomei
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);      //folosim culoarea rosie
    for (int i = -raza; i <= raza; i++)
    {
        for (int j = -raza; j <= 0; j++)              //doar jumatatea superioara
        {
            if (i * i + j * j <= raza * raza)
            {
                SDL_RenderDrawPoint(renderer, centru_x + i, centru_y + j);
            }
        }
    }

    //desenam partea de jos a fantomei
    for (int i = -raza; i <= raza; i = i + raza / 2)
    {
        SDL_RenderDrawLine(renderer, centru_x + i, centru_y, centru_x + i - raza / 4, centru_y + raza / 2);
        SDL_RenderDrawLine(renderer, centru_x + i - raza / 4, centru_y + raza / 2, centru_x + i + raza / 4, centru_y + raza / 2);
    }

    //desenam ochii
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, centru_x - raza / 3, centru_y - raza / 4);
    SDL_RenderDrawPoint(renderer, centru_x + raza / 3, centru_y - raza / 4);

}




void afiseaza_labirint_sdl(char labirint[N][M], Pacman* pacman, Fantoma fantome[])
{

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);       //sterge ecranul
    SDL_RenderClear(renderer);

    deseneaza_scor(pacman);                   //desenam scorul

    for (int i = 0; i < N; i++)        //parcurgem fiecare casuta a labirintului
    {
        for (int j = 0; j < M; j++)
        {
            SDL_Rect celula = { j * CELL_SIZE, i * CELL_SIZE + SPATIU_SCOR, CELL_SIZE, CELL_SIZE };

            if (labirint[i][j] == '#')
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);          // pereti albaștri
                SDL_RenderFillRect(renderer, &celula);
            }
            else if (labirint[i][j] == 'o')
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);          // punct alb
                int centru_x = j * CELL_SIZE + CELL_SIZE / 2;
                int centru_y = i * CELL_SIZE + SPATIU_SCOR + CELL_SIZE / 2;
                int raza = 4;

                for (int k = -raza; k <= raza; k++)
                {
                    for (int t = -raza; t <= raza; t++)
                    {
                        if (k * k + t * t <= raza * raza)
                        {
                            SDL_RenderDrawPoint(renderer, centru_x + k, centru_y + t);
                        }
                    }
                }
            }
            else if (labirint[i][j] == 'P')
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);            // pacman galben
                int centru_x = j * CELL_SIZE + CELL_SIZE / 2;               //pozitia pe axa x a centrului cercului
                int centru_y = i * CELL_SIZE + SPATIU_SCOR + CELL_SIZE / 2;     //pozitia pe axa y a centrului cercului
                int raza = CELL_SIZE / 2 - 2;                                   //raza cercului

                for (int k = 0; k < raza * 2; k++)
                {
                    for (int t = 0; t < raza * 2; t++)
                    {
                        int dx = raza - k;                   //distanta fata de centru
                        int dy = raza - t;
                        if ((dx * dx + dy * dy) <= (raza * raza))            //verificam ecuatia cercului
                        {
                            SDL_RenderDrawPoint(renderer, centru_x + dx, centru_y + dy);          //se deseneaza punctul 
                        }
                    }
                }
                //adaugam gura pt pacman
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);    //culoarea neagra pt gura lui pacman
                for (int unghi = 30; unghi <= 330; unghi++)
                {
                    if (unghi > 45 && unghi < 315)            //se formeaza o deschidere a gurii
                        continue;
                    float rad = unghi * M_PI / 180.0;        //transformam din grade in radiani 
                    int x = centru_x + (raza * cos(rad));        //coordonata x a marginii
                    int y = centru_y + (raza * sin(rad));         //coordonata y a marginii 
                    SDL_RenderDrawLine(renderer, centru_x, centru_y, x, y);          //desenam o linie de la centru la margine 

                }
            }
            else if (labirint[i][j] == 'F')
            {
                deseneaza_fantoma(i, j);
            }



        }
    }

    SDL_RenderPresent(renderer);

}



int main(int argc, char* argv[])
{
    char labirint[N][M];


    Pacman pacman = { 0 };
    pacman.simbol = 'P';          //simbol
    pacman.puncte = 0;            //puncte initiale 

    Fantoma fantome[NR_FANTOME] = { 0 };

    initSDL();              //initializare sdl

    formare_contur(labirint);
    plaseaza_pacman(&pacman, labirint, 1, 1);
    plaseaza_fantoma(fantome, labirint);


    int game_over = 0;
    SDL_Event event;




    while (!game_over) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game_over = 1;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_w: misca_pacman(labirint, &pacman, 'w'); break;
                case SDLK_s: misca_pacman(labirint, &pacman, 's'); break;
                case SDLK_a: misca_pacman(labirint, &pacman, 'a'); break;
                case SDLK_d: misca_pacman(labirint, &pacman, 'd'); break;
                case SDLK_q: game_over = 1; break;
                }
            }
        }
        counter++;

        if (counter % VITEZA_FANTOME == 0)
        {
            misca_fantoma(fantome, labirint, &pacman);
        }


       


        afiseaza_labirint_sdl(labirint, &pacman, fantome);          //redeseneaza labirintul


        if (pacman.puncte == NR_PUNCTE)                //verifica daca pacman a adunat toate punctele 
        {
            printf("AI CÂȘTIGAT! Scor final: %d\n", pacman.puncte);
            game_over = 1;
        }

        SDL_Delay(30);            //control viteza joc
    }

    closeSDL();
    return 0;
}
