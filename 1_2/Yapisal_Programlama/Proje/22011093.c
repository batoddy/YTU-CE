#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// video linki: https://youtu.be/z8O0gUkQv7w

#define SAVE_NUMBER 20
#define MAX_PARTS 50
typedef struct Board
{
    int **matrix;
    int N;
} Board; 

typedef struct Player
{
    char name[40];
    int A;
    int B;
    int C;
    int D;
    int E;
    int set;
    int point;
} Player;

typedef struct Move
{
    int skipx;
    int skipy;
    int getx;
    int gety;
    int tox;
    int toy;

} Move;

typedef struct
{
    int value;
    int label; // A, B, C, D, E için etiket
} Priority;

enum colors // 0 is empty
{
    A = 1,
    B,
    C,
    D,
    E
};

Board create_new_board(void);
void print_board(Board *board);
void print_board_selected(Board *board, int x, int y);
void make_a_move(Board *board, Player *player);

void againstComputer();
void againstPlayer();

void againstPlayer_newGame();
void againstPlayer_loadSave();

void add_to_Inventory(Player *player, int skipper);

void print_player(Player *player);
int check_game_over(Board *board);
int check_skipper_set(Player *player);
Player *check_winner(Player *player1, Player *player2);

void ai_move(Board *board, Player *computer);

FILE *create_file(char *);
int write_file(char *, char *);

void save_game(Board *board, Player *player1, Player *player2);
void read_save(Board *board, Player *player1, Player *player2);

int main()
{
    int menu_select, menu_flag = 1;
    // FILE *fp;

    srand(time(NULL));
    printf("================ Welcome to Skipitty Game ================");
    printf("\n.\n.\n.\n");

    do
    {
        printf("\n1-Singleplayer (Against Computer)\n");
        printf("2-Multiplayer (Against Player)\n");
        printf("0-Exit\n");
        printf("Selection: ");
        scanf("%d", &menu_select);

        switch (menu_select)
        {
        case 0:
            menu_flag = 0;
            break;
        case 1:
            againstComputer();
            break;
        case 2:
            againstPlayer();
            break;
        default:
            break;
        }
    } while (menu_flag == 1);

    return 0;
}

void againstComputer()
{
    printf("Against Computer (PvC)\n");

    int game_over_flag = 1;
    int i;
    char end_game_select;
    Board board;
    Player *player1, *computer, *winner;

    player1 = (Player *)calloc(1, sizeof(Player));
    computer = (Player *)calloc(1, sizeof(Player));

    printf("Enter player1's name:");
    scanf("%s", player1->name);
    // sprintf(player1->name, "PLAYER_1");
    sprintf(computer->name, "COMPUTER");

    board = create_new_board();
    printf("THE GAME BEGINS!!!\n");
    print_board(&board);

    for (i = 0; i < board.N + 1; i++)
        printf("==");
    printf("\n");

    while (game_over_flag == 1)
    {
        getchar();

        printf("[c]- Continue the game\n");
        printf("[p]- End the game\n");
        scanf("%c", &end_game_select);
        if (end_game_select == 'p')
            game_over_flag = 0;

        if (game_over_flag == 1)
        {
            ai_move(&board, computer);
            game_over_flag = check_game_over(&board); // if returns 0 game is over
        }

        if (game_over_flag == 1)
        {
            make_a_move(&board, player1);
            game_over_flag = check_game_over(&board); // if returns 0 game is over
        }
    }
    save_game(&board, player1, computer);
    printf("========== GAME IS OVER ==========\n");
    printf("========== GAME IS OVER ==========\n");
    printf("========== GAME IS OVER ==========\n");
    printf("========== GAME IS OVER ==========\n");
    printf("========== GAME IS OVER ==========\n\n");
    printf("======= PLAYER INVENTORIES =======\n");

    print_player(player1);
    print_player(computer);
    winner = check_winner(player1, computer);
}

void againstPlayer()
{
    int menu_select;

    printf("\n Multiplayer (PvP)\n");
    printf("1-New Game\n");
    printf("2-Load from Saves\n");
    printf("0-Exit\n");
    printf("Selection: ");
    scanf("%d", &menu_select);

    switch (menu_select)
    {
    case 1:
        againstPlayer_newGame();
        break;
    case 2:
        // againstPlayer_loadSave();
        break;
    default:
        break;
    }
}

