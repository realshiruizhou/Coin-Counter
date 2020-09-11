// Shirui Zhou P5
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <list>
#include <vector>
#include <stdlib.h>
#include <chrono>
#include <limits>
#include <unordered_map>
#include <stack>
#include <string>

#define PI 3.14159265

struct pixel{
    int r;
    int g;
    int b;
};
int numCols, numRows, maxPixel;
int dimes, pennies, nickels, quarters, half_dollars;
const int lower = 90;
const int higher = 250;
int grayscale[1200][1200];
int sobel[1200][1200];
int visited[1200][1200];
int votes[1200][1200];
int final_img[1200][1200];
int centers[1200][1200];
int visited2[1200][1200];
pixel init_img[1200][1200];
const double thres = 2.25;
using namespace std;

struct GVect{
    int gradientx;
    int gradienty;
    int magnitude;
};

GVect gradients[1200][1200];

void drawCircleHelper(int xc, int yc, int x, int y){
    if(xc + x < numRows && yc + y < numCols){
        votes[xc+x][yc+y] += 1;
    }
    if(xc - x >= 0 && yc + y < numCols){
        votes[xc-x][yc+y] += 1;
    }
    if(xc + x < numRows && yc - y >= 0) {
        votes[xc + x][yc - y] += 1;
    }
    if(xc - x >= 0 && yc - y >= 0) {
        votes[xc - x][yc - y] += 1;
    }
    if(xc + y < numRows && yc + x < numCols) {
        votes[xc + y][yc + x] += 1;
    }
    if(xc - y >= 0 && yc + x < numCols) {
        votes[xc - y][yc + x] += 1;
    }
    if(xc + y < numRows && yc - x >= 0) {
        votes[xc + y][yc - x] += 1;
    }
    if(xc - y >= 0 && yc - x >= 0) {
        votes[xc - y][yc - x] += 1;
    }
}

void drawCircle(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    drawCircleHelper(xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else {
            d = d + 4 * x + 6;
        }
        drawCircleHelper(xc, yc, x, y);
    }
}

void drawCircleHelper2(int xc, int yc, int x, int y){
    if(xc + x < numRows && yc + y < numCols){
        init_img[xc+x][yc+y] = {255, 0 ,0};
    }
    if(xc - x >= 0 && yc + y < numCols){
        init_img[xc-x][yc+y] = {255, 0 ,0};
    }
    if(xc + x < numRows && yc - y >= 0) {
        init_img[xc + x][yc - y] = {255, 0 ,0};
    }
    if(xc - x >= 0 && yc - y >= 0) {
        init_img[xc - x][yc - y] = {255, 0 ,0};
    }
    if(xc + y < numRows && yc + x < numCols) {
        init_img[xc + y][yc + x] = {255, 0 ,0};
    }
    if(xc - y >= 0 && yc + x < numCols) {
        init_img[xc - y][yc + x] = {255, 0 ,0};
    }
    if(xc + y < numRows && yc - x >= 0) {
        init_img[xc + y][yc - x] = {255, 0 ,0};
    }
    if(xc - y >= 0 && yc - x >= 0) {
        init_img[xc - y][yc - x] = {255, 0 ,0};
    }
}

void drawCircle2(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    drawCircleHelper2(xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else {
            d = d + 4 * x + 6;
        }
        drawCircleHelper2(xc, yc, x, y);
    }
}

void recur(int x, int y){
    if(x <= 0 || y <= 0 || x >= numRows || y >= numCols) {
        return;
    }
    if(visited[x][y] == 1){
        return;
    }
    final_img[x][y] = 255;
    visited[x][y] = 1;
    if(final_img[x][y + 1] > 100){
        recur(x, y + 1);
    }
    if(final_img[x + 1][y + 1] > 100){
        recur(x + 1, y + 1);
    }
    if(final_img[x - 1][y + 1] > 100){
        recur(x - 1, y + 1);
    }
    if(final_img[x][y - 1] > 100){
        recur(x, y - 1);
    }
    if(final_img[x + 1][y - 1] > 100){
        recur(x + 1, y - 1);
    }
    if(final_img[x - 1][y - 1] > 100){
        recur(x - 1, y - 1);
    }
    if(final_img[x + 1][y] > 100){
        recur(x + 1, y);
    }
    if(final_img[x - 1][y] > 100){
        recur(x - 1, y);
    }
}

