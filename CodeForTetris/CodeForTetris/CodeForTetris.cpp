#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
using namespace std;
#define H 20
#define W 15
#define OFFSET_X 30 // Độ dời của bảng chơi sang bên phải để nhường chỗ cho panel bên trái

class Tetris {
private:
    char board[H][W] = {};
    char blocks[7][4][4];
    int x, y, b;
    int speed;
    int linesCleared;
    int validBlocks[8];
    bool isPaused;
    bool isRunning;
    int score;
    time_t startTime;
    int elapsedSeconds;
    void gotoxy(int x, int y) {
        COORD c = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    }
    int calculateScore(int lines) {
        int pts[] = { 0, 100, 300, 500, 800 };
        if (lines >= 1 && lines <= 4) return pts[lines];
        return 0;
    }

public:

    Tetris() {
        isPaused = false;
        isRunning = true;
        initGameVariables();

        // Khởi tạo dữ liệu các khối gạch
        char tempBlocks[7][4][4] = {
            {{' ',' ',' ',' '}, {'I','I','I','I'}, {' ',' ',' ',' '}, {' ',' ',' ',' '}}, // I
            {{' ',' ',' ',' '}, {' ','O','O',' '}, {' ','O','O',' '}, {' ',' ',' ',' '}}, // O
            {{' ',' ',' ',' '}, {' ','T',' ',' '}, {'T','T','T',' '}, {' ',' ',' ',' '}}, // T
            {{' ',' ',' ',' '}, {' ','S','S',' '}, {'S','S',' ',' '}, {' ',' ',' ',' '}}, // S
            {{' ',' ',' ',' '}, {'Z','Z',' ',' '}, {' ','Z','Z',' '}, {' ',' ',' ',' '}}, // Z
            {{' ',' ',' ',' '}, {'J',' ',' ',' '}, {'J','J','J',' '}, {' ',' ',' ',' '}}, // J
            {{' ',' ',' ',' '}, {' ',' ','L',' '}, {'L','L','L',' '}, {' ',' ',' ',' '}}  // L
        };
        memcpy(blocks, tempBlocks, sizeof(blocks));
    }

    void initGameVariables() {
        x = 4; y = 0; b = 1;
        speed = 200;
        score = 0;
        linesCleared = 0;
        startTime = time(0);
        elapsedSeconds = 0;
        int initialValid[] = { 0, 2, 9, 11, 12, 13, 14, 15 };
        for (int i = 0; i < 8; i++) validBlocks[i] = initialValid[i];
    }

    void boardDelBlock() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (blocks[b][i][j] != ' ' && y + i < H)
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

    /*VE BAN BEN TRAI MAN HINH GAMEPLAY*/

    void drawSidePanel() {
        gotoxy(2, 2);  cout << "==========================";
        gotoxy(2, 3);  cout << "      TETRIS CONTROL      ";
        gotoxy(2, 4);  cout << "==========================";

        gotoxy(2, 6);  cout << " [W] : Rotate (Xoay)";
        gotoxy(2, 7);  cout << " [A] : Left   (Trai)";
        gotoxy(2, 8);  cout << " [D] : Right  (Phai)";
        gotoxy(2, 9);  cout << " [S] : Down   (Xuong)";

        gotoxy(2, 11); cout << "--------------------------";
        gotoxy(2, 12); cout << " [P] : PAUSE  (Tam dung)";
        gotoxy(2, 13); cout << " [R] : RESET  (Choi lai)";
        gotoxy(2, 14); cout << " [Q] : QUIT   (Thoat)";
        gotoxy(2, 15); cout << "--------------------------";

        elapsedSeconds = (int)difftime(time(0), startTime);
        int m = elapsedSeconds / 60;
        int s = elapsedSeconds % 60;

        gotoxy(2, 17); cout << " Score : " << score << "          ";
        gotoxy(2, 18); cout << " Lines : " << linesCleared << "          ";
        gotoxy(2, 19); cout << " Time  : " << m << "m " << s << "s     ";
        gotoxy(2, 20); cout << " Speed : " << speed << "ms     ";

        gotoxy(2, 22); cout << "--------------------------";
        if (isPaused) {
            gotoxy(2, 23); cout << " STATUS: >> PAUSED <<  ";
        } else {
            gotoxy(2, 23); cout << " STATUS: Playing...    ";
        }
}

    void resetGame() {
        initGameVariables();
        initBoard();
        system("cls");
        b = validBlocks[rand() % 8];
        isPaused = false;
    }


    void draw() {
        drawSidePanel();
        // Vẽ Board game
        for (int i = 0; i < H; i++) {
            gotoxy(OFFSET_X, i);
            for (int j = 0; j < W; j++) {
                if (board[i][j] == '#') cout << "##";
                else if (board[i][j] == ' ') cout << "  ";
                else cout << "[]";
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

    void run() {

        /*HIEN THI MENU*/
        int choice = showMenu();
        if (choice == 1) {
            system("cls");
            cout << "Hen gap lai!\n";
            return;
        }

        /*MAN HINH CHOI GAME*/
        srand((unsigned int)time(0));
        system("cls");
        initBoard();
        b = rand() % 7; // Lấy random khối gạch

        while (isRunning) {
            // VÒNG LẶP KIỂM TRA PHÍM (Tối ưu phản hồi)
            // Thay vì Sleep(speed), ta chia nhỏ ra thành nhiều lần Sleep(10)
            for (int i = 0; i < speed / 10; i++) {
                if (_kbhit()) {
                    char c = _getch();
                    if (c == 'q' || c == 'Q') { isRunning = false; break; }
                    if (c == 'r' || c == 'R') {
                        system("cls");
                        initGameVariables();
                        initBoard();
                        b = rand() % 7;
                        break;
                    }
                    if (c == 'p' || c == 'P') {
                        isPaused = !isPaused;
                        drawSidePanel(); // Cập nhật trạng thái ngay lập tức
                    }

                    if (!isPaused) {
                        boardDelBlock();
                        if (c == 'a' && canMove(-1, 0)) x--;
                        if (c == 'd' && canMove(1, 0)) x++;
                        if (c == 's' && canMove(0, 1)) y++;
                        if (c == 'w' && canRotate()) rotateBlock();
                        block2Board();
                        draw(); // Vẽ lại ngay khi có thao tác phím
                    }
                }
                Sleep(10);
            }

            // LOGIC TỰ RƠI (Chỉ chạy khi không Pause)
            if (isRunning && !isPaused) {
                boardDelBlock();
                if (canMove(0, 1)) {
                    y++;
                }
                else {
                    block2Board();
                    int removed = removeLine();
                    linesCleared += removed;
                    score += calculateScore(removed);
                    if (removed > 0) speed = max(50, speed - removed * 10);
                    x = 4; y = 0; b = rand() % 7;
                    // Kiểm tra thua cuộc
                    if (!canMove(0, 0)) {
                        system("cls");
                        gotoxy(10, 10); cout << "GAME OVER! Lines: " << linesCleared;
                        Sleep(2000);
                        isRunning = false;
                    }
                }
                block2Board();
                draw();
            }
            else if (isPaused) {
                draw(); // Vẫn vẽ để hiện chữ PAUSED
            }
        }
    }
};

int main() {
    Tetris game;
    game.run();
    return 0;
}