void againstPlayer_newGame()
{
    int game_over_flag = 1;
    int i;
    char end_game_select;
    Board board;
    Player *player1, *player2, *winner;

    player1 = (Player *)calloc(1, sizeof(Player));
    player2 = (Player *)calloc(1, sizeof(Player));

    printf("Enter player1's name:");
    scanf("%s", player1->name);
    printf("Enter player2's name:");
    scanf("%s", player2->name);

    board = create_new_board();
    printf("THE GAME BEGINS!!!\n");
    print_board(&board);

    for (i = 0; i < board.N + 1; i++)
        printf("==");
    printf("\n");
    // check_game_over(&board);

    while (game_over_flag == 1)
    {
        getchar();

        printf("[c]- Continue the game\n");
        printf("[p]- End the game\n");
        scanf("%c", &end_game_select);
        if (end_game_select == 'p')
            game_over_flag = 0;

        if (game_over_flag == 1)
        {
            make_a_move(&board, player1);
            game_over_flag = check_game_over(&board); // if returns 0 game is over

            getchar();
            printf("[c]- Continue the game\n");
            printf("[p]- End the game\n");
            scanf("%c", &end_game_select);

            if (end_game_select == 'p')
                game_over_flag = 0;
        }

        if (game_over_flag == 1)
        {
            make_a_move(&board, player2);
            game_over_flag = check_game_over(&board); // if returns 0 game is over
        }
    }

    save_game(&board, player1, player2);
    printf("========== GAME IS OVER ==========\n");
    printf("========== GAME IS OVER ==========\n");
    printf("========== GAME IS OVER ==========\n");
    printf("========== GAME IS OVER ==========\n");
    printf("========== GAME IS OVER ==========\n\n");
    printf("======= PLAYER INVENTORIES =======\n");

    print_player(player1);
    print_player(player2);
    winner = check_winner(player1, player2);
}
/*
void againstPlayer_loadSave()
{
    Board board;
    Player *player1, *player2;
    read_save(&board, player1, player2);
}*/

Board create_new_board(void)
{
    int i, j;
    int dimention_flag;
    Board board;

    do
    {
        printf("Please enter the board dimention (8-20) (Only odd numbers): ");
        scanf("%d", &board.N);
        dimention_flag = 1;

        if (board.N < 8 || board.N > 20 || (board.N % 2) != 0)
        {
            dimention_flag = 0;
            printf("Please enter a dimention between 8-20 and an odd number!!!\n");
        }

    } while (dimention_flag == 0);

    board.matrix = (int **)calloc(board.N, sizeof(int *)); // allocate dynamic memory for board
    for (i = 0; i < board.N; i++)
        board.matrix[i] = (int *)calloc(board.N, sizeof(int));

    if (board.matrix != NULL)
        printf("Board %dx%d created succesfully/\n", board.N, board.N);
    else
        printf("Board can't created!!");

    for (i = 0; i < board.N; i++)
    {
        for (j = 0; j < board.N; j++)
            board.matrix[i][j] = (rand() % 5) + 1; // 5 renkten birini rastgele şekilde matrise atar
    }
    board.matrix[board.N / 2][board.N / 2] = 0;
    board.matrix[board.N / 2 - 1][board.N / 2] = 0;
    board.matrix[board.N / 2][board.N / 2 - 1] = 0;
    board.matrix[board.N / 2 - 1][board.N / 2 - 1] = 0;

    return board;
}

