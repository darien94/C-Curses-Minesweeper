#include<curses.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

int nrows, ncols, row, col, p, i, j, defcol=9, defrow=9, nrmine=10, steaguri, setflag[100][100],setmine[100][100],setok[100][100],setnum[100][100];
time_t start, stop;
	WINDOW* wnd;
char x;

int bombe_vecine(int row, int col){
int k=0;
	if(setmine[row-1][col]==1) k++;
	if(setmine[row-1][col+2]==1) k++;
	if(setmine[row-1][col-2]==1) k++;
	if(setmine[row+1][col]==1) k++;
	if(setmine[row+1][col-2]==1)k++;
	if(setmine[row+1][col+2]==1)k++;
	if(setmine[row][col-2]==1) k++;
	if(setmine[row][col+2]==1) k++;
return k;
}


void generare_mine(){
	int i, j, abscisa, ordonata,k=0;
	for(i=1;i<=nrows;i++)
	for(j=1;j<=ncols;j++)
		setmine[i][j]=0;
	srand(time(NULL));
		for(i=1;i<=nrmine;i++){
			abscisa=rand()%(defrow+3);
			ordonata=rand()%(2*defcol+2);
		     while(((abscisa+2)%2>defcol-1)||(abscisa<2)||(abscisa%2==1)||(ordonata<2)||(ordonata>defrow+1)||(setmine[ordonata][abscisa]==1)){
				abscisa=rand() % (2*defcol+2);
				ordonata=rand() % (defrow+3);
			}
			setmine[ordonata][abscisa]=1;
			mvaddch(ordonata,abscisa,'x');
		}
}


void lovire_casute_vecine(int a,int b){     //functie recursiva care verifica daca o casuta are bombe ca vecine...daca da, se va afisa numarul de mine vecine Daca nu, se va marca acea casuta cu un caracter "#", apoi se va relua procedeul pentru casutele vecine ei
int m,n,ok=0;

	if(setmine[a][b]==1)
		return;
	if(setnum[a][b]==1)
		return;
	if(setok[a][b]==1)
		return;
	
	if(bombe_vecine(a,b)!=0){
		mvprintw(a,b,"%d",bombe_vecine(a,b));
		setnum[a][b]=1;
		setok[a][b]=1;
		return;
	}			
	else{
		mvprintw(a,b,"#");
		setok[a][b]=1;
			for(m=2;m<defrow+2;m++)
			for(n=2;n<2*defcol+2;n=n++)
				if((a-m<=1)&&(a-m>=-1)&&(b-n<=2)&&(b-n>=-2)&&(n%2==0)&&(a+b-m-n<=3)&&(setok[m][n]==0)&&(setnum[m][n]==0))
				lovire_casute_vecine(m,n);	
	}
}