void canny(){
    string type;
    string cols;
    string rows;
    string max;
    ifstream img("image.ppm");
    img >> type;
    img >> cols;
    numCols = stoi(cols);
    img >> rows;
    numRows = stoi(rows);
    img >> max;
    maxPixel = stoi(max);
    for(int j = 0; j < numRows; j++){
        for(int k = 0; k < numCols; k++){
            int r, g, b;
            img >> r;
            img >> g;
            img >> b;
            pixel t = {r, g, b};
            init_img[j][k] = t;
            int gray = (int)(0.2126 * r + 0.7152 * g + 0.0722 * b + .5);
            grayscale[j][k] = gray;
            sobel[j][k] = gray;
            GVect temp = {0, 0, 0};
            gradients[j][k] = temp;
        }
    }
    img.close();

    ofstream out("imageg.ppm");
    out << "P3" << endl;
    out << numCols << " " << numRows << endl;
    out << "255" << endl;

    for(int x = 0; x < numRows; x++){
        for(int y = 0; y < numCols; y++){
            int g = grayscale[x][y];
            out << g << " " << g << " " << g << endl;
        }
    }
    out.close();

    for(int j = 1; j < numRows - 1; j++){
        for(int k = 1; k < numCols - 1; k++){
            int gy = 0;
            gy += grayscale[j - 1][k - 1] * 1;
            gy += grayscale[j - 1][k] * 2;
            gy += grayscale[j][k + 1] * 0;
            gy += grayscale[j - 1][k + 1] * 1;
            gy += grayscale[j + 1][k + 1] * -1;
            gy += grayscale[j + 1][k - 1] * - 1;
            gy += grayscale[j + 1][k] * -2;
            gy += grayscale[j][k - 1] * 0;

            int gx = 0;
            gx += grayscale[j - 1][k] * 0;
            gx += grayscale[j - 1][k - 1] * -1;
            gx += grayscale[j + 1][k + 1] * 1;
            gx += grayscale[j][k - 1] * -2;
            gx += grayscale[j][k + 1] * 2;
            gx += grayscale[j - 1][k + 1] * 1;
            gx += grayscale[j + 1][k - 1] * - 1;
            gx += grayscale[j + 1][k] * 0;
            int gradient = (int)(sqrt(pow(gx, 2) + pow(gy, 2)) + .5);

            if(gradient > higher) {
                sobel[j][k] = 255;
                final_img[j][k] = 255;
            }
            else if(gradient > lower){
                sobel[j][k] = 125;
                final_img[j][k] = 125;
            }
            else{
                sobel[j][k] = 0;
                final_img[j][k] = 0;
            }
            GVect toAdd = {gx, gy, gradient};
            gradients[j][k] = toAdd;
        }
    }

    ofstream final("imagem.ppm");
    final << "P3" << endl;
    final << numCols << " " << numRows << endl;
    final << "255" << endl;

    for(int x = 0; x < numRows; x++){
        for(int y = 0; y < numCols; y++){
            int g = sobel[x][y];
            final << g << " " << g << " " << g << endl;
        }
    }
    final.close();

    for(int x = 1; x < numRows - 1; x++){
        for(int y = 1; y < numCols - 1; y++){
            if(sobel[x][y] == 255 || sobel[x][y] == 125){
                double angle;
                if(gradients[x][y].gradientx == 0){
                    angle = 0;
                }
                else{
                    angle = atan(gradients[x][y].gradienty/gradients[x][y].gradientx) * 180 / PI;
                    angle = round(angle / 45) * 45;
                }
                bool greatest = true;
                if(angle == -90 || angle == 90){
                    if(gradients[x][y].magnitude < gradients[x + 1][y].magnitude || gradients[x][y].magnitude < gradients[x - 1][y].magnitude){
                        greatest = false;
                    }
                }
                else if(angle == 0){
                    if(gradients[x][y].magnitude < gradients[x][y + 1].magnitude || gradients[x][y].magnitude < gradients[x][y - 1].magnitude){
                        greatest = false;
                    }
                }
                else if(angle == 45){
                    if(gradients[x][y].magnitude < gradients[x - 1][y + 1].magnitude || gradients[x][y].magnitude < gradients[x + 1][y - 1].magnitude){
                        greatest = false;
                    }
                }
                else if(angle == -45){
                    if(gradients[x][y].magnitude < gradients[x + 1][y + 1].magnitude || gradients[x][y].magnitude < gradients[x - 1][y - 1].magnitude){
                        greatest = false;
                    }
                }
                if(!greatest){
                    final_img[x][y] = 0;
                    sobel[x][y] = 0;
                }
                else{
                    final_img[x][y] = sobel[x][y];
                }
            }
        }
    }

    ofstream nms("image1.ppm");
    nms << "P3" << endl;
    nms << numCols << " " << numRows << endl;
    nms << "255" << endl;

    for(int x = 0; x < numRows; x++){
        for(int y = 0; y < numCols; y++){
            int g = final_img[x][y];
            nms << g << " " << g << " " << g << endl;
        }
    }
    nms.close();

    for(int x = 0; x < numRows; x++){
        for(int y = 0; y < numCols; y++){
            if(final_img[x][y] == 255){
                recur(x, y);
            }
        }
    }

    ofstream fin("imagef.ppm");
    fin << "P3" << endl;
    fin << numCols << " " << numRows << endl;
    fin << "255" << endl;

    for(int x = 0; x < numRows; x++){
        for(int y = 0; y < numCols; y++){
            int g;
            if(final_img[x][y] < 255) {
                g = 0;
            }
            else{
                g = 255;
            }
            fin << g << " " << g << " " << g << endl;
        }
    }
    fin.close();
}