void make_a_move(Board *board, Player *player)
{
    int scoord_flag, jway_flag, move_flag, continue_move_flag;
    int skipperx, skippery, tmx, tmy;
    int skipgetx, skipgety;
    int selected_skipper, getted_skipper; // ingilizcenin içinden geçtim biliyorum :)
    char way;
    // skipperx/y : seçilen skipper
    // tmx/y: to move (hamle yapılacak korcinatlar)
    // skipgetx/y: alınacak skipper
    printf("\n%s's Turn\n", player->name);
    do
    {
        move_flag = 1;
        do
        {
            scoord_flag = 1;
            print_board(board);
            print_player(player);

            getchar();
            printf("Enter the coordinates for skipper(i,j):\n");
            scanf("%d", &skipperx);
            scanf("%d", &skippery);

            if ((skipperx < 0 || skipperx >= board->N || skippery < 0 || skippery >= board->N) && scoord_flag == 1)
            {
                printf("Enter coordinates between (0-%d)!\n", board->N - 1);
                scoord_flag = 0;
            }
            if (scoord_flag == 1) // bu if eğer kordinata yanlışlıkla harf girilirse matris için ayrılmayan bir belleğe ulaşılmaya çalışılmaması için eklenmiştir
            {
                if (board->matrix[skipperx][skippery] == 0)
                {
                    printf("This coordinate is empty, select another coordinate!\n");
                    scoord_flag = 0;
                }
                printf("Coordinates: (%d,%d)\n", skipperx, skippery);
            }
        } while (scoord_flag == 0);

        continue_move_flag = 0;
        do
        {
            print_board_selected(board, skipperx, skippery);

            if (continue_move_flag == 0)
                printf("Reselect coordinates -[x]\n");

            getchar();
            printf("End the movement     -[q]\n");
            printf("Select a way to move [w/a/s/d]: ");
            scanf("%c", &way);

            jway_flag = 1;
            move_flag = 1;
            switch (way)
            {
            case 'w':
                tmx = skipperx - 2;
                tmy = skippery;
                break;
            case 'a':
                tmx = skipperx;
                tmy = skippery - 2;
                break;
            case 'd':
                tmx = skipperx;
                tmy = skippery + 2;
                break;
            case 's':
                tmx = skipperx + 2;
                tmy = skippery;
                break;
            case 'x':
                if (continue_move_flag == 0)
                {
                    printf("Coordinate changed.\n");
                    move_flag = 0;
                    jway_flag = -1;
                    scoord_flag = 0;
                }
                else
                    printf("Coordinate can't chaged middle of a move.\n");

            case 'q':
                printf("Move finished.\n");
                move_flag = 1;
                jway_flag = -1;
                scoord_flag = 0;
                break;
            default:
                printf("Enter any of keys!!!\n");
                jway_flag = 0;
                break;
            }

            skipgetx = (skipperx + tmx) / 2;
            skipgety = (skippery + tmy) / 2;

            if ((jway_flag == 1) && (tmx < 0 || tmy < 0 || tmx >= board->N || tmy >= board->N))
            {
                printf("Out of board, select another way to jump!!|\n");
                jway_flag = 0;
            }
            if (jway_flag == 1 && board->matrix[tmx][tmy] != 0)
            {
                printf("Skippery can't move to %c way, select another way to jump!!\n", way);
                jway_flag = 0;
            }
            if (jway_flag == 1 && board->matrix[skipgetx][skipgety] == 0)
            {
                printf("There is no skip to get with this move!, select another way to jump!\n", way);
                jway_flag = 0;
            }

            // skipperi zıplat
            if (jway_flag == 1)
            {
                selected_skipper = board->matrix[skipperx][skippery];
                getted_skipper = board->matrix[skipgetx][skipgety];
                board->matrix[tmx][tmy] = selected_skipper;
                board->matrix[skipperx][skippery] = 0;
                board->matrix[skipgetx][skipgety] = 0;
                add_to_Inventory(player, getted_skipper);

                print_board_selected(board, tmx, tmy);
                print_player(player);
                skipperx = tmx;
                skippery = tmy;

                continue_move_flag = 1;
                jway_flag = 0;
            }
            // printf("jway_flag:%d\n", jway_flag);
        } while (jway_flag == 0);

        printf("New move\n");
    } while (move_flag == 0);
    printf("(%d,%d):%d ---(%d)--> (%d,%d)\n", skipperx, skippery, board->matrix[skipperx][skippery], board->matrix[skipgetx][skipgety], tmx, tmy);
}

