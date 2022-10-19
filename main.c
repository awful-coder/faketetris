#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WINDOWS
#include <dos.h>
#include <conio.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#endif
#include <math.h>

#define PIECE_WIDTH 4
#define PIECE_HEIGHT 4
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

typedef struct Piece
{
    int y, x;

    int rotation;

    bool rotations[4][PIECE_HEIGHT][PIECE_WIDTH];
} Piece;

typedef struct Game
{
    unsigned int score;
    /* the active piece */
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

/* check if a piece overlaps with the board/is out of bounds */
bool pieceCollides(Piece piece, bool board[BOARD_HEIGHT][BOARD_WIDTH])
{
    int y, x;
    for (y = 0; y < PIECE_HEIGHT; y++)
        for (x = 0; x < PIECE_WIDTH; x++)
            if (piece.rotations[piece.rotation][y][x] && (piece.x + x < 0 ||
                                                          piece.x + x >= BOARD_WIDTH ||
                                                          piece.y + y >= BOARD_HEIGHT ||
                                                          piece.y + y >= 0 && board[piece.y + y][piece.x + x]))
                return true;
    return false;
}

int ghostPieceY(Piece piece, bool board[BOARD_HEIGHT][BOARD_WIDTH])
{
    do
        piece.y++;
    while (!pieceCollides(piece, board));

    return piece.y - 1;
}

void draw(Game game)
{
    int y, x, ghostY = ghostPieceY(game.piece, game.board);

    clear();

    printf("########################   score: %u\n", game.score);

    for (y = 0; y < BOARD_HEIGHT; y++)
    {
        fputs("##", stdout);
        for (x = 0; x < BOARD_WIDTH; x++)
        {
            if (game.board[y][x])
            {
                fputs("[]", stdout);
            }
            else if (x >= game.piece.x && x < game.piece.x + PIECE_WIDTH)
            {
                if (y >= game.piece.y && y < game.piece.y + PIECE_HEIGHT && game.piece.rotations[game.piece.rotation][y - game.piece.y][x - game.piece.x])
                {
                    fputs("()", stdout);
                }
                else if (y >= ghostY && y < ghostY + PIECE_HEIGHT && game.piece.rotations[game.piece.rotation][y - ghostY][x - game.piece.x])
                {
                    fputs("::", stdout);
                }
                else
                {
                    fputs(" .", stdout);
                }
            }
            else
            {
                fputs(" .", stdout);
            }
        }
        fputs("##", stdout);
        switch (y)
        {
        case 0:
            puts("   wasd or hjkl");
            break;
        case 1:
            puts("   <space> to hard drop");
            break;
        case 2:
            puts("   q to quit");
            break;
        default:
            puts("");
        }
    }

    puts("########################");
}

/* check if a line is made and if so clear and award points */
Game updateLines(Game game)
{
    int lines = 0;
    int y, x;

    /* check for lines */
    for (y = 0; y < BOARD_HEIGHT; y++)
    {
        bool isLine = true;
        for (x = 0; x < BOARD_WIDTH; x++)
            if (!game.board[y][x])
            {
                isLine = false;
                break;
            }

        if (isLine)
        {
            lines++;
            game.score += 10;

            /* shift lines above down */
            int copyY;
            for (copyY = y; copyY > 0; copyY--)
                for (x = 0; x < BOARD_WIDTH; x++)
                    game.board[copyY][x] = game.board[copyY - 1][x];

            for (x = 0; x < BOARD_WIDTH; x++)
                game.board[0][x] = false;
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

Piece newPiece()
{
    Piece piece;

    switch (rand() % 7)
    {
    case 0:
    {
        Piece _piece = {.rotations = {
                            {{}, {1, 1, 1, 1}},
                            {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},
                            {{}, {}, {1, 1, 1, 1}},
                            {{0, 1}, {0, 1}, {0, 1}, {0, 1}}}};
        piece = _piece;
        break;
    }
    case 1:
    {
        Piece _piece = {.rotations = {
                            {{1}, {1, 1, 1}},
                            {{0, 1, 1}, {0, 1}, {0, 1}},
                            {{}, {1, 1, 1}, {0, 0, 1}},
                            {{0, 1}, {0, 1}, {1, 1}}}};
        piece = _piece;
        break;
    }
    case 2:
    {
        Piece _piece = {.rotations = {
                            {{0, 0, 1}, {1, 1, 1}},
                            {{0, 1}, {0, 1}, {0, 1, 1}},
                            {{}, {1, 1, 1}, {1}},
                            {{1, 1}, {0, 1}, {0, 1}}}};
        piece = _piece;
        break;
    }
    case 3:
    {
        Piece _piece = {.rotations = {
                            {{1, 1}, {1, 1}},
                            {{1, 1}, {1, 1}},
                            {{1, 1}, {1, 1}},
                            {{1, 1}, {1, 1}},
                        }};
        piece = _piece;
        break;
    }
    case 4:
    {
        Piece _piece = {.rotations = {
                            {{0, 1, 1}, {1, 1}},
                            {{0, 1}, {0, 1, 1}, {0, 0, 1}},
                            {{}, {0, 1, 1}, {1, 1}},
                            {{1}, {1, 1}, {0, 1}},
                        }};
        piece = _piece;
        break;
    }
    case 5:
    {
        Piece _piece = {.rotations = {
                            {{0, 1}, {1, 1, 1}},
                            {{0, 1}, {0, 1, 1}, {0, 1}},
                            {{}, {1, 1, 1}, {0, 1}},
                            {{0, 1}, {1, 1}, {0, 1}},
                        }};
        piece = _piece;
        break;
    }
    case 6:
    {
        Piece _piece = {
            .rotations = {
                {{1, 1}, {0, 1, 1}},
                {{0, 0, 1}, {0, 1, 1}, {0, 1}},
                {{}, {1, 1}, {0, 1, 1}},
                {{0, 1}, {1, 1}, {1}}},
        };
        piece = _piece;
        break;
    }
    }

    piece.x = rand() % (BOARD_WIDTH - PIECE_WIDTH);
    piece.y = -4;
    piece.rotation = rand() % 4;

    return piece;
}

Game stopPiece(Game game)
{
    int x;
    int y;

    /* check for death */
    for (y = 0; y < PIECE_HEIGHT; y++)
        for (x = 0; x < PIECE_WIDTH; x++)
            if (game.piece.rotations[game.piece.rotation][y][x] && game.piece.y + y < 0)
            {
                game.alive = false;
                return game;
            }

    /* put the piece onto the board */
    for (y = 0; y < PIECE_HEIGHT; y++)
        for (x = 0; x < PIECE_WIDTH; x++)
            if (game.piece.rotations[game.piece.rotation][y][x])
                game.board[game.piece.y + y][game.piece.x + x] = true;

    game = updateLines(game);

    game.piece = newPiece();

    return game;
}

Game updatePiece(Game game)
{
    /* should the piece stop */
    game.piece.y++;
    bool should_stop = pieceCollides(game.piece, game.board);

    if (should_stop)
    {
        game.piece.y--;
        game = stopPiece(game);
    }

    return game;
}

Game update(Game game)
{
    game = updatePiece(game);

    draw(game);

    return game;
}

#ifndef _WINDOWS
/* kbhit for linux so we can poll, thanks to this guy https://cboard.cprogramming.com/c-programming/63166-kbhit-linux-post449301.html#post449301 */
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif

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
            else
                draw(game);
            break;
        case 'd':
        case 'l':
            game.piece.x++;
            if (pieceCollides(game.piece, game.board))
                game.piece.x--;
            else
                draw(game);
            break;
        case 's':
        case 'j':
            game = updatePiece(game);
            draw(game);
            break;
        case 'w':
        case 'k':
        {
            int lastRotation = game.piece.rotation;
            game.piece.rotation = (game.piece.rotation + 1) % 4;
            if (pieceCollides(game.piece, game.board) &&
                (game.piece.x += 1, pieceCollides(game.piece, game.board)) && /* wallkicks */
                (game.piece.x -= 2, pieceCollides(game.piece, game.board)) &&
                (game.piece.x += 1, true))
            {
                game.piece.rotation = lastRotation;
            }
            else
                draw(game);
            break;
        }
        case ' ':
            game.piece.y = ghostPieceY(game.piece, game.board);
            game = stopPiece(game);

            draw(game);
            break;
        }
    }

    return game;
}

int main()
{
    srand(time(0));

    Game game = {.alive = true, .piece = newPiece()};

    draw(game);

    time_t lastUpdate = time(0) - 1;

    while (game.alive)
    {
        game = updateInput(game);

        time_t now = time(0);

        if (lastUpdate - now <= -1)
        {
            lastUpdate = now;
            game = update(game);
        }
    }
}
