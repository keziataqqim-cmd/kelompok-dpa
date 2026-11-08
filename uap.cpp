#include <ncurses.h>
#include <cstdlib>
#include <ctime>

const int SIZE = 3;
char board[SIZE][SIZE];

enum Mode { TWO_PLAYER, VS_COMPUTER };

bool isEnterKey(int ch) {
    if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) return true;
#ifdef PADENTER
    if (ch == PADENTER) return true;
#endif
    return false;
}


void resetBoard() {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            board[r][c] = ' ';
}

char checkWinner() {
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0];
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return board[0][i];
    }
    if (board[1][1] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return board[1][1];
    if (board[1][1] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return board[1][1];
    return ' ';
}

bool isFull() {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (board[r][c] == ' ')
                return false;
    return true;
}

bool findWinningMove(char player, int &outR, int &outC) {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (board[r][c] == ' ') {
                board[r][c] = player;
                bool win = (checkWinner() == player);
                board[r][c] = ' ';
                if (win) {
                    outR = r;
                    outC = c;
                    return true;
                }
            }
        }
    }
    return false;
}

void computerMove(char me, char opponent) {
    int r, c;
    if (findWinningMove(me, r, c) || findWinningMove(opponent, r, c)) {
        board[r][c] = me;
        return;
    }
    if (board[1][1] == ' ') {
        board[1][1] = me;
        return;
    }
    int empty[SIZE * SIZE][2];
    int n = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == ' ') {
                empty[n][0] = i;
                empty[n][1] = j;
                n++;
            }
        }
    }
    if (n > 0) {
        int pick = rand() % n;
        board[empty[pick][0]][empty[pick][1]] = me;
    }
}

void drawBoard(int curR, int curC) {
    int top = 5, left = 10;
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            int y = top + r * 2;
            int x = left + c * 4;
            char cell = board[r][c];

            if (r == curR && c == curC) attron(A_REVERSE);
            if (cell == 'X') attron(COLOR_PAIR(1) | A_BOLD);
            if (cell == 'O') attron(COLOR_PAIR(2) | A_BOLD);

            mvprintw(y, x, " %c ", cell);

            attroff(A_REVERSE);
            attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | A_BOLD);

            if (c < SIZE - 1) mvaddch(y, x + 3, '|');
        }
        if (r < SIZE - 1) mvhline(top + r * 2 + 1, left, '-', 11);
    }
}

void showHelp() {
    erase();
    mvprintw(1, 2, "=== CARA MAIN ===");
    mvprintw(3, 2, "Pemain X dan O bergantian mengisi kotak.");
    mvprintw(4, 2, "Yang pertama membuat 3 tanda sejajar menang");
    mvprintw(5, 2, "(horizontal, vertikal, atau diagonal).");
    mvprintw(7, 2, "Panah       : gerakkan kursor");
    mvprintw(8, 2, "Enter/Spasi : isi kotak");
    mvprintw(9, 2, "r           : ronde baru (skor tetap)");
    mvprintw(10, 2, "m           : kembali ke menu");
    mvprintw(11, 2, "q           : keluar");
    mvprintw(13, 2, "Tekan tombol apa saja untuk kembali...");
    refresh();
    getch();
}

int showMenu() {
    const char *items[] = {"2 Pemain", "Lawan Komputer", "Cara Main", "Keluar"};
    const int N = 4;
    int sel = 0;

    while (true) {
        erase();
        mvprintw(1, 4, "=== TIC TAC TOE ===");
        for (int i = 0; i < N; i++) {
            if (i == sel) attron(A_REVERSE);
            mvprintw(3 + i, 4, " %s ", items[i]);
            attroff(A_REVERSE);
        }
        mvprintw(9, 4, "Panah: pilih | Enter: OK");
        refresh();

        int ch = getch();
        if (ch == KEY_UP && sel > 0) sel--;
        else if (ch == KEY_DOWN && sel < N - 1) sel++;
        else if (isEnterKey(ch) || ch == ' ') return sel;
    }
}

bool playGame(Mode mode) {
    int scoreX = 0, scoreO = 0, draws = 0;
    int curR = 1, curC = 1;
    char turn = 'X';
    bool gameOver = false;
    char winner = ' ';

    resetBoard();

    while (true) {
        erase();
        mvprintw(0, 0, "=== TIC TAC TOE === (%s)",
                 mode == TWO_PLAYER ? "2 Pemain" : "Lawan Komputer");
        mvprintw(1, 0, "Panah:gerak Enter:pilih r:ronde baru m:menu q:keluar");
        mvprintw(2, 0, "Skor  X: %d   O: %d   Seri: %d", scoreX, scoreO, draws);
        drawBoard(curR, curC);

        if (!gameOver) {
            if (mode == VS_COMPUTER)
                mvprintw(12, 0, "Giliran kamu (X)");
            else
                mvprintw(12, 0, "Giliran pemain: %c", turn);
        } else if (winner != ' ') {
            mvprintw(12, 0, "Pemain %c menang! Tekan r untuk ronde baru.", winner);
        } else {
            mvprintw(12, 0, "Seri! Tekan r untuk ronde baru.");
        }
        refresh();

        int ch = getch();

        if (ch == 'q') return false;
        if (ch == 'm') return true;
        if (ch == 'r') {
            resetBoard();
            turn = 'X';
            gameOver = false;
            winner = ' ';
            continue;
        }
        if (gameOver) continue;

        bool moved = false;
        switch (ch) {
            case KEY_UP:    if (curR > 0) curR--; break;
            case KEY_DOWN:  if (curR < SIZE - 1) curR++; break;
            case KEY_LEFT:  if (curC > 0) curC--; break;
            case KEY_RIGHT: if (curC < SIZE - 1) curC++; break;
            default:
                if ((isEnterKey(ch) || ch == ' ') && board[curR][curC] == ' ') {
                    board[curR][curC] = turn;
                    moved = true;
                }
                break;
        }

        if (moved) {
            winner = checkWinner();
            if (winner != ' ' || isFull()) {
                gameOver = true;
            } else {
                turn = (turn == 'X') ? 'O' : 'X';

                // Giliran komputer (selalu jadi O)
                if (mode == VS_COMPUTER && turn == 'O') {
                    computerMove('O', 'X');
                    winner = checkWinner();
                    if (winner != ' ' || isFull())
                        gameOver = true;
                    else
                        turn = 'X';
                }
            }

            if (gameOver) {
                if (winner == 'X') scoreX++;
                else if (winner == 'O') scoreO++;
                else draws++;
            }
        }
    }
}

int main() {
    srand((unsigned)time(nullptr));

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);   
        init_pair(2, COLOR_CYAN, COLOR_BLACK);  
    }

    bool running = true;
    while (running) {
        int choice = showMenu();
        if (choice == 0) running = playGame(TWO_PLAYER);
        else if (choice == 1) running = playGame(VS_COMPUTER);
        else if (choice == 2) showHelp();
        else running = false;
    }

    endwin();
    return 0;
}