void print_board(Board *board)
{
    int i, j;

    printf("  ");
    for (i = 0; i < board->N; i++)
    {
        printf("\033[4m%d\033[0m ", i % 10);
    }
    printf("\n");

    for (i = 0; i < board->N; i++)
    {
        printf("%d|", i % 10);
        for (j = 0; j < board->N; j++)
        {
            switch (board->matrix[i][j])
            {
            case 0:
                printf("\033[4m \033[0m");
                break;
            case A:
                printf("\033[4;31mA\033[0m");
                break;
            case B:
                printf("\033[4;34mB\033[0m");
                break;
            case C:
                printf("\033[4;33mC\033[0m");
                break;
            case D:
                printf("\033[4;32mD\033[0m");
                break;
            case E:
                printf("\033[4;35mE\033[0m");
                break;
            default:
                break;
            }
            printf("|");
        }

        printf("\n");
    }
    printf("\n");
}

void print_board_selected(Board *board, int x, int y)
{
    int i, j;

    printf("  ");
    for (i = 0; i < board->N; i++)
    {
        printf("\033[4m%d\033[0m ", i % 10);
    }
    printf("\n");

    for (i = 0; i < board->N; i++)
    {
        printf("%d|", i % 10);
        for (j = 0; j < board->N; j++)
        {
            if (i == x && j == y)
            {
                switch (board->matrix[i][j])
                {
                case A:
                    printf("\033[4mA\033[0m");
                    break;
                case B:
                    printf("\033[4mB\033[0m");
                    break;
                case C:
                    printf("\033[4mC\033[0m");
                    break;
                case D:
                    printf("\033[4mD\033[0m");
                    break;
                case E:
                    printf("\033[4mE\033[0m");
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (board->matrix[i][j])
                {
                case 0:
                    printf("\033[4m \033[0m");
                    break;
                case A:
                    printf("\033[4;31mA\033[0m");
                    break;
                case B:
                    printf("\033[4;34mB\033[0m");
                    break;
                case C:
                    printf("\033[4;33mC\033[0m");
                    break;
                case D:
                    printf("\033[4;32mD\033[0m");
                    break;
                case E:
                    printf("\033[4;35mE\033[0m");
                    break;
                default:
                    break;
                }
            }
            printf("|");
        }

        printf("\n");
    }
    printf("\n");
}
void add_to_Inventory(Player *player, int skipper)
{
    player->point += 100;
    switch (skipper)
    {
    case A:
        player->A++;
        break;
    case B:
        player->B++;
        break;
    case C:
        player->C++;
        break;
    case D:
        player->D++;
        break;
    case E:
        player->E++;
        break;

    default:
        break;
    }
}

void print_player(Player *player)
{

    printf("Player %s's Inventory:\n", player->name);
    printf("A: %d\n", player->A);
    printf("B: %d\n", player->B);
    printf("C: %d\n", player->C);
    printf("D: %d\n", player->D);
    printf("E: %d\n", player->E);
    printf("Points: %d\n", player->point);
}

int check_game_over(Board *board)
{
    int i, j;
    int w, a, s, d;
    int skipper_flag = 0;
    for (i = 0; i < board->N; i++)
    {
        for (j = 0; j < board->N; j++)
        {
            if (board->matrix[i][j] != 0)
            {
                w = j - 1;
                a = i - 1;
                s = i + 1;
                d = j + 1;
                // printf("i:%d j:%d | w:%d a:%d s:%d d:%d\n", i, j, w, a, s, d);
                if (w >= 0)
                {
                    if (board->matrix[i][w] != 0)
                        skipper_flag = 1; // printf("Skipper flag 1 | w\n");
                }
                if (a >= 0)
                {
                    if (board->matrix[a][j] != 0)
                        skipper_flag = 1; // printf("Skipper flag 1 | a\n");
                }
                if (s < board->N)
                {
                    if (board->matrix[s][j] != 0)
                        skipper_flag = 1; // printf("Skipper flag 1 | s\n");
                }
                if (d < board->N)
                {
                    if (board->matrix[i][d] != 0)
                        skipper_flag = 1; // printf("Skipper flag 1 | d\n");
                }
            }
        }
    }
    return skipper_flag;
}

int check_skipper_set(Player *player)
{
    int i;
    int skipper_arr[5];
    int min;
    skipper_arr[0] = player->A;
    skipper_arr[1] = player->B;
    skipper_arr[2] = player->C;
    skipper_arr[3] = player->D;
    skipper_arr[4] = player->E;

    min = skipper_arr[0];
    for (i = 0; i < 5; i++)
    {
        if (min > skipper_arr[i])
            min = skipper_arr[i];
    }
    player->set = min;
    return min;
}

Player *check_winner(Player *player1, Player *player2)
{
    Player *winner;

    winner = (Player *)calloc(1, sizeof(Player));

    check_skipper_set(player1);
    check_skipper_set(player2);

    if (player1->set > player2->set)
        winner = player1;
    else if (player1->set < player2->set)
        winner = player2;
    else if (player1->set == player2->set)
    {
        if (player1->point > player2->point)
            winner = player1;
        else if (player1->point < player2->point)
            winner = player2;
        else
            printf("There is no winner!!!\n");
    }
    printf("%s's sets:%d || %s's sets:%d\n", player1->name, player1->set, player2->name, player2->set);
    printf("\n\nCongrulations, the winner is '%s'\n\n", winner->name);
    print_player(winner);

    return winner;
}

int compare(const void *a, const void *b)
{
    return ((Priority *)a)->value - ((Priority *)b)->value;
}

void ai_move(Board *board, Player *computer)
{
    printf("\n%s's Turn\n", computer->name);

    // check for moves // for making sets
    Move *move_arr, *ava_move_arr;
    int i, j;
    int w, a, s, d, gw, ga, gs, gd;
    int move_ctr = 0, ava_move_ctr = 0;
    int min, want_skipper;
    int prio_idx = 0;
    int move, selected_skipper, getted_skipper;

    Priority prio_arr[5];

    move_arr = (Move *)calloc(100, sizeof(Move));
    ava_move_arr = (Move *)calloc(100, sizeof(Move));

    for (i = 0; i < board->N; i++)
    {
        for (j = 0; j < board->N; j++)
        {
            if (board->matrix[i][j] != 0)
            {
                w = j - 2;
                a = i - 2;
                s = i + 2;
                d = j + 2;

                gw = (w + j) / 2;
                ga = (a + i) / 2;
                gs = (s + i) / 2;
                gd = (d + j) / 2;

                if (w >= 0)
                {
                    if (board->matrix[i][w] == 0 && board->matrix[i][gw] != 0)
                    {

                        move_arr[move_ctr].skipx = i;
                        move_arr[move_ctr].skipy = j;

                        move_arr[move_ctr].getx = i;
                        move_arr[move_ctr].gety = gw;

                        move_arr[move_ctr].tox = i;
                        move_arr[move_ctr].toy = w;
                        // printf("w:(%d,%d):%d ---(%d)--> (%d,%d)\n", move_arr[move_ctr].skipx, move_arr[move_ctr].skipy, board->matrix[move_arr[move_ctr].skipx][move_arr[move_ctr].skipy], board->matrix[move_arr[move_ctr].getx][move_arr[move_ctr].gety], move_arr[move_ctr].tox, move_arr[move_ctr].toy);

                        move_ctr++;
                    }
                }
                if (a >= 0)
                {
                    if (board->matrix[a][j] == 0 && board->matrix[ga][j] != 0)
                    {

                        move_arr[move_ctr].skipx = i;
                        move_arr[move_ctr].skipy = j;

                        move_arr[move_ctr].getx = ga;
                        move_arr[move_ctr].gety = j;

                        move_arr[move_ctr].tox = a;
                        move_arr[move_ctr].toy = j;
                        // printf("a:(%d,%d):%d ---(%d)--> (%d,%d)\n", move_arr[move_ctr].skipx, move_arr[move_ctr].skipy, board->matrix[move_arr[move_ctr].skipx][move_arr[move_ctr].skipy], board->matrix[move_arr[move_ctr].getx][move_arr[move_ctr].gety], move_arr[move_ctr].tox, move_arr[move_ctr].toy);

                        move_ctr++;
                    }
                }
                if (s < board->N)
                {
                    if (board->matrix[s][j] == 0 && board->matrix[gs][j] != 0)
                    {

                        move_arr[move_ctr].skipx = i;
                        move_arr[move_ctr].skipy = j;

                        move_arr[move_ctr].getx = gs;
                        move_arr[move_ctr].gety = j;

                        move_arr[move_ctr].tox = s;
                        move_arr[move_ctr].toy = j;
                        // printf("s:(%d,%d):%d ---(%d)--> (%d,%d)\n", move_arr[move_ctr].skipx, move_arr[move_ctr].skipy, board->matrix[move_arr[move_ctr].skipx][move_arr[move_ctr].skipy], board->matrix[move_arr[move_ctr].getx][move_arr[move_ctr].gety], move_arr[move_ctr].tox, move_arr[move_ctr].toy);

                        move_ctr++;
                    }
                }
                if (d < board->N)
                {
                    if (board->matrix[i][d] == 0 && board->matrix[i][gd] != 0)
                    {
                        move_arr[move_ctr].skipx = i;
                        move_arr[move_ctr].skipy = j;

                        move_arr[move_ctr].getx = i;
                        move_arr[move_ctr].gety = gd;

                        move_arr[move_ctr].tox = i;
                        move_arr[move_ctr].toy = d;
                        // printf("d:(%d,%d):%d ---(%d)--> (%d,%d)\n", move_arr[move_ctr].skipx, move_arr[move_ctr].skipy, board->matrix[move_arr[move_ctr].skipx][move_arr[move_ctr].skipy], board->matrix[move_arr[move_ctr].getx][move_arr[move_ctr].gety], move_arr[move_ctr].tox, move_arr[move_ctr].toy);

                        move_ctr++;
                    }
                }
            }
        }
    }
    printf("Computer's move options\n");
    for (i = 0; i < move_ctr; i++)
    {
        printf("(%d,%d):%d ---(%d)--> (%d,%d)\n", move_arr[i].skipx, move_arr[i].skipy, board->matrix[move_arr[i].skipx][move_arr[i].skipy], board->matrix[move_arr[i].getx][move_arr[i].gety], move_arr[i].tox, move_arr[i].toy);
    }

    /*computer->A = 1;
    computer->B = 5;
    computer->C = 0;
    computer->D = 3;
    computer->E = 3;*/

    // look for minimum color
    prio_arr[0].value = computer->A;
    prio_arr[0].label = 1;
    prio_arr[1].value = computer->B;
    prio_arr[1].label = 2;
    prio_arr[2].value = computer->C;
    prio_arr[2].label = 3;
    prio_arr[3].value = computer->D;
    prio_arr[3].label = 4;
    prio_arr[4].value = computer->E;
    prio_arr[4].label = 5;

    qsort(prio_arr, 5, sizeof(Priority), compare);

    // Sıralanmış diziyi yazdırma
    printf("Priority:\n");
    for (i = 0; i < 5; i++)
        printf("%d: %d\n", prio_arr[i].label, prio_arr[i].value);

    //--------------------------------------------
    printf("Min:%d,want_skipper:%d\n", prio_arr[0].value, prio_arr[0].label);
    printf("Available moves:\n");

    do
    {
        want_skipper = prio_arr[prio_idx].label;

        for (i = 0; i < move_ctr; i++)
        {
            printf("checked skipper: %d | %d\n", board->matrix[move_arr[i].getx][move_arr[i].gety], prio_arr[prio_idx].label);
            if (board->matrix[move_arr[i].getx][move_arr[i].gety] == want_skipper)
            {
                ava_move_arr[ava_move_ctr].skipx = move_arr[i].skipx;
                ava_move_arr[ava_move_ctr].skipy = move_arr[i].skipy;

                ava_move_arr[ava_move_ctr].getx = move_arr[i].getx;
                ava_move_arr[ava_move_ctr].gety = move_arr[i].gety;

                ava_move_arr[ava_move_ctr].tox = move_arr[i].tox;
                ava_move_arr[ava_move_ctr].toy = move_arr[i].toy;

                printf("ava: (%d,%d):%d ---(%d)--> (%d,%d)\n", ava_move_arr[ava_move_ctr].skipx, ava_move_arr[ava_move_ctr].skipy, board->matrix[ava_move_arr[ava_move_ctr].skipx][ava_move_arr[ava_move_ctr].skipy], board->matrix[ava_move_arr[ava_move_ctr].getx][ava_move_arr[ava_move_ctr].gety], ava_move_arr[ava_move_ctr].tox, ava_move_arr[ava_move_ctr].toy);
                ava_move_ctr++;
            }
        }
        prio_idx++;
    } while (ava_move_ctr == 0);

    move = rand() % ava_move_ctr;

    print_board_selected(board, ava_move_arr[move].skipx, ava_move_arr[move].skipy);

    selected_skipper = board->matrix[ava_move_arr[move].skipx][ava_move_arr[move].skipy];
    getted_skipper = board->matrix[ava_move_arr[move].getx][ava_move_arr[move].gety];
    board->matrix[ava_move_arr[move].tox][ava_move_arr[move].toy] = selected_skipper;
    board->matrix[ava_move_arr[move].skipx][ava_move_arr[move].skipy] = 0;
    board->matrix[ava_move_arr[move].getx][ava_move_arr[move].gety] = 0;
    add_to_Inventory(computer, getted_skipper);

    print_board_selected(board, ava_move_arr[move].tox, ava_move_arr[move].toy);
    print_player(computer);
}

void save_game(Board *board, Player *player1, Player *player2)
{
    FILE *fp;
    int i, j;
    char file_name[30], input[30];
    char board_data[400];
    char board_d[10];
    char file_data[500] = "";

    printf("Save game\n");
    printf("Enter the name for the save pls:\n");
    getchar();
    scanf("%s", input);

    sprintf(file_name, "%s.txt", input);

    printf("%s\n", file_name);
    fp = fopen(file_name, "w+");
    // sprintf(data, "%d", board->N); // line 1
    // fputs(data, fp);

    for (i = 0; i < board->N; i++)
    {
        for (j = 0; j < board->N; j++)
        {
            sprintf(board_d, "%d", board->matrix[i][j]);
            strcat(board_data, board_d);
        }
    }
    strcat(board_data, "\0");
    printf("Board data: %s\n", board_data);

    //  printf("file data: %s\n");
    sprintf(file_data, "%d\n", board->N);
    fputs(file_data, fp);

    sprintf(file_data, "%s\n", board_data);
    fputs(file_data, fp);

    sprintf(file_data, "%s\n", player1->name);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player1->A);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player1->B);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player1->C);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player1->D);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player1->E);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player1->point);
    fputs(file_data, fp);

    sprintf(file_data, "%s\n", player2->name);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player2->A);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player2->B);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player2->C);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player2->D);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player2->E);
    fputs(file_data, fp);

    sprintf(file_data, "%d\n", player2->point);
    fputs(file_data, fp);
    fclose(fp);
}

