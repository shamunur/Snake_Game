#include<iostream>
#include<windows.h>
#include<conio.h>
#include <unordered_set>
using namespace std;

int height = 25;
int width = 90;
int gameover = 0, counter;
int lflag = 0, rflag = 0, uflag = 0, dflag = 0;

class Snake {
    int x, y, fx, fy;

    struct node {
        int nx, ny;
        struct node* next;
        struct node* prev;
    };

    struct node* head = NULL;

public:

    /* Manages the visibility of the cursor in the console.*/
    void cursorvisibility(bool x) {
        HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(out, &cursorInfo);
        cursorInfo.bVisible = x;
        SetConsoleCursorInfo(out, &cursorInfo);
    }

    /* Sets the console cursor position to the specified coordinates (x, y) using WinAPI's */
    void gotoxy(int x, int y){
        COORD pos = { x, y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }

   /**[Screen Management] */

    /*  Displays the game screen borders */
    int setBorder(int w1,int w2,int h1,int h2){
        // upper and lower wall
        for (int i = w1; i <= w2; i++) {
            gotoxy(i, h1);
            cout << "#";
            gotoxy(i, h2);
            cout << "#";
        }
        // left and right wall
        for (int i = h1; i <= h2; i++) {
            gotoxy(w1, i);
            cout << "#";
            gotoxy(w2, i);
            cout << "#";
        }
    }

    /*  Displays the current score and ESC functionality */
    void sideScreen(){
        gotoxy(99, 9);
        cout << "WELCOME TO ";
        gotoxy(95, 10);
        cout <<"*** SNAKE GAME ***";
        gotoxy(94, 11);
        cout <<"********************";
        gotoxy(99, 13);
        cout <<" SCORE = " << counter * 10;
        gotoxy(94, 20);
        cout << " ~ Press ESC to Pause";
    }

	/* Displays the game-over screen, the player's score */
    int finalScreen(){
        setBorder(width/4, width/4+width/2, height/4, height/4+height/2);

        gotoxy( width/2 - 9, height/2 - 2 );
        cout << "*** GAME OVER ***";
        gotoxy( width/2 - 10, height/2 - 1 );
        cout << "*******************";

        gotoxy(width/2 - 9, height/2+2 );
        cout << " You Scored : " << counter * 10;
        cursorvisibility(false);

        Sleep(15000); // Pause for 1.5 seconds before clearing the screen
        system("cls");
    }


   /**[Fruit and Snake Management] */

    /* Create the Fruit */
    void createFruit(){
        gotoxy(fx, fy);
        cout << "@";
    }

    /* Create the Snake */
    void createSnake(struct node* head){
        struct node* ptr = head;
        int count = 0;
        while (ptr != NULL) {
            gotoxy(ptr->nx, ptr->ny);
            if (count == 0) {
                cout << "O"; // head
            } else {
                cout << "o"; // body
            }
            ptr = ptr->next;
            count++;
        }
    }

    /* Create the Snake's Body */
	void createBody(struct node* head, int x, int y){
		struct node* ptr, * previous;
		ptr = head;
		previous = head;

		while (ptr->next != NULL){
			previous = ptr;
			ptr = ptr->next;
		}
		while (previous != head){
			ptr->nx = previous->nx;
			ptr->ny = previous->ny;
			previous = previous->prev;
			ptr = ptr->prev;
		}
		ptr->nx = previous->nx;
		ptr->ny = previous->ny;
		previous->nx = x;
		previous->ny = y;
	}

    /* Remove the Snake from the Screen */
    void removeSnake(struct node* head){
        struct node* ptr = head;
        while (ptr != NULL) {
            gotoxy(ptr->nx, ptr->ny);
            cout << " ";
            ptr = ptr->next;
        }
    }

    /* Snake's Initial Position */
    void setInitialPosition() {
        head = new node;
        // Set the initial coordinates for the snake head at the center of the game grid.
        head->nx = width / 2;
        head->ny = height / 2;

        head->next = NULL;
        head->prev = NULL;

        // Set the x and y variables to the same values as the head's coordinates.
        x = width / 2;
        y = height / 2;
    }

    /* Illustrate the snake and the fruit */
    void draw(){
        createSnake(head);
        createFruit();
        cursorvisibility(false);
        Sleep(70);

        removeSnake(head);
        cursorvisibility(true);
    }

   /**[Snake Movement] */

    /* Resets the Movement flags to their default values.*/
    void resetflag(){
		uflag = 0;
		dflag = 0;
		lflag = 0;
		rflag = 0;
	}

    /* Handles keyboard inputs for movement and game pause*/
	void snakeMove(){
		int h;
		char ch;
		// If a key is pressed
		if (_kbhit()){
			ch = _getch();   // Store the pressed key
			h = ch;         // store the ascii code of the pressed key
            switch (h){
                case 72:
                    if (dflag != 1){
                        resetflag();
                        uflag = 1;
                    }
                    break;
                case 80:
                    if (uflag != 1){
                        resetflag();
                        dflag = 1;
                    }
                    break;
                case 75:
                    if (rflag != 1){
                        resetflag();
                        lflag = 1;
                    }
                    break;
                case 77:
                    if (lflag != 1){
                        resetflag();
                        rflag = 1;
                    }
                    break;
                // to pause the game if Esc is pressed
                case 27:
                {
                    gotoxy(0, 26);
                    system("pause");
                    //overwrite characters with spaces,clearing that line on the console screen
                    for (int i = 0; i < 31; i++){
                        // moves the cursor from right to left (x = 31 to x = 1) and replace chars with space in reverse.
                        gotoxy(31 - i, 26);
                        cout << "\b" << " ";
                    }
                }
                    break;
                default:
                    break;
			}
		}
	}

    /* Controls the Snake's movement direction */
	void moveRoute(){
		if (uflag == 1){
            y--;
		}
		else if (dflag == 1){
            y++;
		}
		else if (lflag == 1){
            x--;
		}
		else if (rflag == 1){
            x++;
		}
	}

   /**[Fruit Consumption and Update Body] */

	/* Generates a random position for the fruit on the screen */
    void generatefruit() {
        do {
            fx = rand() % width;
        } while (fx == 0 || fx == width);

        do {
            fy = rand() % height;
        } while (fy == 0 || fy == height);
    }


    /* Checks for fruit consumption and update counter and score */
    void consumeFruit() {
        // Checks if the Snake's head position matches the fruit position
        if (x == fx && y == fy) {
            struct node* temp = new node;
            temp->next = NULL;
            temp->prev = NULL;
            struct node* ptr = head;
            while (ptr->next != NULL) {
                ptr = ptr->next;
            }
            ptr->next = temp;
            temp->prev = ptr;

            // generate fruit, update counter and score
            generatefruit();
            counter++;
            sideScreen();
        }
    }

    /* Updates the Snake's body if the snake consume fruit */
    void updateSnakeBody(){
        consumeFruit();
        createBody(head, x, y);
    }

    /**[Snake Crash Detection] */

    /* Checks if the Snake crash the screen Border */
	bool checkBorderCrash(){
        if (x == width || x == 0 || y == height || y == 0) {
            gotoxy(fx, fy);   // Clears the position of the fruit
            cout << " ";
            return true;
        }
        return false;
    }

    /* Checks if Snake crash with itself */
    bool checkSelfCrash(){
        // track unique x and y coordinates of the snake body nodes.
        unordered_set<int> mapx;
        unordered_set<int> mapy;
        struct node* current = head;
        while (current != NULL) {
            //inserts unique x and y coordinates into the sets.
            mapx.insert(current->nx);
            mapy.insert(current->ny);
            if (current != head && x == current->nx && y == current->ny) {
                gotoxy(fx, fy);
                cout << " ";
                return true;
            }
            current = current->next;
        }
        return false;
    }

   /* Handles crash detection of the Snake between the Borders or Itself */
    void checkCrash(){
        if ( checkBorderCrash() || checkSelfCrash() ) {
            gameover = 1;   // Set the game is over.
        }
    }

    /**[Main Setup] */

    /* Setup all functions */
    void setup(){
		counter = 0;
		gameover = 0;

		setBorder(0,width,0,height);
		sideScreen();
        generatefruit();
		setInitialPosition();
	}

    /* Controls the main game loop */
	int game(){
		setup();
		while (!gameover)
		{
			draw();
			snakeMove();
			moveRoute();
			checkCrash();
			updateSnakeBody();
		}
		finalScreen();
	}

};


int main() {
    Snake s;

    s.game();

    return 0;
}

