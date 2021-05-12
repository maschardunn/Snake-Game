#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

void initGame();
void mainMenu();
void lossPrompt(int score);
void createBounds(int gameStartY, int gameStartX, int gameHeight, int gameWidth);

int main(void) {
    initGame();

    int gameHeight = 16;
    int gameWidth = 32;
    int gameStartY = (LINES / 2) - (gameHeight / 2);
    int gameStartX = (COLS / 2) - (gameWidth / 2);
    int gameEndY = (LINES / 2) + (gameHeight / 2);
    int gameEndX = (COLS / 2) + (gameWidth / 2);

    int xVel = 1;
    int yVel = 0;
    int snakeSize = 4;
    int **pos=malloc(snakeSize * sizeof(int *));
    int startY = 0;
    int startX = 0;
    int score = 0;
    int foodBoundsStartY = gameStartY + 1;
    int foodBoundsStartX = gameStartX + 1;
    int foodPosY = rand() % (gameHeight-1) + foodBoundsStartY;
    int foodPosX = rand() % (gameWidth-1) + foodBoundsStartX;

    createBounds(gameStartY, gameStartX, gameHeight, gameWidth);
    mainMenu();

    for(int i = 0; i < snakeSize; i++) {
        pos[i] = malloc(sizeof(int)*2);
    }

    for(int i=0; i<snakeSize; i++) {
        pos[i][0] = LINES/2;
        pos[i][1] = COLS/2-i;
    }

    createBounds(gameStartY, gameStartX, gameHeight, gameWidth);

    nodelay(stdscr, TRUE);

    bool gameOver = false;
    while(!gameOver) {
        srand(time(NULL));

        int key = wgetch(stdscr);
        switch(key) {
            case KEY_UP:
                yVel=-1;
                xVel=0;
                break;
            case KEY_DOWN:
                yVel=1;
                xVel=0;
                break;
            case KEY_LEFT:
                yVel=0;
                xVel=-1;
                break;
            case KEY_RIGHT:
                yVel=0;
                xVel=1;
                break;
            case 'q':
                gameOver = true;
                break;
            case 'p':
                nodelay(stdscr, FALSE);
                mvprintw(LINES / 2, (COLS / 2) - 3, "PAUSED");
                do {
                    key = wgetch(stdscr);

                } while(key != 'p');

                nodelay(stdscr, TRUE);
                break;

        }

        // Game boundaries
        if(pos[0][0] == gameStartY)
            gameOver = true;
        else if(pos[0][1] == gameStartX)
            gameOver = true;
        else if(pos[0][0] == gameEndY)
            gameOver = true;
        else if(pos[0][1] == gameEndX)
            gameOver = true;

        // Check if head collides with itself
        for(int i=snakeSize-1; i > 0; i--) {
            if(pos[0][0] == pos[i][0] && pos[0][1] == pos[i][1])
                gameOver = true;

        }

        // Check to make sure food doesn't spawn under snake
        for(int i=snakeSize-1; i > 0; i--) {
            if(foodPosY == pos[i][0] && foodPosX == pos[i][1]) {
                foodPosY = rand() % (gameHeight-1) + foodBoundsStartY;
                foodPosX = rand() % (gameWidth-1) + foodBoundsStartX;
            }
        }

        clear();
        mvprintw(gameStartY - 1, (COLS / 2) - 5, "SNAKE GAME");
        createBounds(gameStartY, gameStartX, gameHeight, gameWidth);
        mvprintw(gameEndY + 1, (COLS / 2) - 3, "Score: %d", score);
        mvaddch(foodPosY, foodPosX, '#');

        if(pos[0][0] == foodPosY && pos[0][1] == foodPosX) {
            score++;
            snakeSize++;

            foodPosY = rand() % (gameHeight - 1) + foodBoundsStartY;
            foodPosX = rand() % (gameWidth - 1) + foodBoundsStartX;

            pos = realloc(pos, snakeSize*sizeof(int *));
            pos[snakeSize-1]=malloc(2*sizeof(int));

        }

        for(int i=0; i<snakeSize; i++) {
            mvaddch(pos[i][0]+startY, pos[i][1]+startX, 'O');
        }
        for(int i = snakeSize-1; i > 0; i--) {
            pos[i][0] = pos[i-1][0];
            pos[i][1] = pos[i-1][1];
        }

        pos[0][0]+=yVel;
        pos[0][1]+=xVel;

        // Speed left/right as score gets higher
        if(yVel == 0 && xVel == 1)
            usleep(125000 - (score * 5000);
        // Speed up/down as score gets higher
        else
            usleep(150000 - (score * 5000);

    }

    nodelay(stdscr, false);
    lossPrompt(score);
    int key;
    do {
        key = wgetch(stdscr);

    } while (key != 'q');

    endwin();
}

// Initialize game-sensitive curses settings
void initGame() {
    initscr();
    curs_set(0);
    noecho();
    keypad(stdscr, TRUE);
    srand(time(NULL));
}

void mainMenu() {

    char *title = "SNAKE GAME";
    char *author = "By: Mason Dunn";
    char *tooltip = "Press \"Enter\" to begin";

    mvprintw((LINES / 2) - 1, (COLS / 2) - (strlen(title) / 2), title);
    mvprintw((LINES / 2), (COLS / 2) - (strlen(author) / 2), author);
    mvprintw((LINES / 2) + 1, (COLS / 2) - (strlen(tooltip) / 2), tooltip);

    int key;
    do {
        key = wgetch(stdscr);

    } while (key != 10);
}

void lossPrompt(int score) {
    nodelay(stdscr, false);
    char *prompt = "You lose!";

    mvprintw((LINES / 2), (COLS / 2) - (strlen(prompt) / 2), prompt);
    mvprintw((LINES / 2) + 1, (COLS / 2) - 8, "Your score was: %d", score);
}

// Create game area
void createBounds(int gameStartY, int gameStartX, int gameHeight, int gameWidth) {
    mvaddch(gameStartY, gameStartX, ACS_ULCORNER);
    mvaddch(gameStartY, gameStartX + gameWidth, ACS_URCORNER);
    mvaddch(gameStartY + gameHeight, gameStartX, ACS_LLCORNER);
    mvaddch(gameStartY + gameHeight, gameStartX + gameWidth, ACS_LRCORNER);

    for(int i = gameStartY+1; i < gameStartY + gameHeight; i++) {
        mvaddch(i, gameStartX, ACS_VLINE);
    }
    for(int i = gameStartY+1; i < gameStartY + gameHeight; i++) {
        mvaddch(i, gameStartX + gameWidth, ACS_VLINE);
    }

    for(int i = gameStartX+1; i < gameStartX + gameWidth; i++) {
        mvaddch(gameStartY, i, ACS_HLINE);
    }
    for(int i = gameStartX+1; i < gameStartX + gameWidth; i++) {
        mvaddch(gameStartY + gameHeight, i, ACS_HLINE);
    }

}
