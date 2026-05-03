/*
 * ASCII Terminal Racing Game
 * Copyright (C) 2026 WinGitX86
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#endif

using namespace std;

// 游戏配置
const int WIDTH = 16;
const int HEIGHT = 20;

// 全局变量
int carPos = WIDTH / 2;
int score = 0;
vector<int> obstacles;

// 跨平台按键检测
bool kbhit() {
#ifdef _WIN32
    return _kbhit();
#else
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
    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;
#endif
}

char getch() {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

// 清屏
void clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 初始化障碍
void initObstacles() {
    obstacles.clear();
    for (int i = 0; i < HEIGHT; i++) obstacles.push_back(-1);
}

// 绘制游戏画面
void draw() {
    clear();
    for (int y = 0; y < HEIGHT; y++) {
        cout << "|";
        for (int x = 0; x < WIDTH; x++) {
            if (y == HEIGHT - 1 && x == carPos) cout << "▲";
            else if (obstacles[y] == x) cout << "■";
            else cout << " ";
        }
        cout << "|\n";
    }
    cout << "Score: " << score << endl;
    cout << "← → Move | Q Quit" << endl;
}

// 更新障碍
void updateObstacles() {
    // 下移
    for (int i = HEIGHT - 1; i > 0; i--) obstacles[i] = obstacles[i - 1];
    // 顶部随机生成障碍
    obstacles[0] = rand() % WIDTH;
}

// 碰撞检测
bool checkCollision() {
    return obstacles[HEIGHT - 1] == carPos;
}

int main() {
    srand(time(0));
    initObstacles();

    while (true) {
        draw();
        score++;

        if (kbhit()) {
            char key = getch();
            if (key == 75 && carPos > 0) carPos--;    // 左箭头
            if (key == 77 && carPos < WIDTH - 1) carPos++;  // 右箭头
            if (key == 'q' || key == 'Q') break;
        }

        updateObstacles();

        if (checkCollision()) {
            clear();
            cout << "GAME OVER!" << endl;
            cout << "Final Score: " << score << endl;
            break;
        }

        usleep(120000);
    }
    return 0;
}