void joc_nou(){
	int g,ok;
	steaguri=nrmine;	//Numarul de steaguri va fi egal cu numarul de mine
	nrows=defrow+6;
	ncols=2*defcol+4;
	clear();
	refresh();
	curs_set(0);
	generare_mine();

		for(i=1;i<=defrow+3;i++)
			for(j=1;j<=2*defcol+2;j++)
			{
				setok[i][j]=0;     //Initializam vectorii care ne vor arata daca o anumita casuta a fost marcata sau daca ea afiseaza numarul de mine invecinate
				setnum[i][j]=0;
			}
	
	mvaddstr(0, 0, "Succes!");      //Desenare ecran principal de joc
	for(j=0;j<=ncols-2;j++)
		mvaddstr(1,j,"=");
	for(i=2;i<=nrows-5;i++){
		mvaddstr(i, 0, "||");
		for(j=2;j<=ncols-4;j=j+2)
		mvaddstr(i, j, "_|");
		mvaddstr(i, ncols-2, "|");
	}
	for(j=0;j<=ncols-2;j++)
		mvaddstr(nrows-4,j,"=");

	mvprintw(defrow+5, 2, "Timpul de joc: %ld ",clock());
	mvprintw(defrow+6, 2, "Timpul de la ultima mutare: %1.0f",difftime(stop,start));
	mvprintw(defrow+7, 2, "Bombe ramase:  %d",steaguri);
	noecho();

	row=(nrows-2)/2;	 //seteaza randul ca fiind cel din centrul tablei (am alocat spatiu pentru si pentru bordura tablei)
	
	if (defcol%2==0)	
		col=(2*defcol+1)/2;
	else
		col=(2*defcol+1)/2 +1;   //seteaza coloana ca fiind cea din centru tablei de joc

	move(row,col);   	//muta cursorul pe casuta din centru (sau una adiacenta)
	delch();
	insch('*');	//adauga un caracter "*" in centru, care va indica pozitia noastra in cadrul tabelei de joc (deplasabil cu tastele sageti)
	while(1){
		time(&start);
		ok=1;	
		p=getch();
		switch (tolower(p)) {
				case KEY_LEFT:
					attroff(A_STANDOUT);	//cred ca am adaugat si am sters caractere mai mult decat era suficient*
					if((setflag[row][col]!=1)&&(setok[row][col]!=1)){
						attroff(A_STANDOUT);	
						delch();
						insch('_'); 
					}
						else{ 
						if(setnum[row][col]==1)
						mvprintw(row,col,"%d",bombe_vecine(row,col));						
						else
						if(setok[row][col]==1)
						mvaddch(row,col,'#');
						else
						if(setflag[row][col]==1){
							attroff(A_STANDOUT);
							mvaddch(row,col,'T');
						}
					     }	

					col = MAX(col - 2, 2);
					move(row,col);
					attron(A_STANDOUT);
					if((setflag[row][col]!=1)&&(setok[row][col]!=1)){						
						delch();
						insch('*'); 
					}
					else{ 
						if(setnum[row][col]==1)
							mvprintw(row,col,"%d",bombe_vecine(row,col));			
						else
						if(setok[row][col]==1)
							mvaddch(row,col,'#');
						else
						if(setflag[row][col]==1)
							mvaddch(row,col,'T');
					     }					
					break;

				case KEY_RIGHT:
					attroff(A_STANDOUT);
					if((setflag[row][col]!=1)&&(setok[row][col]!=1)){
						attroff(A_STANDOUT);						
						delch();
					 	insch('_');
					}

					else{ 
						if(setnum[row][col]==1)
						mvprintw(row,col,"%d",bombe_vecine(row,col));						
						else
						if(setok[row][col]==1)
						mvaddch(row,col,'#');
						else
						if(setflag[row][col]==1)
						mvaddch(row,col,'T');
					}

					col = MIN(col + 2, ncols - 4);
					attron(A_STANDOUT);
					move(row,col);
					if((setflag[row][col]!=1)&&(setok[row][col]!=1)){						
					delch();
					insch('*');
					}

					else{ 
						if(setnum[row][col]==1)
							mvprintw(row,col,"%d",bombe_vecine(row,col));						
			
						else
						if(setok[row][col]==1)
							mvaddch(row,col,'#');
						else
						if(setflag[row][col]==1)
							mvaddch(row,col,'T');
					}
					break;
				case KEY_UP:
					attroff(A_STANDOUT);
					if((setflag[row][col]!=1)&&(setok[row][col]!=1)){
						attroff(A_STANDOUT);						
						delch();
						insch('_');
					}
					else{ 
						if(setnum[row][col]==1)
							mvprintw(row,col,"%d",bombe_vecine(row,col));						
			
						else
						if(setok[row][col]==1)
							mvaddch(row,col,'#');
						else
						if(setflag[row][col]==1)
							mvaddch(row,col,'T');
					}
					row = MAX(row - 1, 2);
					move(row,col);
					attron(A_STANDOUT);
					if((setflag[row][col]!=1)&&(setok[row][col]!=1)){						
						delch();
						insch('*');
					}
					else{ 
						if(setnum[row][col]==1)
							mvprintw(row,col,"%d",bombe_vecine(row,col));						
						else
						if(setok[row][col]==1)
							mvaddch(row,col,'#');
						else
						if(setflag[row][col]==1)
							mvaddch(row,col,'T');
					}
						break;
				case KEY_DOWN:
						attroff(A_STANDOUT);
					if((setflag[row][col]!=1)&&(setok[row][col]!=1)){
						attroff(A_STANDOUT);					
						delch();
						insch('_');
						attroff(A_STANDOUT);
					}
					else{ 
						if(setnum[row][col]==1)
							mvprintw(row,col,"%d",bombe_vecine(row,col));						
						else
						if(setok[row][col]==1)
							mvaddch(row,col,'#');
						else
						if(setflag[row][col]==1)
							mvaddch(row,col,'T');
					}
					row = MIN(row + 1, nrows - 5);
					move(row,col);
					attron(A_STANDOUT);
					if((setflag[row][col]!=1)&&(setok[row][col]!=1)){						
						delch();
						insch('*');
					}
				
					else{ 
						if(setnum[row][col]==1)
							mvprintw(row,col,"%d",bombe_vecine(row,col));						
			
						else
						if(setok[row][col]==1)
							mvaddch(row,col,'#');
						else
						if(setflag[row][col]==1)
							mvaddch(row,col,'T');
					}
					break;
				case 'x':	//in caz de apasarea tastei SPACE pe o anumita casuta..daca se loveste o bomba, jocul se termina 
						attroff(A_STANDOUT);					
						if(setmine[row][col]==1){
						mvaddstr(0,0,"Game over! --apasati tasta ESC pentru a reveni la meniul principal");
						for(i=1;i<=defrow+3;i++)
							for(j=1;j<2*defcol+2;j++)
								if(setmine[i][j]==1)        //afisare mine pe teren
								mvaddch(i,j,'X');
						refresh();
						while(1){
							g=getch();
							if(g==27){
								clear();
								meniu_principal();
							}
						}
					}
					else				
						lovire_casute_vecine(row,col);	
						break;	
				case 'm':	/*In cazul apasarii tastei "m" se va marca celula curenta cu un "T"(steag) si se va demarca in caz ca a fost adaugat deja un steag */		
					attroff(A_STANDOUT);
					if(setflag[row][col]==1){
						delch();
				        attron(A_NORMAL);                      //daca exista un steag, atunci se va sterge
						insch('_');
					attron(A_NORMAL);
					attroff(A_NORMAL);
						steaguri++;
						mvprintw(defrow+7, 2, "Bombe ramase:   %d  ",steaguri);
						setflag[row][col]=0;						
						refresh();
					}
					else{
						delch();					//daca nu exista un steag, atunci se va adauga
						insch('T');
						steaguri--;
						mvprintw(defrow+7, 2, "Bombe ramase:   %d  ",steaguri);
						refresh();
						setflag[row][col]=1;	
					}
					break;	
				case 27:		//Daca se apasa tasta ESC se revine la meniul principal
					attroff(A_STANDOUT);					
					meniu_principal();
					break;
		}
				for(i=2;i<=defrow+2;i++)
				for(j=2;j<=2*defcol+2;j=j+2)
					if((setmine[i][j]==0)&&(setflag[i][j]==0)&&(setok[i][j]==0)&&(setnum[i][j]==0)) ok=0; //daca exista casute pe care nu exista bombe si nu exista steag si nu este marcata sau nu afiseaza numarul de bombe alaturate, atunci jocul nu este complet
				if(ok==1)
					mvprintw(0,0,"FELICITARI! AI CASTIGAT!         ");  //Daca jocul este complet, se afiseaza un mesaj de victorie
				time(&stop);

				attroff(A_STANDOUT);
			//	mvprintw(defrow+6, 2, "Timpul de la ultima mutare: %1.0f",difftime(stop,start));
				refresh();
	}
}


