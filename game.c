#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)

// speed in pixels/second
#define SPEED (200)
#define FPS (60)


int collides(SDL_Rect rect1, SDL_Rect rect2) {
    // if (this.pt.x < boxOther.pt.x + boxOther.w && boxOther.pt.x < this.pt.x + this.w) {
        //     if (this.pt.y < boxOther.pt.y + boxOther.h && boxOther.pt.y < this.pt.y + this.h) {
        //         return true;
        //     }
        // }
    if (rect1.x < rect2.x + rect2.w && rect2.x < rect1.x + rect1.w) {
        if (rect1.y < rect2.y + rect2.h && rect2.y < rect1.y + rect1.h) {
            return 1;
        }
    }
    return 0;
}

int main(void) {
    srand(time(NULL));
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("Pac-ish", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!win) {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 1;
    }

    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend) {
        printf("error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }


    // PLAYER
    // load the image into memory using SDL_image library function
    SDL_Surface* surfPlayer = IMG_Load("resources/pacman.png");
    if (!surfPlayer) {
        printf("error creating surface\n");
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // load the image data into the graphics hardware's memory
    SDL_Texture* texPlayer = SDL_CreateTextureFromSurface(rend, surfPlayer);
    SDL_FreeSurface(surfPlayer);
    if (!texPlayer) {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // struct to hold the position and size of the sprite
    SDL_Rect rectPlayer;
    // get and scale the dimensions of texture
    SDL_QueryTexture(texPlayer, NULL, NULL, &rectPlayer.w, &rectPlayer.h);
    rectPlayer.w = 30;
    rectPlayer.h = 30;
    float posPlayer[2] = {(WINDOW_WIDTH - rectPlayer.w) / 2, (WINDOW_HEIGHT - rectPlayer.h) / 2}; // {x, y}


    // GHOSTS
    //load the image into memory using SDL_image library function
    SDL_Surface* surfGhost = IMG_Load("resources/ghost.png");
    if (!surfGhost) {
        printf("error creating surface\n");
        SDL_DestroyTexture(texPlayer);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // load the image data into the graphics hardware's memory
    SDL_Texture* texGhost = SDL_CreateTextureFromSurface(rend, surfGhost);
    SDL_FreeSurface(surfGhost);
    if (!texGhost) {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(texPlayer);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    float posGhostsArr[3][2];
    SDL_Rect rectGhosts[3];
    for (int i = 0; i < 3; i++) {    
        // struct to hold the position and size of the sprite
        SDL_Rect rectGhost;
        // get and scale the dimensions of texture
        SDL_QueryTexture(texGhost, NULL, NULL, &rectGhost.w, &rectGhost.h);
        rectGhost.w = 30;
        rectGhost.h = 30;
        posGhostsArr[i][0] = rand() % (WINDOW_WIDTH - rectGhost.w); // x
        posGhostsArr[i][1] = rand() % (WINDOW_HEIGHT - rectGhost.h); // y
        rectGhost.x = (int) posGhostsArr[i][0];
        rectGhost.y = (int) posGhostsArr[i][1];
        rectGhosts[i] = rectGhost;
    }


    SDL_Surface* surfCoin = IMG_Load("resources/coin.png");
    if (!surfCoin) {
        printf("error creating surface\n");
        SDL_DestroyTexture(texPlayer);
        SDL_DestroyTexture(texGhost);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // load the image data into the graphics hardware's memory
    SDL_Texture* texCoin = SDL_CreateTextureFromSurface(rend, surfCoin);
    SDL_FreeSurface(surfCoin);
    if (!texPlayer) {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(texPlayer);
        SDL_DestroyTexture(texGhost);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // struct to hold the position and size of the sprite
    SDL_Rect rectCoin;
    // get and scale the dimensions of texture
    SDL_QueryTexture(texCoin, NULL, NULL, &rectCoin.w, &rectCoin.h);
    rectCoin.w = 30;
    rectCoin.h = 30;
    float posCoin[2] = {rand() % (WINDOW_WIDTH - rectCoin.w), rand() % (WINDOW_HEIGHT - rectCoin.h)};


    // LIGHTING
    int lightPower = 100;
    // load the image into memory using SDL_image library function
    SDL_Surface* surfBlack = IMG_Load("resources/black.png");
    if (!surfBlack) {
        printf("error creating surface\n");
        SDL_DestroyTexture(texPlayer);
        SDL_DestroyTexture(texGhost);
        SDL_DestroyTexture(texCoin);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // load the image data into the graphics hardware's memory
    SDL_Texture* texBlack = SDL_CreateTextureFromSurface(rend, surfBlack);
    SDL_FreeSurface(surfBlack);
    if (!texBlack) {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(texPlayer);
        SDL_DestroyTexture(texGhost);
        SDL_DestroyTexture(texCoin);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // struct to hold the position and size of the sprite
    SDL_Rect rectBlack;
    // get and scale the dimensions of texture
    SDL_QueryTexture(texBlack, NULL, NULL, &rectBlack.w, &rectBlack.h);
    rectBlack.w = 2;
    rectBlack.h = 2;


    SDL_Surface* surfLightCircle = IMG_Load("resources/lightCircle.jpeg");
    if (!surfLightCircle) {
        printf("error creating surface\n");
        SDL_DestroyTexture(texPlayer);
        SDL_DestroyTexture(texGhost);
        SDL_DestroyTexture(texCoin);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // load the image data into the graphics hardware's memory
    SDL_Texture* texLightCircle = SDL_CreateTextureFromSurface(rend, surfLightCircle);
    SDL_FreeSurface(surfLightCircle);
    if (!texLightCircle) {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(texPlayer);
        SDL_DestroyTexture(texGhost);
        SDL_DestroyTexture(texCoin);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    // struct to hold the position and size of the sprite
    SDL_Rect rectLightCircle;
    // get and scale the dimensions of texture
    SDL_QueryTexture(texLightCircle, NULL, NULL, &rectLightCircle.w, &rectLightCircle.h);
    rectLightCircle.w = lightPower * 2;
    rectLightCircle.h = lightPower * 2;
    // rectLightCircle.x = rectPlayer.x;
    // rectLightCircle.y = rectPlayer.y;

    int score = 0;
    printf("Score: %d\n", score);
    // player direction
    int dir = 0; // 1, 2, 3, 4 :: up, down, left, right

    // set to 1 when window close button is pressed
    int closeRequested = 0;
    
    // animation loop
    while (!closeRequested) {
        clock_t start = clock();

        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    closeRequested = 1;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            dir = 1;
                            break;
                        case SDL_SCANCODE_S:
                            dir = 2;
                            break;
                        case SDL_SCANCODE_A:
                            dir = 3;
                            break;
                        case SDL_SCANCODE_D:
                            dir = 4;
                            break;
                    }
                    break;
            }
        }

        // move the player
        switch (dir) {
            case 1:
                posPlayer[1] -= SPEED / FPS;
                break;
            case 2:
                posPlayer[1] += SPEED / FPS;
                break;
            case 3:
                posPlayer[0] -= SPEED / FPS;
                break;
            case 4:
                posPlayer[0] += SPEED / FPS;
                break;
        }

        // collision detection with bounds
        if (posPlayer[0] <= 0) posPlayer[0] = 0;
        if (posPlayer[1] <= 0) posPlayer[1] = 0;
        if (posPlayer[0] >= WINDOW_WIDTH - rectPlayer.w) posPlayer[0] = WINDOW_WIDTH - rectPlayer.w;
        if (posPlayer[1] >= WINDOW_HEIGHT - rectPlayer.h) posPlayer[1] = WINDOW_HEIGHT - rectPlayer.h;

        // move ghost
        for (int i = 0; i < 3; i++) {
            int difX = posPlayer[0] - posGhostsArr[i][0];
            int difY = posPlayer[1] - posGhostsArr[i][1];
            if (abs(difX) > abs(difY)) {
                if (difX > 0) { // on right
                    posGhostsArr[i][0] += SPEED/2 * 1/FPS;
                }
                else {
                    posGhostsArr[i][0] -= SPEED/2 * 1/FPS;
                }
            }
            else {
                if (difY > 0) { // on the down
                    posGhostsArr[i][1] += SPEED/2 * 1/FPS;
                }
                else {
                    posGhostsArr[i][1] -= SPEED/2 * 1/FPS;
                }
            }
            rectGhosts[i].x = (int) posGhostsArr[i][0];
            rectGhosts[i].y = (int) posGhostsArr[i][1];
        }

        // set the player positions in the struct
        
        rectPlayer.x = (int) posPlayer[0];
        rectPlayer.y = (int) posPlayer[1];

        if (collides(rectPlayer, rectCoin)) {
            posCoin[0] = rand() % (WINDOW_WIDTH - rectCoin.w);
            posCoin[1] = rand() % (WINDOW_HEIGHT - rectCoin.h);
            score += 1;
            printf("Score: %d\n", score);
        }
        for (int i = 0; i < 3; i++) {
            if (collides(rectPlayer, rectGhosts[i])) {
                closeRequested = 1;
            }
        }

        rectCoin.x = (int) posCoin[0];
        rectCoin.y = (int) posCoin[1];

        float center[2] = {rectPlayer.x + rectPlayer.w/2, rectPlayer.y + rectPlayer.h/2};
        rectLightCircle.x = center[0] - rectLightCircle.w/2;
        rectLightCircle.y = center[1] - rectLightCircle.h/2;

        // DRAW THE FRAME
        // clear the window
        SDL_RenderClear(rend);
        SDL_RenderCopy(rend, texLightCircle, NULL, &rectLightCircle);
        for (int i = 0; i < 3; i++) {
            SDL_RenderCopy(rend, texGhost, NULL, &rectGhosts[i]);
        }
        SDL_RenderCopy(rend, texCoin, NULL, &rectCoin);

        // LIGHTING
        int minLight = 6;
        float topLeft[2] = {center[0] - lightPower - 4, center[1] - lightPower - 4};
        float bottomRight[2] = {center[0] + lightPower + 4, center[1] + lightPower + 4};
        for (int x = topLeft[0]; x < bottomRight[0]; x += 2) {
            for (int y = topLeft[1]; y < bottomRight[1]; y += 2) {
                float distFromCenter = sqrt((center[0] - x) * (center[0] - x) + (center[1] - y) * (center[1] - y));
                // if (distFromCenter < lightPower + 4) {
                    float tempLightPower = distFromCenter - lightPower;
                    tempLightPower = tempLightPower * rand()/RAND_MAX;
                    // printf("%f\n", tempLightPower);

                    if (tempLightPower > -minLight) {
                        rectBlack.x = x;
                        rectBlack.y = y;
                        SDL_RenderCopy(rend, texBlack, NULL, &rectBlack);
                    }
                // }
            }
        }

        // draw the images to the window
        SDL_RenderCopy(rend, texPlayer, NULL, &rectPlayer);

        SDL_Rect rectBar;
        //left
        rectBar.x = 0;
        rectBar.y = 0;
        rectBar.w = rectLightCircle.x;
        rectBar.h = WINDOW_HEIGHT;
        SDL_RenderCopy(rend, texBlack, NULL, &rectBar);

        //right
        rectBar.x = rectLightCircle.x + rectLightCircle.w;
        rectBar.y = 0;
        rectBar.w = WINDOW_WIDTH - rectBar.x;
        rectBar.h = WINDOW_HEIGHT;
        SDL_RenderCopy(rend, texBlack, NULL, &rectBar);

        //top
        rectBar.x = rectLightCircle.x;
        rectBar.y = 0;
        rectBar.w = rectLightCircle.w;
        rectBar.h = rectLightCircle.y;
        SDL_RenderCopy(rend, texBlack, NULL, &rectBar);

        //bottom
        rectBar.x = rectLightCircle.x;
        rectBar.y = rectLightCircle.y + rectLightCircle.h;
        rectBar.w = rectLightCircle.w;
        rectBar.h = WINDOW_HEIGHT - rectBar.y;
        SDL_RenderCopy(rend, texBlack, NULL, &rectBar);


        SDL_RenderPresent(rend);

        // wait 1/FPSth of a second
        clock_t end = clock();
        double frameTime = (double) (end - start) / CLOCKS_PER_SEC;
        frameTime *= 1000; // seconds to milliseconds
        double waitTime = (1000 - frameTime > 0) ? 1000 - frameTime : 0;
        SDL_Delay(waitTime/FPS);
    }
    printf("\n\nYOU DIED! Score: %d\n", score);
    SDL_Delay(2000);
    
    // clean up resources before exiting
    SDL_DestroyTexture(texPlayer);
    SDL_DestroyTexture(texGhost);
    SDL_DestroyTexture(texCoin);
    SDL_DestroyTexture(texLightCircle);
    // SDL_DestroyTexture(texLight);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
