#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

void add_to_Inventory(Player *player, int skipper);

void print_player(Player *player);
int check_game_over(Board *board);
int check_skipper_set(Player *player);
Player *check_winner(Player *player1, Player *player2);

int main()
{
    int menu_select, menu_flag = 1;
    // FILE *fp;

    srand(time(NULL));
    printf("================ Welcome to Skipitty Game ================");
    printf("\n.\n.\n.\n");

    do
    {
        printf("1-Singleplayer (Against Computer)\n");
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
    printf("Under developement");
}
void againstPlayer()
{
    int game_over_flag = 1;
    int i;
    char end_game_select;
    Board board;
    Player *player1, *player2, *winner;

    player1 = (Player *)calloc(1, sizeof(Player));
    player2 = (Player *)calloc(1, sizeof(Player));

    sprintf(player1->name, "PLAYER_1");
    sprintf(player2->name, "PLAYER_2");

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
/*
FILE *open_file(char *file_name)
{
    FILE fp;

    fp = fopen(file_name, 'w+');
    if (fptr == NULL)
    {
        printf("The file is not opened. The program will now exit.");
        exit(0);
    }
    fclose(fp);
}

int *write_file(char file_name, char *txt)
{
    fopen(file_name, "w+");
    fputs(txt, fp);
    fclose(fp);
}*/