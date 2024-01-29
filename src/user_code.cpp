
#include <M5Unified.h>
int img_width = 320;
int img_height = 240;
int SQUARE_SIZE = 16;
int numSquaresX = img_width / SQUARE_SIZE + 1;
int numSquaresY = img_height / SQUARE_SIZE + 1;
int squares[21][16]; //
M5Canvas canvas(&M5.Display);

struct _pong {
    double x;
    double y;
    double dx;
    double dy;
    int class_num;
};
int pong_no = 2;

struct _pong pong[4];

double randomNum(double min, double max) {
    int rand_int = rand() % 100;
    double rand_uni = (rand_int / 100.0) * (max - min) + min;
    return rand_uni;
}

int updateSquareAndBounce(int numSquaresX, int numSquaresY, double x, double y, double &dx, double &dy, int class_num) {

    double updatedDx = dx;
    double updatedDy = dy;
    // ボールの円周上の複数のポイントをチェックする
    for(double angle = 0; angle < M_PI * 2; angle += M_PI / 4) {
        int checkX = x + cos(angle) * (SQUARE_SIZE / 2);
        int checkY = y + sin(angle) * (SQUARE_SIZE / 2);

        int i = floor(checkX / SQUARE_SIZE);
        int j = floor(checkY / SQUARE_SIZE);

        if(i >= 0 && i < numSquaresX && j >= 0 && j < numSquaresY) {
            if(squares[i][j] != class_num) {
                squares[i][j] = class_num;

                // 角度からバウンド方向を決定
                if(abs(cos(angle)) > abs(sin(angle))) {
                    updatedDx = -updatedDx;
                } else {
                    updatedDy = -updatedDy;
                }

                // ボールがループにはまらないように、バウンドにランダム性を加える。
                updatedDx += randomNum(-0.25, 0.25);
                updatedDy += randomNum(-0.25, 0.25);
            }
        }
    }

    dx = updatedDx;
    dy = updatedDy;

    return 0;
}

int checkBoundaryCollision(int image_width, int img_height, double x, double y, double &dx, double &dy) {
    if(x + dx > image_width - SQUARE_SIZE / 2 || x + dx < SQUARE_SIZE / 2) {
        dx = -dx;
    }
    if(y + dy > img_height - SQUARE_SIZE / 2 || y + dy < SQUARE_SIZE / 2) {
        dy = -dy;
    }
    return 0;
}

void setup(void) {

    auto cfg = M5.config();
    M5.begin(cfg);
    canvas.createSprite(320, 240);

    for(int i = 0; i < numSquaresX; i++) {
        for(int j = 0; j < numSquaresY; j++) {
            if(i <= (numSquaresX / 2)) {
                squares[i][j] = 0;
            } else {
                squares[i][j] = 1;
            }
            if((i <= (numSquaresX / 2)) && (j >= (numSquaresY / 2)) && (pong_no >= 3)) {
                squares[i][j] = 2;
            }
            if((i >= (numSquaresX / 2)) && (j >= (numSquaresY / 2)) && (pong_no >= 4)) {
                squares[i][j] = 3;
            }
        }
    }

    pong[0].x = img_width / 4;
    pong[0].y = img_height / 4;
    pong[0].dx = 5.0;
    pong[0].dy = -5.0;
    pong[0].class_num = 0;

    pong[1].x = img_width * 3 / 4;
    pong[1].y = img_height / 4;
    pong[1].dx = -5.0;
    pong[1].dy = 5.0;
    pong[1].class_num = 1;

    pong[2].x = img_width / 4;
    pong[2].y = img_height / 4 * 3;
    pong[2].dx = -5.0;
    pong[2].dy = 5.0;
    pong[2].class_num = 2;

    pong[3].x = img_width * 3 / 4;
    pong[3].y = img_height / 4 * 3;
    pong[3].dx = -5.0;
    pong[3].dy = 5.0;
    pong[3].class_num = 3;
}

void loop(void) {

    M5.Display.startWrite();

    for(int i = 0; i < numSquaresX; i++) {
        for(int j = 0; j < numSquaresY; j++) {
            if(squares[i][j] == 0) {
                uint16_t color = M5.Lcd.color565(237, 223, 214);
                canvas.fillRect(i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
            }
            if(squares[i][j] == 1) {
                uint16_t color = M5.Lcd.color565(200, 100, 81);
                canvas.fillRect(i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
            }
            if(squares[i][j] == 2) {
                uint16_t color = M5.Lcd.color565(103, 193, 173);
                canvas.fillRect(i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
            }
            if(squares[i][j] == 3) {
                uint16_t color = M5.Lcd.color565(00, 139, 171);
                canvas.fillRect(i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
            }
        }
    }
    for(int i = 0; i < pong_no; i++) {
        uint16_t color = M5.Lcd.color565(237, 223, 214);
        if(pong[i].class_num == 0)
            color = M5.Lcd.color565(200, 100, 81);
        if(pong[i].class_num == 1)
            color = M5.Lcd.color565(237, 223, 214);
        if(pong[i].class_num == 2)
            color = M5.Lcd.color565(00, 139, 171);
        if(pong[i].class_num == 3)
            color = M5.Lcd.color565(103, 193, 173);
        canvas.fillCircle(pong[i].x, pong[i].y, SQUARE_SIZE / 2, color);
        updateSquareAndBounce(numSquaresX, numSquaresY, pong[i].x, pong[i].y, pong[i].dx, pong[i].dy, pong[i].class_num);
        checkBoundaryCollision(img_width, img_height, pong[i].x, pong[i].y, pong[i].dx, pong[i].dy);

        pong[i].x += pong[i].dx;
        pong[i].y += pong[i].dy;
    }
    canvas.pushSprite(0, 0);

    M5.Display.endWrite();
    M5.delay(50);
    M5.update();
}

#if defined(ESP_PLATFORM) && !defined(ARDUINO)
extern "C" {
int app_main(int, char **) {
    setup();
    for(;;) {
        loop();
    }
    return 0;
}
}
#endif
