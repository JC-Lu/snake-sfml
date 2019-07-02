#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

//  window width and height
const int win_w = 960;
const int win_h = 540;

//  node width and height
const int node_w = 30;
const int node_h = 30;

//  node direction
enum Direction {
    UP, RIGHT, DOWN, LEFT
};

//  snake body node
struct Node {
    //  node x position
    int x;
    //  node y position
    int y;
    //  node direction
    Direction direction;
};

//  snake all node
vector<Node> snake;
//  apple sprite
Sprite s_apple;
//  snake direction
Direction direction;
//  game start or stop
bool game_start = true;
//  game score
int score = 0;

//  game initialize
void init();

void init_apple();

//  snake move to direction
void move();

//  snake collision
void collision();

//  add snake body node
void add_node();

int main(int argc, char **args) {

    RenderWindow window(VideoMode(win_w, win_h), "Snake");

    //  load game background texture
    Texture t_background;
    if (!t_background.loadFromFile("res/images/d.png")) {
        return EXIT_FAILURE;
    }

    //  load snake head and body and tail texture
    Texture heads[4], tail[4], body, apple;
    for (int index = 0; index < 4; index++) {
        char path[1024];
        if (!sprintf(path, "res/images/head_%d.png", index)) {
            return EXIT_FAILURE;
        }
        heads[index].loadFromFile(path);
        if (!sprintf(path, "res/images/end_%d.png", index)) {
            return EXIT_FAILURE;
        }
        tail[index].loadFromFile(path);
    }

    //  load apple texture
    if (!body.loadFromFile("res/images/body.png") ||
        !apple.loadFromFile("res/images/apple.png")) {
        return EXIT_FAILURE;
    }

    //  create background sprite
    Sprite s_background(t_background);

    //  create snake head sprite and tail sprite
    Sprite s_heads[4], s_tails[4], s_body(body);
    for (int index = 0; index < 4; index++) {
        s_heads[index] = Sprite(heads[index]);
        s_tails[index] = Sprite(tail[index]);
    }
    s_apple = Sprite(apple);
    s_apple.setScale(0.6, 0.6);

    //  load font
    Font font;
    if (!font.loadFromFile("res/ttf/simhei.ttf")) {
        return EXIT_FAILURE;
    }

    //  game options text
    Text optionText = Text("W(UP), S(DOWN), A(LEFT), D(RIGHT), R(REST GAME)", font, 30);
    optionText.setFillColor(Color::Blue);

    //  game score text
    char scoreStr[256];
    sprintf(scoreStr, "SCORE: %d", score);
    Text scoreText = Text(scoreStr, font, 30);
    scoreText.setFillColor(Color::Blue);
    scoreText.setPosition(0, 30);

    init();
    game_start = false;

    Clock clock;
    float timer = 0, delay = 0.1;

    while (window.isOpen()) {

        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        //  listening window closed
        Event e{};
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }
        }

        //  listening key down ( W and D and S and A )
        if (Keyboard::isKeyPressed(Keyboard::W)) { direction = UP; }
        if (Keyboard::isKeyPressed(Keyboard::D)) { direction = RIGHT; }
        if (Keyboard::isKeyPressed(Keyboard::S)) { direction = DOWN; }
        if (Keyboard::isKeyPressed(Keyboard::A)) { direction = LEFT; }
        if (Keyboard::isKeyPressed(Keyboard::R)) {
            init();
        }

        //  snake move
        if (timer > delay && game_start) {
            timer = 0;
            move();
        }

        collision();

        //  clear window screen
        window.clear();

        //  draw game background
        window.draw(s_background);

        //  draw snake
        for (auto it = snake.begin(); it != snake.end(); it++) {
            if (it == snake.begin()) {
                switch ((*it).direction) {
                    case UP:
                        s_heads[0].setPosition((*it).x, (*it).y);
                        window.draw(s_heads[0]);
                        break;
                    case RIGHT:
                        s_heads[1].setPosition((*it).x, (*it).y);
                        window.draw(s_heads[1]);
                        break;
                    case DOWN:
                        s_heads[2].setPosition((*it).x, (*it).y);
                        window.draw(s_heads[2]);
                        break;
                    case LEFT:
                        s_heads[3].setPosition((*it).x, (*it).y);
                        window.draw(s_heads[3]);
                        break;
                }
            } else if (it == snake.end() - 1) {
                switch ((*it).direction) {
                    case UP:
                        s_tails[0].setPosition((*it).x, (*it).y);
                        window.draw(s_tails[0]);
                        break;
                    case RIGHT:
                        s_tails[1].setPosition((*it).x, (*it).y);
                        window.draw(s_tails[1]);
                        break;
                    case DOWN:
                        s_tails[2].setPosition((*it).x, (*it).y);
                        window.draw(s_tails[2]);
                        break;
                    case LEFT:
                        s_tails[3].setPosition((*it).x, (*it).y);
                        window.draw(s_tails[3]);
                        break;
                }
            } else {
                s_body.setPosition((*it).x, (*it).y);
                window.draw(s_body);
            }
        }

        //  draw apple
        window.draw(s_apple);

        //  draw options text
        window.draw(optionText);

        //  draw score text
        sprintf(scoreStr, "SCORE: %d", score);
        scoreText.setString(scoreStr);
        window.draw(scoreText);

        //  update screen display
        window.display();
    }


    return EXIT_SUCCESS;
}

