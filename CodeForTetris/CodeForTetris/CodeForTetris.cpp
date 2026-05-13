#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
using namespace std;
#define H 20
#define W 15
char board[H][W] = {};
char blocks[][4][4] = {
       {{' ',' ',' ',' '}, // I
     {'I','I','I','I'},
     {' ',' ',' ',' '},
     {' ',' ',' ',' '}},

    {{' ',' ',' ',' '}, // O
     {' ','O','O',' '},
     {' ','O','O',' '},
     {' ',' ',' ',' '}},

    {{' ',' ',' ',' '}, // T
     {' ','T',' ',' '},
     {'T','T','T',' '},
     {' ',' ',' ',' '}},

    {{' ',' ',' ',' '}, // S
     {' ','S','S',' '},
     {'S','S',' ',' '},
     {' ',' ',' ',' '}},

    {{' ',' ',' ',' '}, // Z
     {'Z','Z',' ',' '},
     {' ','Z','Z',' '},
     {' ',' ',' ',' '}},

    {{' ',' ',' ',' '}, // J
     {'J',' ',' ',' '},
     {'J','J','J',' '},
     {' ',' ',' ',' '}},

    {{' ',' ',' ',' '}, // L
     {' ',' ','L',' '},
     {'L','L','L',' '},
     {' ',' ',' ',' '}}
};


int x = 4, y = 0, b = 1;
int speed = 200;
int linesCleared = 0;
void gotoxy(int x, int y) {
    COORD c = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void boardDelBlock() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (blocks[b][i][j] != ' ' && y + j < H)
                board[y + i][x + j] = ' ';
}
void block2Board() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (blocks[b][i][j] != ' ')
                board[y + i][x + j] = blocks[b][i][j];
}
void initBoard() {
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            if ((i == H - 1) || (j == 0) || (j == W - 1)) board[i][j] = '#';
            else board[i][j] = ' ';
}
void draw() {
    gotoxy(0, 0);
    for (int i = 0; i < H; i++, cout << endl)
        for (int j = 0; j < W; j++) {
            if (board[i][j] == '#') {
                // Có thể dùng ký tự ASCII 219 (tùy font console) hoặc in 2 dấu vạch
                cout << "##";
            }
            else if (board[i][j] == ' ') {
                cout << "  "; // 2 dấu cách cho không gian rỗng
            }
            else {
                // In ra hình vuông cho các khối gạch
                cout << "[]";
            }
        }
}
bool canMove(int dx, int dy) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (blocks[b][i][j] != ' ') {
                int tx = x + j + dx;
                int ty = y + i + dy;
                if (tx < 1 || tx >= W - 1 || ty >= H - 1) return false;
                if (board[ty][tx] != ' ') return false;
            }
    return true;
}
int removeLine() {
    int count = 0;
    for (int i = H - 2; i >= 1; i--) {
        bool full = true;
        for (int j = 1; j < W - 1; j++) {
            if (board[i][j] == ' ') { full = false; break; }
        }
        if (full) {
            for (int k = i; k >= 1; k--)
                for (int j = 1; j < W - 1; j++)
                    board[k][j] = board[k - 1][j];
            i++;
            count++;
        }
    }
    return count;
}
bool canRotate() {
    char temp[4][4];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[j][3 - i] = blocks[b][i][j];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (temp[i][j] != ' ') {
                int tx = x + j;
                int ty = y + i;

                if (tx < 1 || tx >= W - 1 || ty >= H - 1)
                    return false;

                if (board[ty][tx] != ' ')
                    return false;
            }
        }
    }

    return true;
}
void rotateBlock() {
    char temp[4][4];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[j][3 - i] = blocks[b][i][j];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            blocks[b][i][j] = temp[i][j];
}
int showMenu() {
    int selectedItem = 0; // 0 là Play, 1 là Exit

    while (true) {
        system("cls"); // Xóa màn hình

        cout << "====================================\n";
        cout << "             TETRIS GAME            \n";
        cout << "====================================\n\n";

        // Hiển thị nút Play
        if (selectedItem == 0) {
            cout << "          >>  PLAY  <<\n";
        }
        else {
            cout << "              PLAY    \n";
        }

        // Hiển thị nút Exit
        if (selectedItem == 1) {
            cout << "          >>  EXIT  <<\n";
        }
        else {
            cout << "              EXIT    \n";
        }

        cout << "\n------------------------------------\n";
        cout << " Su dung phim 'w' (len) va 's' (xuong)\n";
        cout << " Nhan 'Enter' de chon.\n";

        char c = _getch(); // Nhận phím từ người dùng

        if (c == 'w' || c == 'W') {
            selectedItem--;
            if (selectedItem < 0) selectedItem = 1; // Cuộn từ trên xuống dưới
        }
        else if (c == 's' || c == 'S') {
            selectedItem++;
            if (selectedItem > 1) selectedItem = 0; // Cuộn từ dưới lên trên
        }
        else if (c == '\r') { // Phím Enter trong console thường có mã '\r' (Carriage Return)
            return selectedItem;
        }
    }
}

int main()
{
    int menuChoice = showMenu();
    if (menuChoice == 1) {
        system("cls");
        cout << "Cam on ban da choi game. Hen gap lai!\n";
        return 0; // Thoát chương trình nếu chọn Exit
    }
    srand(time(0));
    b = rand() % 7;
    system("cls");
    initBoard();
    while (1) {
        boardDelBlock();
        if (_kbhit()) {
            char c = _getch();
            if (c == 'a' && canMove(-1, 0)) x--;
            if (c == 'd' && canMove(1, 0)) x++;
            if (c == 's' && canMove(0, 1))  y++;
            if (c == 'w' && canRotate())
                rotateBlock();
            if (c == 'q') break;
        }
        if (canMove(0, 1)) y++;
        else {
            block2Board();
            int removed = removeLine();
            linesCleared += removed;
            if (removed > 0) {
                speed = max(50, speed - removed * 10);
            }
            x = 5; y = 0; b = rand() % 7;
        }
        block2Board();
        draw();
        Sleep(speed);
    }
    return 0;
}