void Settings()
{
	int rand=2, coloana=0, x,c;
	clear();							//Desenare ecran de setari
	mvaddstr(0,10,"Setari");			
	mvaddstr(2,2, "Numarul de linii ale tabelei (default 9): ");
	mvaddstr(3,2, "Numarul de coloane ale tabelei (default 9): ");
	mvaddstr(4,2,"Numarul de mine (default 10)");
	mvaddstr(5,2,"Incepe jocul");	
	mvaddstr(6,2, "Dificultate scazuta");
	mvaddstr(7,2, "Dificultate medie");
	mvaddstr(8,2, "Dificultate ridicata");

	move(rand,coloana);
	insch('*');						// Deplasarea se va face cu un caracter steluta
	while(1){
		c=getch();
		switch(c){
			case KEY_DOWN:
				if(rand<8){			//doar in sus si in jos in cadrul meniului
					delch();
					rand=rand+1;
					move(rand,coloana);
					insch('*');
				}
				break;
			case KEY_UP:
				if(rand>2){
					delch();
					rand=rand-1;
					move(rand,coloana);
					insch('*');
				}
				break;
			case 32:				// Daca se apasa tasta space, se va selecta optiunea din dreptul stelutei
				if(rand==2) {
					scanf("%d",&x);		//Se va selecta cu tasta space optiunea, apoi se va introduce numarul dorit urmat de 										tasta enter
					defrow=x;
				}
				if(rand==3) {
					scanf("%d",&x);
					defcol=x;
				}
				if(rand==4) {
					scanf("%d",&x);
					nrmine=x;
				}
				if(rand==5)
				joc_nou();
				if(rand==6){
					nrmine=6;
					defrow=6;
					defcol=6;
				}
				if(rand==7){
					nrmine=15;
					defrow=12;
					defcol=12;
				}
				if(rand==8){
					nrmine=30;
					defrow=16;
					defcol=16;
				}								
				break;
		}
	}
}


void meniu_principal()
{	
	clear();	
	int rand=2,coloana=0;
	mvaddstr(0, 0, "=+=+=+=+=+=+=+=+=+=+=+=+=+=+");               //Desenare meniu principal
	mvaddstr(1, 3, "Meniu Minesweeper (^_^)");
	mvaddstr(2, 1, "New Game");
	mvaddstr(3, 1, "Settings");
	mvaddstr(4, 1, "Quit");
	mvaddstr(5, 0, "=+=+=+=+=+=+=+=+=+=+=+=+=+=+");
	move(rand,coloana);
	insch('*');
	while(1){
		p=getch();
		switch(p){
			case KEY_DOWN:
				if(rand<4){			
					delch();
					rand=rand+1;
					move(rand,coloana);
					insch('*');
				}
				break;
			case KEY_UP:
				if(rand>2){
					delch();
					rand=rand-1;
					move(rand,coloana);
					insch('*');
				}
				break;
			case 32:
				if(rand==2) joc_nou();
				if(rand==3) Settings();
				if(rand==4) endwin();
				break;
		}	
	}
}


int main()
{
	wnd=initscr();

	refresh();
	curs_set(0);
	clear();
	keypad(wnd,TRUE);	
	meniu_principal();

	x=getch();
	if(tolower(x)=='q')
		endwin();
return 1;	
}
