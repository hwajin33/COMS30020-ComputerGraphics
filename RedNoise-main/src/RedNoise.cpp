#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <CanvasTriangle.h>
#include <cstdlib>

#define WIDTH 320
#define HEIGHT 240

using namespace std;

std::vector<float> interpolateSingleFloats(float from, float to, size_t numberOfValues) {
    numberOfValues = numberOfValues - 1;
    float betweenValue = (to - from)/numberOfValues;
    std::vector<float> res;

    for (int x = 0; x <= numberOfValues; x++) {
        res.push_back(from);
        from = from + betweenValue;
    }
    return res;
}

// (x, y, z)
vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, size_t numberOfVectors) {
    numberOfVectors = numberOfVectors - 1;
    float x_interval = (to.x - from.x)/numberOfVectors;
    float y_interval = (to.y - from.y)/numberOfVectors;
    float z_interval = (to.z - from.z)/numberOfVectors;

    vector<glm::vec3> result;

    for (int x = 0; x <= numberOfVectors; x++) {
        result.push_back(from);
        from = from + glm::vec3(x_interval, y_interval, z_interval);
    }

    return result;
}

uint32_t colourPacking(Colour col) {
    return (255 << 24) + (int(col.red) << 16) + (int(col.green) << 8) + int(col.blue);
}

Colour random_colour() {
    float red = rand()%256;
    float green = rand()%256;
    float blue = rand()%256;
    return Colour(red, green, blue);
}

CanvasTriangle random_vertices() {
    CanvasPoint v0 = CanvasPoint(rand()%WIDTH, rand()%HEIGHT);
    CanvasPoint v1 = CanvasPoint(rand()%WIDTH, rand()%HEIGHT);
    CanvasPoint v2 = CanvasPoint(rand()%WIDTH, rand()%HEIGHT);
    return CanvasTriangle(v0, v1, v2);
}

// Drawing a line on the canvas
void drawLine(CanvasPoint from, CanvasPoint to, DrawingWindow &window, Colour colour) {
    float xDistance = to.x - from.x;
    float yDistance = to.y - from.y;
    float numberOfSteps = max(abs(xDistance), abs(yDistance));
    float xStepSize = xDistance/numberOfSteps;
    float yStepSize = yDistance/numberOfSteps;
    for (float i = 0.0; i < numberOfSteps; i++) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        window.setPixelColour(round(x), round(y), colourPacking(colour));
        //(0, 127)
    }
}

void drawTriangle(CanvasTriangle triangle, Colour triColour, DrawingWindow &window) {
    drawLine(triangle.v0(), triangle.v1(), window, triColour);
    drawLine(triangle.v1(), triangle.v2(), window, triColour);
    drawLine(triangle.v2(), triangle.v0(), window, triColour);
}

void randomTriangle(DrawingWindow &window) {
    drawTriangle(random_vertices(), random_colour(), window);
}

void fillInTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour colour) {
    // sort the vertices
    if (triangle.v0().y < triangle.v1().y) {
        swap(triangle.vertices[0], triangle.vertices[1]);
    }
    if (triangle.v0().y < triangle.v2().y) {
        swap(triangle.vertices[0], triangle.vertices[2]);
    }
    if (triangle.v1().y < triangle.v2().y) {
        swap(triangle.vertices[1], triangle.vertices[2]);
    }

    float x_difference = (triangle.v2().x) - (triangle.v0().x);
    float y_difference = (triangle.v2().y) - (triangle.v0().y);
    float ratio = x_difference/y_difference;



    // cut the triangle in half by drawing a line in the middle
    //drawLine()
}

void draw(DrawingWindow &window) {
//	window.clearPixels();
//    std::vector<float> result = interpolateSingleFloats(255, 0, window.width);

//	for (size_t y = 0; y < window.height; y++) {
//		for (size_t x = 0; x < window.width; x++) {
////          float red = rand() % 256;
////			float green = 0.0;
////			float blue = 0.0;
//          uint32_t colour = (255 << 24) + (int(result[x]) << 16) + (int(result[x]) << 8) + int(result[x]);
//			window.setPixelColour(x, y, colour);
//		}
//	}

//    glm::vec3 red(255, 0, 0);        // red
//    glm::vec3 blue(0, 0, 255);       // blue
//    glm::vec3 green(0, 255, 0);    // green
//    glm::vec3 yellow(255, 255, 0);   // yellow
//
//    vector<glm::vec3> result_ColumnCalc;
//    //    result = interpolateThreeElementValues(from, to, 4);
//    vector<glm::vec3> leftColumn = interpolateThreeElementValues(red, yellow, window.height);
//    vector<glm::vec3> rightColumn = interpolateThreeElementValues(blue, green, window.height);
//
//    for(size_t y = 0; y <= window.height; y++) {
//        result_ColumnCalc = interpolateThreeElementValues(leftColumn[y], rightColumn[y], window.width);
//
//        for(size_t x = 0; x <= window.width; x++) {
//            glm::vec3 res3 = result_ColumnCalc[x];
//            uint32_t colour = (255 << 24) + (int(res3[0]) << 16) + (int(res3[1]) << 8) + int(res3[2]);
//            window.setPixelColour(x, y, colour);
//        }
//    }

////  drawing a line from left right to the middle of the canvas [W3 task 2]
//    drawLine(CanvasPoint(1, 1), CanvasPoint(window.width/2, window.height/2), window, Colour(255, 255, 255));

    //drawTriangle(CanvasTriangle(CanvasPoint(1, 1), CanvasPoint(2, 5), CanvasPoint(9, 17)), random_colour(), window);

}

void handleEvent(SDL_Event event, DrawingWindow &window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
        else if (event.key.keysym.sym == SDLK_u) {
            randomTriangle(window);
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}


int main(int argc, char *argv[]) {
//    glm::vec3 from(1.0, 4.0, 9.2);
//    glm::vec3 to(4.0, 1.0, 9.8);

    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

//    vector<glm::vec3> result;
//    for (size_t i = 0; i < result.size(); i++) std::cout << result[i].x << " " << result[i].y << " " << result[i].z << std::endl;
//    std::cout << std::endl;

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        draw(window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();

//        std::vector<float> result;
//        result = interpolateSingleFloats(2.2, 8.5, 7);
//        for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
//        std::cout << std::endl;
    }
}
