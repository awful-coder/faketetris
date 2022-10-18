/*
Clone of Tetris in C89.
Copyright (c) 2022  Kyle Zhou. Mail improvements to <kyle.zhou26@ycdsbk12.ca> for the retro feel.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

kevin please patch this for windows i did my best
yall should try coding while listening to ren ren ai ai circu circu lation lation its surprisingly effective
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
// if you dont use unix-like or windows you are a clown
#ifdef _WINDOWS
// actually if you use windows you are also a clown
#include <dos.h>
#include <conio.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#endif
#include <math.h>

#define PIECE_WIDTH 4
#define PIECE_HEIGHT 4
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

typedef struct Piece
{
    int y, x;

    unsigned int rotation;

    bool rotations[4][PIECE_HEIGHT][PIECE_WIDTH];
} Piece;

typedef struct Game
{
    unsigned int score;
    // the active piece
    Piece piece;

    bool board[BOARD_HEIGHT][BOARD_WIDTH];

    bool alive;
} Game;

void clear()
{
#ifdef _WINDOWS
    clrscr();
#else
    printf("\033c");
#endif
}

void draw(Game game)
{
    unsigned int y, x;

    clear();

    printf("########################   score: %u   wasd or hjkl\n", game.score);

    for (y = 0; y < BOARD_HEIGHT; y++)
    {
        fputs("##", stdout);
        for (x = 0; x < BOARD_WIDTH; x++)
        {
            fputs(game.board[y][x] ||
                          y >= game.piece.y && y < game.piece.y + PIECE_HEIGHT &&
                              x >= game.piece.x && x < game.piece.x + PIECE_WIDTH &&
                              game.piece.rotations[game.piece.rotation][y - game.piece.y][x - game.piece.x]
                      ? "[]"
                      : " .",
                  stdout);
        }
        puts("##");
    }

    puts("########################");
}

// check if a line is made and if so clear and award points
Game updateLines(Game game)
{
    unsigned int lines = 0;
    unsigned int y, x;

    for (y = 0; y < BOARD_HEIGHT; y++)
    {
        bool isLine = true;
        for (x = 0; x < BOARD_HEIGHT; x++)
        {
            if (!game.board[y][x])
            {
                isLine = false;
                break;
            }
        }

        if (isLine)
        {
            lines++;
            game.score += 10;

            // clear line

            bool newBoard[BOARD_WIDTH][BOARD_HEIGHT];
            int offset = -1;
            unsigned int newY;
            for (newY = 1; newY < BOARD_HEIGHT; newY++)
            {
                if (newY == y)
                {
                    offset++;
                }

                for (x = 0; x < BOARD_WIDTH; x++)
                    newBoard[y][x] = game.board[y + offset][x];
            }

            Game newGame = {.score = game.score, .piece = game.piece, .board = newBoard};

            game = newGame;
        }
    }

    switch (lines)
    {
    case 2:
        game.score += 2;
        break;
    case 3:
        game.score += 5;
        break;
    case 4:
        game.score += 10;
        break;
    }

    return game;
}

// check if a piece overlaps with the board
bool pieceCollides(Piece piece, bool board[BOARD_HEIGHT][BOARD_WIDTH])
{
    unsigned int y, x;
    for (y = 0; y < PIECE_HEIGHT; y++)
    {
        for (x = 0; x < PIECE_WIDTH; x++)
        {
            if (piece.rotations[piece.rotation][y][x] && piece.y + y >= BOARD_HEIGHT && (piece.x + x < 0 || piece.x + x >= BOARD_WIDTH || piece.y + y < 0 || board[piece.y + y][piece.x + x]))
                return true;
        }
    }

    return false;
}

Piece newPiece()
{
    Piece piece;

    switch (rand() % 7)
    {
    case 0:
    {
        Piece _piece = {.rotations = {{{}, {1, 1, 1, 1}}, {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}, {{}, {}, {1, 1, 1, 1}}, {{0, 1}, {0, 1}, {0, 1}, {0, 1}}}};
        piece = _piece;
        break;
    }
    case 1:
    {
        Piece _piece = {.rotations = {
                            {{1}, {1, 1, 1}},
                        }};
        piece = _piece;
        break;
    }
    }

    piece.x = rand() % (BOARD_WIDTH - PIECE_WIDTH);
    piece.y = -4;

    return piece;
}

Game updatePiece(Game game)
{
    // should the piece stop
    game.piece.y++;
    bool should_stop = pieceCollides(game.piece, game.board);

    if (should_stop)
    {
        if (game.piece.y < 0)
        {
            game.alive = false;
        }
        else
        {
            unsigned int x;
            unsigned int y;

            game.piece.y--;
            // put the piece onto the board
            for (y = 0; y < PIECE_HEIGHT; y++)
            {
                for (x = 0; x < PIECE_WIDTH; x++)
                {
                    if (game.piece.rotations[game.piece.rotation][y][x])
                        game.board[game.piece.y + y][game.piece.x + x] = true;
                }
            }

            game = updateLines(game);
        }
    }

    return game;
}

Game update(Game game)
{
    game = updatePiece(game);

    return game;
}

int kbhit() {
    struct timeval timeout;
    fd_set rdset;

    FD_ZERO(&rdset);
    FD_SET(0, &rdset);
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;

    return select(1, &rdset, NULL, NULL, &timeout);  
}

Game updateInput(Game game)
{
    while (kbhit())
    {
        char c = getchar();

        switch (c)
        {
        case 'q':
            game.alive = false;
            break;
        case 'a':
        case 'h':
            game.piece.x--;
            if (pieceCollides(game.piece, game.board))
                game.piece.x++;
            break;
        case 'd':
        case 'l':
            game.piece.x++;
            if (pieceCollides(game.piece, game.board))
                game.piece.x--;
            break;
        }
    }

    return game;
}

int main()
{
    srand(time(0));

    Game game = {.alive = true};

    while (game.alive)
    {
        game = updateInput(game);

        game = update(game);

        draw(game);

        if (!game.alive)
            break;

        sleep(1);
    }
}