bool loneEdge(int x, int y){
    int neighbors = 0;
    if(final_img[x - 1][y] == 255){
        neighbors += 1;
    }
    if(final_img[x + 1][y] == 255){
        neighbors += 1;
    }
    if(final_img[x][y - 1] == 255){
        neighbors += 1;
    }
    if(final_img[x][y + 1] == 255){
        neighbors += 1;
    }
    if(final_img[x + 1][y + 1] == 255){
        neighbors += 1;
    }
    if(final_img[x - 1][y - 1] == 255){
        neighbors += 1;
    }
    if(final_img[x - 1][y + 1] == 255){
        neighbors += 1;
    }
    if(final_img[x + 1][y - 1] == 255){
        neighbors += 1;
    }
    return neighbors <= 1;
}

void clear(int x, int y){
    if(x <= 0 || y <= 0 || x >= numRows || y >= numCols) {
        return;
    }
    if(visited2[x][y] == 1){
        return;
    }
    visited2[x][y] = 1;
    if(centers[x][y + 1] == 255){
        centers[x][y + 1] = 0;
        recur(x, y + 1);
    }
    if(centers[x + 1][y + 1] == 255){
        centers[x + 1][y + 1] = 0;
        recur(x + 1, y + 1);
    }
    if(centers[x - 1][y + 1] == 255){
        centers[x - 1][y + 1] = 0;
        recur(x - 1, y + 1);
    }
    if(centers[x][y - 1] == 255){
        centers[x][y - 1] = 0;
        recur(x, y - 1);
    }
    if(centers[x + 1][y - 1] == 255){
        centers[x + 1][y - 1] = 0;
        recur(x + 1, y - 1);
    }
    if(centers[x - 1][y - 1] == 255){
        centers[x - 1][y - 1] = 0;
        recur(x - 1, y - 1);
    }
    if(centers[x + 1][y] == 255){
        centers[x + 1][y] = 0;
        recur(x + 1, y);
    }
    if(centers[x - 1][y] == 255){
        centers[x - 1][y] = 0;
        recur(x - 1, y);
    }
}
void circles(){ //[22, 31) for 1, [22, 29) for 2, [) for 3
    for(int r = 20; r < 45; r++) {
        for (int x = 1; x < numRows - 1; x++) {
            for (int y = 1; y < numCols - 1; y++) {
                if (final_img[x][y] == 255 && !loneEdge(x, y)) {
                    drawCircle(x, y, r);
                }
            }
        }
        for (int x = 1; x < numRows - 1; x++) {
            for (int y = 1; y < numCols - 1; y++) {
                if (votes[x][y] > thres * r) {
                    drawCircle2(x, y, r);
//                    if(r < 24){
//                        pennies++;
//                    }
//                    else if(r == 26){
//                        nickels++;
//                    }
//                    else if(r > 29){
//                        quarters++;
//                    }

                    if(r < 21){
                        dimes++;
                    }
                    if(r >=21 && r < 23){
                        pennies++;
                    }
                    else if(24 <= r && r < 26){
                        nickels++;
                    }
                    else if(r > 27 && r < 35){
                        quarters++;
                    }
                    else if(r >= 35){
                        half_dollars++;
                    }
                }
                votes[x][y] = 0;
            }
        }
    }
    ofstream fin2("votes.ppm");
    fin2 << "P3" << endl;
    fin2 << numCols << " " << numRows << endl;
    fin2 << "255" << endl;

    for(int x = 0; x < numRows; x++){
        for(int y = 0; y < numCols; y++){
            int g;
            if(votes[x][y] >= 255){
                g = 255;
            }
            else{
                g = votes[x][y];
            }
            fin2 << g << " " << g << " " << g << endl;
        }
    }
    fin2.close();

//    ofstream fin("center.ppm");
//    fin << "P3" << endl;
//    fin << numCols << " " << numRows << endl;
//    fin << "255" << endl;
//
//    for(int x = 0; x < numRows; x++){
//        for(int y = 0; y < numCols; y++){
//            int f = centers[x][y];
//            fin << f << " " << "0" << " " << "0" << endl;
//        }
//    }
//    fin.close();

    ofstream fin3("imagec.ppm");
    fin3 << "P3" << endl;
    fin3 << numCols << " " << numRows << endl;
    fin3 << "255" << endl;

    for(int x = 0; x < numRows; x++){
        for(int y = 0; y < numCols; y++){
            pixel temp = init_img[x][y];
            fin3 << temp.r << " " << temp.g << " " << temp.b << endl;
        }
    }
    fin3.close();
}

int main(){
    canny();
    circles();
    double total = .01 * pennies + .05 * nickels + .1 * dimes + .25 * quarters + .5 * half_dollars;
    cout<<"There are "<< pennies << " pennies, " << nickels << " nickels, " << dimes << " dimes, " << quarters <<" quarters, and " << half_dollars <<" half dollars for a total of $" << total;
    std::ofstream out("results.txt");
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());
    cout<<"There are "<< pennies << " pennies, " << nickels << " nickels, " << dimes << " dimes, " << quarters <<" quarters, and " << half_dollars <<" half dollars for a total of $" << total;
    return 0;
}