//  game initialize
void init() {

    game_start = true;
    score = 0;

    //  vector not empty clear all node
    if (!snake.empty()) {
        snake.clear();
    }

    direction = RIGHT;

    //  create head and body and tail node
    Node head = {90, 270, direction};
    Node body = {60, 270, direction};
    Node tail = {30, 270, direction};

    //  initialize apple position
    init_apple();

    //  push to vector
    snake.push_back(head);
    snake.push_back(body);
    snake.push_back(tail);
}

void init_apple() {
    default_random_engine random_engine((unsigned) time(nullptr));
    s_apple.setPosition(random_engine() % 32 * 30, random_engine() % 18 * 30);
}

//  snake move to direction
void move() {

    Node up{};

    for (auto it = snake.begin(); it != snake.end(); it++) {
        if (it == snake.begin()) {
            up = (*it);
            switch (direction) {
                case UP:
                    (*it).y -= 30;
                    break;
                case RIGHT:
                    (*it).x += 30;
                    break;
                case DOWN:
                    (*it).y += 30;
                    break;
                case LEFT:
                    (*it).x -= 30;
                    break;
            }
            (*it).direction = direction;
        } else {
            auto swap = (*it);
            (*it).x = up.x;
            (*it).y = up.y;
            (*it).direction = up.direction;
            up = swap;
        }
    }
}

void collision() {

    Node head = snake[0];

    // if snake overstep bound
    if (head.x > 930 || head.x < 0 || head.y > 530 || head.y < 0) {
        game_start = false;
    }

    // if snake eat apple
    auto apple_pos = s_apple.getPosition();
    if (head.x >= apple_pos.x && head.x < apple_pos.x + node_w && head.y >= apple_pos.y &&
        head.y < apple_pos.y + node_h) {
        init_apple();
        add_node();
        score += 1;
    }

    //  if snake eat self
    for (auto it = snake.begin() + 1; it != snake.end(); it++) {
        auto node = *it;
        if (head.x >= node.x && head.x < node.x + node_w && head.y >= node.y &&
            head.y < node.y + node_h) {
            game_start = false;
        }
    }
}

void add_node() {

    Node node{};
    Node tail = *(snake.end() - 1);
    switch (tail.direction) {
        case LEFT:
            node.y = tail.y;
            node.x = tail.x + node_w;
            node.direction = tail.direction;
            break;
        case RIGHT:
            node.y = tail.y;
            node.x = tail.x - node_w;
            node.direction = tail.direction;
            break;
        case UP:
            node.y = tail.y + node_h;
            node.x = tail.x;
            node.direction = tail.direction;
            break;
        case DOWN:
            node.y = tail.y - node_h;
            node.x = tail.x;
            node.direction = tail.direction;
            break;
    }
    snake.insert(snake.end(), node);
}