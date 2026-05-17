#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream>
#include <string>
#include <cstdlib> //color
#include <windows.h>
using namespace std;
#define H 20
#define W 15
#define OFFSET_X 30
#define OFFSET_RIGHT_X 65 // Độ dời của bảng chơi sang bên phải để nhường chỗ cho panel bên trái

//Hàm ẩn con trỏ chuột trong console
void hideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE; // ẩn con trỏ
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}
// Hàm hiện con trỏ chuột trong console
void showCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE; // hiện lại con trỏ
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}



class Tetris {
private:
    char board[H][W] = {};
    char blocks[7][4][4];
    int blockColors[7] = { 0, 1, 2, 3, 4, 5, 6 };
    int colorCode;
    int boardColor[H][W]; // lưu màu cho từng ô
	int nextBlock = rand() % 7; // khối tiếp theo (hiển thị ở panel bên phải)

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
            {{' ',' ',' ',' '}, {'I','I','I','I'}, {' ',' ',' ',' '}, {' ',' ',' ',' '}}, // I 1
			{{' ',' ',' ',' '}, {' ','O','O',' '}, {' ','O','O',' '}, {' ',' ',' ',' '}}, // O 2
            {{' ',' ',' ',' '}, {' ','T',' ',' '}, {'T','T','T',' '}, {' ',' ',' ',' '}}, // T 3
            {{' ',' ',' ',' '}, {' ','S','S',' '}, {'S','S',' ',' '}, {' ',' ',' ',' '}}, // S 4
            {{' ',' ',' ',' '}, {'Z','Z',' ',' '}, {' ','Z','Z',' '}, {' ',' ',' ',' '}}, // Z 5
            {{' ',' ',' ',' '}, {'J',' ',' ',' '}, {'J','J','J',' '}, {' ',' ',' ',' '}}, // J 6
            {{' ',' ',' ',' '}, {' ',' ','L',' '}, {'L','L','L',' '}, {' ',' ',' ',' '}}  // L 7
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
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (blocks[b][i][j] != ' ' && y + i < H) {
                    board[y + i][x + j] = ' ';
                    boardColor[y + i][x + j] = -1; // reset màu ô về rỗng
                }
            }
        }
    }

    void block2Board() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (blocks[b][i][j] != ' ') {
                    board[y + i][x + j] = blocks[b][i][j];
                    boardColor[y + i][x + j] = blockColors[b]; // gán màu cố định cho ô
                }
            }
        }
    }

    void initBoard() {
        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
                if ((i == H - 1) || (j == 0) || (j == W - 1) ) board[i][j] = '#';
                else board[i][j] = ' ';
    }

    /*VE BAN BEN TRAI MAN HINH GAMEPLAY*/

    // Vẽ khối tiếp theo ở góc trên bên phải
    void drawNextBlock(int nextBlock) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        int rx = OFFSET_RIGHT_X - 5; // vị trí X góc phải
        int ry = 3;              // vị trí Y góc trên

        gotoxy(rx, ry);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
  
        gotoxy(rx, ry++);cout << "   NEXT   |";
        gotoxy(rx, ry++);cout << "   BLOCK  |";
        
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

		ry = 1; 
        rx += 15; // Dời sang phải để vẽ block bên trong khung
        for (int i = 0; i < 4; i++) {
            gotoxy(rx, ry + 1 + i);
            for (int j = 0; j < 4; j++) {
                if (blocks[nextBlock][i][j] != ' ') {
                    SetColorBlock(blockColors[nextBlock]);
                    cout << "  ";
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
                else {
                    cout << "  ";
                }
            }
        }
    }

   // Hàm vẽ bảng hướng dẫn bên tay trái
    void drawLeftPanel() {
		int ry = 3; // Tọa độ Y bắt đầu của panel bên trái
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
        gotoxy(3, ry++);  cout << "==========================";
        gotoxy(3, ry++);  cout << "      TETRIS CONTROL      ";
		gotoxy(3, ry++);  cout << "=========================="; ry++;   
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        gotoxy(3, ry++);  cout << " [W] : Rotate (Xoay)";
        gotoxy(3, ry++);  cout << " [A] : Left   (Trai)";
        gotoxy(3, ry++);  cout << " [D] : Right  (Phai)";
		gotoxy(3, ry++);  cout << " [S] : Down   (Xuong)"; ry++;


        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
        gotoxy(3, ry++); cout << "--------------------------";
        gotoxy(3, ry++); cout << " [P] : PAUSE  (Tam dung)";
        gotoxy(3, ry++); cout << " [R] : RESET  (Choi lai)";
        gotoxy(3, ry++); cout << " [Q] : QUIT   (Thoat)";
        gotoxy(3, ry++); cout << "--------------------------";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }

    void drawRightPanel() {
        int elapsedSeconds = (int)difftime(time(0), startTime);
        int m = elapsedSeconds / 60;
        int s = elapsedSeconds % 60;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        int rx = OFFSET_RIGHT_X - 4; // Sử dụng biến ngắn gọn cho tọa độ X bên phải
		int ry = 8; // Tọa độ Y bắt đầu của panel bên phải

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
        gotoxy(rx, ry++);  cout << "==========================";
        gotoxy(rx, ry++);  cout << "       GAME STATUS        ";
		gotoxy(rx, ry++);  cout << "==========================";
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);


        gotoxy(rx, ry++);  cout << " Score : " << score << "          ";
        gotoxy(rx, ry++);  cout << " Best  : " << maxScore << "          ";
        gotoxy(rx, ry++);  cout << " Lines : " << linesCleared << "          ";
        gotoxy(rx, ry++);  cout << " Time  : " << m << "m " << s << "s     ";
		gotoxy(rx, ry++); cout << " Speed : " << 400 - speed << "ms     "; 


        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
        gotoxy(rx, ry++); cout << "--------------------------";
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        if (isPaused) {
            gotoxy(rx, ry++); cout << " STATUS: >> PAUSED <<  ";
        }
        else {
            gotoxy(rx, ry++); cout << " STATUS: Playing...    ";
        }

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN );
        gotoxy(rx, ry++); cout << "--------------------------";
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }

    void resetGame() {
        initGameVariables();
        initBoard();
        system("cls");
		b = nextBlock; // Sử dụng khối đã chọn làm khối đầu tiên
		nextBlock = rand() % 7; // Lấy random khối gạch tiếp theo
        isPaused = false;
    }
    void SetColorBlock(int colorCode) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        switch (colorCode) {
            case 0: SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE); break; // Màu xanh dương sáng cho khối I
            case 1: SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN | BACKGROUND_INTENSITY); break; // Màu xanh lá sáng cho khối O
            case 2: SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY); break; // Màu xanh lơ sáng cho khối T
            case 3: SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;// Màu tím sáng cho khối J
            case 4: SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY); break;// Màu vàng sáng cho khối S
            case 5: SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN); break;// Màu vàng đậm cho khối Z
            case 6: SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_BLUE); break;// Màu đỏ sáng cho khối L
        }
    }
    void draw() {
        drawLeftPanel();
        drawRightPanel();
        drawNextBlock(nextBlock);

        // Vẽ khung bao quanh toàn bộ màn hình
        int frameWidth = OFFSET_RIGHT_X + W + 9;
        int frameHeight = H;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        for (int i = 0; i < frameHeight; i++) {
            for (int j = 0; j < frameWidth; j++) {
                gotoxy(j, i);
                if (i == 0 || i == frameHeight - 1 || j == 0 || j == frameWidth - 1) {
                    
                    SetConsoleTextAttribute(hConsole, BACKGROUND_RED);
                    cout << "  ";
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
                if(i == 7 && (j >= OFFSET_RIGHT_X - 5 && j < OFFSET_RIGHT_X + W + 7)) {
                    SetConsoleTextAttribute(hConsole, BACKGROUND_RED);
                    cout << "  ";
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }   
            }
        }



        
        for (int i = 0; i < H; i++) {
            gotoxy(OFFSET_X, i);
            for (int j = 0; j < W; j++) {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                if (board[i][j] == '#' || i == 0) {
                    SetConsoleTextAttribute(hConsole, BACKGROUND_RED);
                    cout << "  ";
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
                else if (board[i][j] == ' ') {
                    cout << "  ";
                }
                else {
                    // lấy màu từ boardColor thay vì blockColors[b]
                    int colorCode = boardColor[i][j];
                    SetColorBlock(colorCode);
                    cout << "  ";
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
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
                        //system("color 1F");
                        

                        // Đổi màu nền xanh lá, chữ đỏ
                        SetConsoleTextAttribute(hConsole, BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_INTENSITY);
                        cout << "  ";
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
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

        int cx = 16;

        // Vẽ khung đỏ bao quanh màn hình Game Over
        int frameWidth = 39; // chiều rộng khung 
        int frameHeight = 18; // chiều cao khung 

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        for (int i = 0; i < frameHeight; i++) {
            for (int j = 0; j < frameWidth; j++) {
                gotoxy(cx + j, i); // dịch sang phải theo cx
                if (i == 0 || i == frameHeight - 1 || j == 0 || j == frameWidth - 1) {
                    SetConsoleTextAttribute(hConsole, BACKGROUND_RED);
                    cout << "  ";
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
            }
        }
        cx = 20;
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        gotoxy(cx, 3);  cout << "================================";
        gotoxy(cx, 4);  cout << "          GAME  OVER           ";
        gotoxy(cx, 5);  cout << "================================";
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        gotoxy(cx, 7);  cout << "   Score      :  " << score;
        gotoxy(cx, 8);  cout << "   Lines      :  " << linesCleared;
        gotoxy(cx, 9);  cout << "   Best Score :  " << maxScore;

        int m = elapsedSeconds / 60;
        int s = elapsedSeconds % 60;
        gotoxy(cx, 10); cout << "   Time       :  " << m << "m " << s << "s";


        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        gotoxy(cx, 12); cout << "--------------------------------";
        gotoxy(cx, 13); cout << "   >> Back to Menu  [Enter] << ";
        gotoxy(cx, 14); cout << "--------------------------------";
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);


        while (_getch() != '\r');
    }

    void run() {

        bool quitApp = false;


        while (!quitApp) {
            int choice = showMenu();

            if (choice == 3) { // 3: EXIT
                system("cls");
                int cx = 16;

                // Khung bao quanh thông báo Exit
                int frameWidth = 30; // rộng hơn để ôm chữ
                int frameHeight = 7;  // cao hơn để ôm nội dung

                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				 // Màu ngẫu nhiên cho khung
                while (true) {
                    int colorEndCode = rand() % 7;
                    for (int i = 0; i < frameHeight; i++) {
                        for (int j = 0; j < frameWidth; j++) {
                            gotoxy(cx + j, i);
                            if (i == 0 || i == frameHeight - 1 || j == 0 || j == frameWidth - 1) {
								SetColorBlock(colorEndCode);
                                cout << "  ";   // chỉ in 1 khoảng trắng để khung đều
                                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                            }
                        }
                    }

                    // In thông báo bên trong khung
                    gotoxy(cx + 10, 3);
					colorEndCode = rand() % 15 + 1; // Màu ngẫu nhiên cho thông báo
                    SetConsoleTextAttribute(hConsole, colorEndCode);
                    cout << "Hen gap lai!";
					Sleep(500);
                    if(_kbhit() && _getch() == '\r') {
						exit(0);
					}
                }
               
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
			b = nextBlock; // Sử dụng khối đã chọn làm khối đầu tiên
            nextBlock = rand() % 7; // Lấy random khối gạch tiếp theo
			

            while (isRunning) {
            // VÒNG LẶP KIỂM TRA PHÍM (Tối ưu phản hồi)
                for (int i = 0; i < speed / 10; i++) {
                    if (_kbhit()) {
                        char c = _getch();
                        if (c == 'q' || c == 'Q') {
                            showGameOver();  // Nếu không đặt được thì mới Game Over
                            isRunning = false;
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
                        x = 4;
                        y = 0;
                        b = nextBlock;          // dùng khối đã preview
                        nextBlock = rand() % 7; // cập nhật khối tiếp theo

                        // Kiểm tra xem khối mới có thể đặt vào vị trí ban đầu không
                        if (canMove(0, 0)) {
                            block2Board();   // Chỉ vẽ khối mới nếu còn chỗ
                        }
                        else {
                            showGameOver();  // Nếu không đặt được thì mới Game Over
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
    hideCursor(); // ẩn con trỏ ngay khi chạy
    Tetris game;
    game.run();
    showCursor(); // hiện lại con trỏ khi thoát game
    return 0;
}



