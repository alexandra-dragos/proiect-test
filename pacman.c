#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <conio.h>

#define N 21             //nr de linii
#define M 21               //nr de coloane
#define NR_OBSTACOLE 45       //nr de obstacole
#define NR_PUNCTE 15        //nr de puncte
#define NR_FANTOME 4       //nr fantomelor


typedef struct 
{
    int x;        //coordonata fantomei pe linii
    int y;        //coordonata fantomei pe coloana
    int simbol;   //simbol pt fantoma
}Fantoma;



typedef  struct 
{
    int x;                    //coordonata lui Pacman pe linie
    int y;                    // coordonata lui Pacman pe coloana
    int simbol;               //simbolul cu care este reprezentat Pacman
    int puncte;              //scorul
}Pacman;


void formare_contur(char labirint[N][M])
{
    for(int i=0; i<N; i++)
    {
        for(int j=0; j<M; j++)
        {
            if(i==0 || j==0 || i==N-1 || j==M-1)
            {
               labirint[i][j]='#';                     // formam peretii labirintului 
            }
            else
            {
                labirint[i][j]=' ';                       // in interiorul sau punem spatii libere
            }
        }
    }
    srand(time(NULL));
    for(int k=0; k<NR_OBSTACOLE; k++)
    {
        int rand_i;                       // variabila care ne da coordonata pt linie
        int rand_j;                         // variabila care ne da coordonata pt coloana
        do
        {
            rand_i=rand()%(N-2)+1;                    //evitam marginile si umplem labirintul cu obstacole
            rand_j=rand()%(M-2)+1;

        } while (labirint[rand_i][rand_j]=='#');
        labirint[rand_i][rand_j]='#';                        //punem obstacolul
        
    }
    for(int i=0; i<NR_PUNCTE; i++)
    {
        int rand_x;
        int rand_y;
        do
        {
            rand_x=rand()%(N-2)+1;                     //coordonata pe linie
            rand_y=rand()%(M-2)+1;                      //coordonata pe coloana
            
        } while (labirint[rand_x][rand_y]=='#');
        labirint[rand_x][rand_y]='o';                       //simbol pt puncte
        

    }

}

void clear_screen()
{
    #ifdef _WIN32
        system ("cls");
    #else
        system("clear");
    #endif
}



void misca_pacman(char labirint[N][M], Pacman* pacman, char directie)
{   
    
    labirint[pacman->x][pacman->y]=' ';                 //stergem pozitia curenta
    int x_nou=pacman->x;                              //variabila pt pozitia lui Pacman pt coordonata de pe linie 
    int y_nou=pacman->y;                             //variabila pt pozitia lui Pacman pt coordonata de pe coloana

    //ACTUALIZARE POZITIE PACMAN 
    if((directie=='W' || directie=='w') && labirint[x_nou-1][y_nou]!='#')          //miscare in sus
    {
        x_nou--;
    }
    else if((directie=='S' || directie=='s') && labirint[x_nou+1][y_nou]!='#')     //miscare in jos
    {
        x_nou++;
    }
    else if((directie=='D' || directie=='d') && labirint[x_nou][y_nou+1]!='#')     //miscare la dreapta
    {
        y_nou++;
    }
    else if((directie=='A' || directie=='a') && labirint[x_nou][y_nou-1]!='#')             //miscare la stanga
    {
        y_nou--;
    }
    
    
    if(labirint[x_nou][y_nou]=='o')       //verificam daca pozitia lui pacman este aceiasi cu pozitia punctului
    {
        pacman->puncte++;                            //crestem scorul
        labirint[x_nou][y_nou]=' ';          //luam punctul
    }
    pacman->x=x_nou;
    pacman->y=y_nou;
    labirint[pacman->x][pacman->y]='P';                                 // plasam in noua pozitie
}

void plaseaza_pacman(Pacman* pacman, char labirint[N][M], int x, int y)
{
    if(labirint[x][y]==' ' || labirint[x][y]=='o')                   //verificam sa nu fie obstacol sau margine
    {
        pacman->x=x;
        pacman->y=y;
        labirint[x][y]='P';
    }
    else{
        printf("LOCATIA ALEASA ESTE OCUPATA!\n");
    }
}

void afisare_labirint(char labirint[N][M], Pacman* pacman, Fantoma fantome[])
{
    char temp[N][M];                  //labirint temporar
    for(int i=0; i<N; i++)
    {
        for(int j=0; j<M; j++)
        {
            temp[i][j]=labirint[i][j];       //copiez labirintul in temp
        }
    }
    temp[pacman->x][pacman->y]=pacman->simbol;
    for(int i=0; i<NR_FANTOME; i++)                          //pun fantomele in temp
    {
        temp[fantome[i].x][fantome[i].y]=fantome[i].simbol;
    }
    for(int i=0; i<N; i++)
    {
        for(int j=0; j<M; j++)
        {
            printf( "%c", temp[i][j]);
        }
        printf("\n");
    }
    printf("SCORUL:%d\n", pacman->puncte);
}

void plaseaza_fantoma(Fantoma fantome[], char labirint[N][M])
{
    for(int i=0; i<NR_FANTOME; i++)
    {
        int fantoma_x, fantoma_y;                  //coordonatele pt pozitia unei fantome
        do
        {
            fantoma_x=rand()%(N-2)+1;
            fantoma_y=rand()%(M-2)+1;            //evitam marginile
        } while (labirint[fantoma_x][fantoma_y]!=' ');                  //punem fantoma pe spatiile libere
        fantome[i].x=fantoma_x;                        //coordonatele unei fantome
        fantome[i].y=fantoma_y;
        fantome[i].simbol='F';                     //simbolul fantomei
        labirint[fantoma_x][fantoma_y]=fantome[i].simbol;               //se pune fantoma pe pozitie in labirint
        
    }
}

