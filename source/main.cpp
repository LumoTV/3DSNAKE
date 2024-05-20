#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int width = 20;
const int height = 15;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;
bool gameOver;
bool gameStarted;

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    srand(time(NULL));
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
}

void GenerateFruit() {
    do {
        fruitX = rand() % width;
        fruitY = rand() % height;
    } while (x == fruitX && y == fruitY);
}

void Draw() {
    consoleClear();
    printf("Score: %d\n", score);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (i == y && j == x)
                printf("O");
            else if (i == fruitY && j == fruitX)
                printf("F");
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        printf("o");
                        print = true;
                    }
                }
                if (!print)
                    printf(" ");
            }
        }
        printf("\n");
    }
}

void Input() {
    scanKeys();
    u16 keys = keysDown();

    if (keys & KEY_LEFT && dir != RIGHT)
        dir = LEFT;
    else if (keys & KEY_RIGHT && dir != LEFT)
        dir = RIGHT;
    else if (keys & KEY_UP && dir != DOWN)
        dir = UP;
    else if (keys & KEY_DOWN && dir != UP)
        dir = DOWN;
}

void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir) {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
    }
    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY) {
        score += 10;
        GenerateFruit();
        nTail++;
    }
}

void TitleScreen() {
    consoleClear();
    printf("\n\n\n");
    printf("  Bienvenue au Snake!\n\n");
    printf("  Appuyez sur A pour Jouer\n");
    printf("  Appuyez sur START pour Quitter\n");
    
    while (1) {
        scanKeys();
        u16 keys = keysDown();

        if (keys & KEY_A) {
            gameStarted = true;
            break;
        }
        if (keys & KEY_START) {
            gameStarted = false;
            break;
        }
        swiWaitForVBlank();
    }
}

void DisplayImage() {
    // Configure the graphics for the top screen
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    int bg = bgInit(3, BgType_Text4bpp, BgSize_T_256x256, 0, 0);

    // Afficher le texte "3DSNAKE" sur le fond
    bgSetPriority(bg, 3);
    bgSetScroll(bg, 0, 0);
    iprintf("\x1b[10;10H3DSNAKE"); // Position (10, 10)

    // Mettre à jour l'écran
    swiWaitForVBlank();
    bgUpdate();
}


int main() {
    // Initialize consoles for both screens
    consoleDemoInit();

    // Display the image on the top screen
    DisplayImage();

    // Handle title screen on the bottom screen
    gameStarted = false;
    TitleScreen();

    if (gameStarted) {
        Setup();
        int frameCounter = 0; // Frame counter to regulate game speed
        while (!gameOver) {
            Draw();
            Input();
            swiWaitForVBlank(); // Wait for VBlank to avoid graphical artifacts
            frameCounter++;
            if (frameCounter >= 5) { // Adjust this number to modify the speed
                frameCounter = 0;
                Logic(); // Execute additional logic after a certain number of frames
            }
        }
    }

    consoleClear();
    printf("\n\n\n");
    printf("  Merci d'avoir joué !\n");
    printf("  Appuyez sur START pour quitter\n");
    while (1) {
        scanKeys();
        u16 keys = keysDown();
        if (keys & KEY_START) {
            break;
        }
        swiWaitForVBlank();
    }

    return 0;
}
