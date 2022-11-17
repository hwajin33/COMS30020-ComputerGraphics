#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include <Colour.h>
#include <CanvasTriangle.h>
#include <CanvasPoint.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ModelTriangle.h>
#include <map>

#define WIDTH 320
#define HEIGHT 240

using namespace std;

// returns an evenly spaced list (as a vector) of size numberOfValues
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

Colour randomColour() {
    float red = rand()%256;
    float green = rand()%256;
    float blue = rand()%256;
    return Colour(red, green, blue);
}

CanvasTriangle randomVertices() {
    CanvasPoint v0 = CanvasPoint(rand()%WIDTH, rand()%HEIGHT);
    CanvasPoint v1 = CanvasPoint(rand()%WIDTH, rand()%HEIGHT);
    CanvasPoint v2 = CanvasPoint(rand()%WIDTH, rand()%HEIGHT);
    return CanvasTriangle(v0, v1, v2);
}

// Drawing a line on the canvas
void drawLine(CanvasPoint from, CanvasPoint to, DrawingWindow &window, Colour colour) {
    float xDistance = to.x - from.x;
    float yDistance = to.y - from.y;
    //float zDistance = to.depth - from.depth;
    float numberOfSteps = max(abs(xDistance), abs(yDistance));
    float xStepSize = xDistance/numberOfSteps;
    float yStepSize = yDistance/numberOfSteps;
    //float zStepSize = zDistance/numberOfSteps;
    for (float i = 0.0; i < numberOfSteps; i++) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        //float z = 1 / (from.depth + (zStepSize * i));
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
    drawTriangle(randomVertices(), randomColour(), window);
}

void fillInTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour fillColour) {
    // sort the vertices
    if (triangle.v0().y > triangle.v1().y) {
        swap(triangle.vertices[0], triangle.vertices[1]);
    }
    if (triangle.v0().y > triangle.v2().y) {
        swap(triangle.vertices[0], triangle.vertices[2]);
    }
    if (triangle.v1().y > triangle.v2().y) {
        swap(triangle.vertices[1], triangle.vertices[2]);
    }

    // finding the horizontal line of the triangle -> length of the bottom of the small triangle
    float x_difference = (triangle.v2().x) - (triangle.v0().x);
    float y_difference = (triangle.v2().y) - (triangle.v0().y);
    //float z_difference = (triangle.v2().depth) - (triangle.v0().depth);
    float ratio = x_difference/y_difference;
    float smallTriBottom = (triangle.v1().y - triangle.v0().y) * ratio;

    // calculate the extra point
    CanvasPoint extraPoint = CanvasPoint((triangle.v0().x + smallTriBottom), triangle.v1().y);

    float diff_1 = triangle.v1().y - triangle.v0().y;
    float diff_2 = triangle.v2().y - triangle.v1().y;

    // getting the position of the x coordinate to draw the line horizontally
    std::vector<float> sideV0ToExtra = interpolateSingleFloats(triangle.v0().x, extraPoint.x, diff_1);
    std::vector<float> sideV0ToV1 = interpolateSingleFloats(triangle.v0().x, triangle.v1().x, diff_1);
    std::vector<float> sideV1ToV2 = interpolateSingleFloats(triangle.v1().x, triangle.v2().x, diff_2);
    std::vector<float> sideExtraToV2 = interpolateSingleFloats(extraPoint.x, triangle.v2().x, diff_2);

    for (size_t i = 0; i < sideV0ToExtra.size(); i++) {
        // adding the y-coordinates with the side x-coordinate to get the position of the diagonal line
        drawLine(CanvasPoint(sideV0ToExtra[i], i + triangle.v0().y), CanvasPoint(sideV0ToV1[i], i + triangle.v0().y),window, fillColour);
    }

    for (size_t i = 0; i < sideExtraToV2.size(); i++) {
        // adding the y-coordinates with the side x-coordinate to get the position of the diagonal line
        drawLine(CanvasPoint(sideExtraToV2[i], i + triangle.v1().y), CanvasPoint(sideV1ToV2[i], i + triangle.v1().y), window, fillColour);
    }

    drawTriangle(triangle, fillColour, window);

    // draw white line border
    Colour whiteBorder = Colour(255, 255, 255);
    drawLine(triangle.vertices[0], triangle.vertices[1], window, whiteBorder);
    drawLine(triangle.vertices[0], triangle.vertices[2], window, whiteBorder);
    drawLine(triangle.vertices[1], triangle.vertices[2], window, whiteBorder);

    // cut the triangle in half by drawing a line in the middle
    //drawLine()
}