void read_save(Board *board, Player *player1, Player *player2)
{
    FILE *fp;
    int i, j, x;
    char file_name[30], input[30];
    char board_data[400];
    char board_d[10];
    char file_data[500] = "";
    char buffer[10];

    printf("Load Game\n");
    printf("Please enter the name of the save file:");
    scanf("%s", input);
    sprintf(file_name, "%s.txt", input);
    printf("%s", file_name);
    fp = fopen(file_name, "r+");

    fscanf(fp, "%s", buffer);
    board->N = atoi(buffer);

    fscanf(fp, "%s", board_data);
    fscanf(fp, "%s", player1->name);
    fscanf(fp, "%s", buffer);
    player1->A = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player1->B = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player1->C = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player1->D = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player1->E = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player1->point = board->N = atoi(buffer);

    fscanf(fp, "%s", player2->name);
    fscanf(fp, "%s", buffer);
    player2->A = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player2->B = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player2->C = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player2->D = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player2->E = board->N = atoi(buffer);

    fscanf(fp, "%s", buffer);
    player2->point = board->N = atoi(buffer);
    x = 0;
    printf("BOARD:\n");
    for (i = 0; i < board->N; i++)
    {
        for (j = 0; j < board->N; j++)
        {
            board->matrix[i][j] = (int)atoi(board_data[x]);
            x++;
        }
    }
    print_board(board);
    print_player(player1);
    print_player(player2);
}
