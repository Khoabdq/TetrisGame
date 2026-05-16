#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream>
#include <string>
using namespace std;
#define H 20
#define W 15
#define OFFSET_X 30
#define OFFSET_RIGHT_X 65 // Độ dời của bảng chơi sang bên phải để nhường chỗ cho panel bên trái

class Tetris {
private:
    char board[H][W] = {};
    char blocks[7][4][4];
    int x, y, b;
    int speed;
    int linesCleared;
    bool isPaused;
    bool isRunning;
    int score;
    time_t startTime;
    int elapsedSeconds;
    int maxScore;

    void gotoxy(int x, int y) {
        COORD c = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    }
    int calculateScore(int lines) {
        int pts[] = { 0, 100, 300, 500, 800 };
        if (lines >= 1 && lines <= 4) return pts[lines];
        return 0;
    }

    void loadMaxScore() {
        maxScore = 0;
        ifstream f("highscore.txt");
        if (f.is_open()) {
            f >> maxScore;
            f.close();
        }
    }

    void saveMaxScore() {
        if (score > maxScore) maxScore = score;
        ofstream f("highscore.txt");
        if (f.is_open()) {
            f<< maxScore;
            f.close();
        }
    }

public:

    Tetris() {
        isPaused = false;
        isRunning = true;
        loadMaxScore();
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

   // Hàm vẽ bảng hướng dẫn bên tay trái
    void drawLeftPanel() {
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
    }

    void drawRightPanel() {
        int elapsedSeconds = (int)difftime(time(0), startTime);
        int m = elapsedSeconds / 60;
        int s = elapsedSeconds % 60;

        int rx = OFFSET_RIGHT_X; // Sử dụng biến ngắn gọn cho tọa độ X bên phải

        gotoxy(rx, 2);  cout << "==========================";
        gotoxy(rx, 3);  cout << "       GAME STATUS        ";
        gotoxy(rx, 4);  cout << "==========================";

        gotoxy(rx, 6);  cout << " Score : " << score << "          ";
        gotoxy(rx, 7);  cout << " Best  : " << maxScore << "          ";
        gotoxy(rx, 8);  cout << " Lines : " << linesCleared << "          ";
        gotoxy(rx, 9);  cout << " Time  : " << m << "m " << s << "s     ";
        gotoxy(rx, 10); cout << " Speed : " << speed << "ms     ";

        gotoxy(rx, 12); cout << "--------------------------";
        if (isPaused) {
            gotoxy(rx, 13); cout << " STATUS: >> PAUSED <<  ";
        }
        else {
            gotoxy(rx, 13); cout << " STATUS: Playing...    ";
        }
        gotoxy(rx, 14); cout << "--------------------------";
    }

    void resetGame() {
        initGameVariables();
        initBoard();
        system("cls");
        b = rand() % 7;
        isPaused = false;
    }


    void draw() {
        drawLeftPanel();
        drawRightPanel();

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
    /* MENU CHÍNH ĐƯỢC CẬP NHẬT GIAO DIỆN KHUNG CHỮ NHẬT */
    int showMenu() {
        int selectedItem = 0; // 0: Playgame, 1: Exit, 2: Help, 3: Settings
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        string options[4] = { "PLAYGAME", "TUTORIAL", "SETTING", "EXIT"};

        while (true) {
            system("cls"); // Xóa màn hình
            SetConsoleTextAttribute(hConsole, 7); // Màu trắng mặc định

            // Vẽ viền ngoài cùng tỉ lệ và kích thước với bảng main game (20 dòng x 30 cột)
            for (int i = 0; i < H; i++) {
                gotoxy(OFFSET_X, i);
                for (int j = 0; j < W; j++) {
                    // Nếu là biên trên, biên dưới, biên trái, biên phải thì in "##"
                    if (i == 0 || i == H - 1 || j == 0 || j == W - 1) {
                        cout << "##";
                    }
                    else {
                        cout << "  "; // Không gian rỗng bên trong
                    }
                }
            }

            // In tiêu đề ở giữa khung
            gotoxy(OFFSET_X + 8, 3);
            SetConsoleTextAttribute(hConsole, 11); // Đổi màu xanh lơ cho title
            cout << "=== TETRIS ===";
            SetConsoleTextAttribute(hConsole, 7);

            // In các tùy chọn căn giữa
            for (int i = 0; i < 4; i++) {
                // Thuật toán căn giữa text: (Chiều rộng tổng là 30. Tâm là 15)
                int len = options[i].length();
                int x_pos = OFFSET_X + 15 - (len / 2) - 3; // Trừ hao 3 kí tự ">> "

                gotoxy(x_pos, 7 + i * 2); // Mỗi nút cách nhau 2 dòng

                if (selectedItem == i) {
                    SetConsoleTextAttribute(hConsole, 14); // Màu Vàng cho nút đang chọn
                    cout << ">> " << options[i] << " <<";
                    SetConsoleTextAttribute(hConsole, 7);
                }
                else {
                    cout << "   " << options[i] << "   ";
                }
            }
            char c = _getch(); // Nhận phím từ người dùng

            if (c == 'w' || c == 'W') {
                selectedItem--;
                if (selectedItem < 0) selectedItem = 3;
            }
            else if (c == 's' || c == 'S') {
                selectedItem++;
                if (selectedItem > 3) selectedItem = 0;
            }
            else if (c == '\r') { // Phím Enter
                return selectedItem;
            }
        }
    }

    void showGameOver() {
        elapsedSeconds = (int)difftime(time(0), startTime);
        saveMaxScore();
        system("cls");

        int cx = 20;

        gotoxy(cx, 3);  cout << "================================";
        gotoxy(cx, 4);  cout << "          GAME  OVER           ";
        gotoxy(cx, 5);  cout << "================================";

        gotoxy(cx, 7);  cout << "   Score      :  " << score;
        gotoxy(cx, 8);  cout << "   Lines      :  " << linesCleared;
        gotoxy(cx, 9);  cout << "   Best Score :  " << maxScore;

        int m = elapsedSeconds / 60;
        int s = elapsedSeconds % 60;
        gotoxy(cx, 10); cout << "   Time       :  " << m << "m " << s << "s";

        gotoxy(cx, 12); cout << "--------------------------------";
        gotoxy(cx, 13); cout << "   >> Back to Menu  [Enter] << ";
        gotoxy(cx, 14); cout << "--------------------------------";

        while (_getch() != '\r');
    }

    void run() {

        bool quitApp = false;


        while (!quitApp) {
            int choice = showMenu();

            if (choice == 3) { // 3: EXIT
                system("cls");
                cout << "Hen gap lai!\n";
                return;
            }
            else if (choice == 1) { // 1: TUTORIAL
                system("cls");
                cout << "me may beo\n";
                cout << "\n>> Nhan Enter de quay lai menu <<";
                while (_getch() != '\r');
                continue;
            }
            else if (choice == 2) { // 2: SETTING
                system("cls");
                cout << "me may beo\n";
                cout << "\n>> Nhan Enter de quay lai menu <<";
                while (_getch() != '\r');
                continue;
            }


            srand((unsigned int)time(0));
            system("cls");
            initGameVariables();
            initBoard();
            isRunning = true;
            b = rand() % 7; // Lấy random khối gạch

            while (isRunning) {
            // VÒNG LẶP KIỂM TRA PHÍM (Tối ưu phản hồi)
                for (int i = 0; i < speed / 10; i++) {
                    if (_kbhit()) {
                        char c = _getch();
                        if (c == 'q' || c == 'Q') {
                            isRunning = false;
                            quitApp = true;
                            break;
                        }
                        if (c == 'r' || c == 'R') {
                            resetGame();
                            break;
                        }
                        if (c == 'p' || c == 'P') {
                            isPaused = !isPaused;
                            drawLeftPanel(); // Cập nhật trạng thái ngay lập tức
                            drawRightPanel();
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
                        // Kiểm tra tinh trang thang thua
                        if (!canMove(0, 0)) {
                            showGameOver();
                            isRunning = false;
                        }
                    }
                    if (isRunning) {
                            block2Board();
                            draw();
                    }
                    block2Board();
                    draw();
                }
                else if (isPaused) {
                    draw();
                }
                }

        }
    }
};

int main() {
    Tetris game;
    game.run();
    return 0;
}