void urmareste_pacman(Fantoma *fantoma, char labirint[N][M], Pacman *pacman)
{
    int x_actual=fantoma->x;       //pozitia actuala pe linie
    int y_actual=fantoma->y;        //pozitia actuala pe coloana
    int x_apropiere=x_actual;        // cea mai buna pozitie a fantomei pt a se apropia de Pacman
    int y_apropiere=y_actual;    
    int min=9999;
    int directie_x[]={-1, 1, 0, 0};     //vector pt directie -1 sus si 1 jos 
    int directie_y[]={0, 0, -1, 1};     //vector pt directie -1 stanga si 1 dreapta 

    for(int i=0; i<4; i++)
    {
        int pozVecina_x=x_actual+directie_x[i];          //pt fiecare directie se calculeaza pozitia vecina 
        int pozVecina_y=y_actual+directie_y[i];

        if(labirint[pozVecina_x][pozVecina_y]==' ' || labirint[pozVecina_x][pozVecina_y]=='o')
        {
            int dist=abs(pozVecina_x - pacman->x)+abs(pozVecina_y - pacman->y);  //calculam cat de departe ar fi fantoma de pacman daca ar merge in acea directie
            if(dist<min)
            {
                min=dist;
                x_apropiere=pozVecina_x;            //se actualizeaza pozitia fantomei
                y_apropiere=pozVecina_y;
            }
        }
    }
    if(x_apropiere!=x_actual || y_apropiere!=y_actual)
    {
        labirint[x_actual][y_actual]=' ';      //se sterge simbolul vechi din labirint 
        fantoma->x = x_apropiere;              //se actualizeaza pozitia fantomei 
        fantoma->y = y_apropiere;
        labirint[fantoma->x][fantoma->y]=fantoma->simbol;            //se plaseaza fantoma pe cea mai buna pozitie 

    }
    //verificare pozitiei fantomei si a lui pacman 
    if(fantoma->x == pacman->x && fantoma->y== pacman->y)
    {
        clear_screen();
        printf("GAME OVER! SCORUL FINAL ESTE:%d\n",pacman->puncte);
        exit(0);
    }

}

void misca_fantoma(Fantoma fantome[], char labirint[N][M], Pacman *pacman)
{
    for(int i=0; i<NR_FANTOME; i++)
    {
        if(i<2)                           //primele 2 fantome il urmaresc pe pacman 
        {
            urmareste_pacman(&fantome[i], labirint, pacman);
        }
        else                              
        {
            int directie=rand()%4;        //0-sus 1-jos 2-stanga 3-dreapta 
            int x_nou=fantome[i].x;       //variabila pt coordonata fantomei
            int y_nou=fantome[i].y;

            //stabilim pozitia
            if (directie==0)
                x_nou--;
            else if(directie==1)
                x_nou++;
            else if(directie==2)
                y_nou--;
            else if(directie==3)
                y_nou++;
        
            //verificam daca pozitia este valida
            if(labirint[x_nou][y_nou]==' ' || labirint[x_nou][y_nou]=='o')
            {
                labirint[fantome[i].x][fantome[i].y]=' ';         //stergem vechea pozitie 
                fantome[i].x=x_nou;
                fantome[i].y=y_nou;                               //actualizam noua pozitie 
                labirint[fantome[i].x][fantome[i].y]=fantome[i].simbol;        //punem fantoma pe pozitie 
            
            }
            else if(fantome[i].x==pacman->x && fantome[i].y==pacman->y)         //verificam daca pozitia fantomei este aceiasi cu cea a lui pacman
            {
                printf("GAME OVER! SCORUL FINAL ESTE:%d", pacman->puncte);
                exit(0);
            }
        }
    }
}   

int main(void)
{
    char labirint[N][M];
    

    Pacman pacman;
    pacman.simbol='P';          //simbol
    pacman.puncte=0;            //puncte initiale 

    Fantoma fantome[NR_FANTOME];

    formare_contur(labirint);
    
    //COMENZILE NECESARE:\n W-merge in sus\n S-merge in jos\n A-merge la dreapta\n D-merge la stanga

    int x_start=1;
    int y_start=1;             //plasam intr-o locatie valida
    plaseaza_pacman(&pacman, labirint, x_start, y_start);
    plaseaza_fantoma(fantome,labirint);

    clear_screen();
    afisare_labirint(labirint, &pacman, fantome);


    char directie;
    while(1)
    {
        printf("INTRODU O DIRECTIE:");
        directie=getch();
        if(pacman.puncte==NR_PUNCTE)
            {
                printf("AI COLECTAT TOATE PUNCTELE. SCORUL TAU ESTE:%d\n", pacman.puncte);
                break;
            }
        else if (directie=='Q' || directie=='q')
        {
            printf("AI IESIT DIN JOC. SCORUL ESTE:%d", pacman.puncte);
            break;
        }
    
        
        
        misca_pacman(labirint, &pacman, directie);
        misca_fantoma(fantome, labirint, &pacman);
        clear_screen();
        afisare_labirint(labirint, &pacman, fantome);
    }
    
return 0;
}