// Reading the mtl file and getting the information of colours for the palette
std::map<std::string, Colour> readMTLFile(const std::string& filename) {
    std::ifstream readFile(filename);
    std::string line;
    std::map<std::string, Colour> getPalette;
    Colour getColour;

    while (std::getline(readFile, line)) {
        std::cout << line << std::endl;
        auto tokens = split(line, ' ');

        if (tokens[0] == "newmtl") {
            getColour.name = tokens[1];
        }
        else if (tokens[0] == "Kd") {
            getPalette.insert({getColour.name, Colour(std::stof(tokens[1]) * 255, std::stof(tokens[2]) * 255, std::stof(tokens[3]) * 255)});
        }
    }
    return getPalette;
}

// Reading the obj file with the already extracted information from the mtl file
// scaleFloat -> scales the position of all vertices at the point at which they are read in from the file, adjust the size of the model when it is loaded.
std::vector<ModelTriangle> readOBJFile(const std::string& filename, float scaleFloat) {
    std::ifstream readFile(filename);
    std::string line;
    std::vector<glm::vec3> objVertices;
    std::vector<ModelTriangle> triangles;
    Colour currentColour;
    std::map<std::string, Colour> palette;

    while (std::getline(readFile, line)) {
        auto tokens = split(line, ' ');

        if (tokens[0] == "v") {
            objVertices.push_back(glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])) * scaleFloat);
        }
        else if (tokens[0] == "f") {
            std::cout << "c" << currentColour << std::endl;
            triangles.push_back(ModelTriangle(objVertices[std::stoi(tokens[1]) - 1], objVertices[std::stoi(tokens[2]) - 1], objVertices[std::stoi(tokens[3]) - 1], currentColour));
        }
        else if (tokens[0] == "usemtl") {
            // call readMTLFile() to access the palette
            //std::vector<ModelTriangle> readMTLFile("cornell-box.mtl");
            std::cout << line << std::endl;
            currentColour = palette[tokens[1]];

        }
        else if (tokens[0] == "mtllib") {
            std::cout << line << std::endl;
            palette = readMTLFile(tokens[1]);
        }
    }
    return triangles;
}

// Projecting on to the image plane
// initial camera position: (0.0, 0.0, 4.0) -> store in vec3 variable
// focal length: distance = 2.0 (constant)
CanvasPoint getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::vec3 vertexPosition, float focalLength, float posRange = 1) {
    CanvasPoint result_imagePlanePosition(float u, float v);

    vertexPosition = cameraPosition - vertexPosition;
//    vec3<float, (glm::precision)> distance_x = cameraPosition[0] - vertexPosition[0];
//    vec3<float, (glm::precision)> distance_y = cameraPosition[1] - vertexPosition[1];
//    vec3<float, (glm::precision)> distance_z = cameraPosition[2] - vertexPosition[2];

   float u = focalLength * (vertexPosition[0] / vertexPosition[2]) * (-1) * posRange + (WIDTH / 2);
   float v = focalLength * (vertexPosition[1] / vertexPosition[2]) * (-1) * posRange + (HEIGHT / 2);

//    result_imagePlanePosition.push_back(position_u);
//    result_imagePlanePosition.push_back(position_v);
    return CanvasPoint(u, v);
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
        else if (event.key.keysym.sym == SDLK_f) {
            fillInTriangle(window, randomVertices(), randomColour());
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}


int main(int argc, char *argv[]) {
//    glm::vec3 from(1.0, 4.0, 9.2);
//    glm::vec3 to(4.0, 1.0, 9.8);
    std::vector<ModelTriangle> obj = readOBJFile("cornell-box.obj", 10.35);


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

