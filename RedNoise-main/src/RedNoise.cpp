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
#include <TextureMap.h>

#define WIDTH 320
#define HEIGHT 240

using namespace std;

glm::vec3 cameraPosition = glm::vec3(0.0, 0.0, 4.0);
float focalLength = 2.0;
std::vector<std::vector<float>> distance(HEIGHT);
glm::mat3 cameraOrientation = glm::mat3(1, 0, 0,
                                        0, 1, 0,
                                        0, 0, 1);

// returns an evenly spaced list (as a vector) of size numberOfValues
std::vector<float> interpolateSingleFloats(float from, float to, size_t numberOfValues) {
    std::vector<float> res;
    // if numberOfValues is 0, then return empty vector
    if (numberOfValues == 0) {
        return res;
    }

//    std::cout << "interpolateSingleFloats is called with " << from << " " << to << " " << numberOfValues << std::endl;
    float betweenValue = (to - from)/float(numberOfValues);

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

float interpolation(float x, float x1, float x2, float y1, float y2) {
    return y1 + (x - x1)*((y2 - y1)/(x2 - x1));
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

// returning the whole size of the map???
uint32_t textureMap(TextureMap& tMap, size_t t_x, size_t t_y) {
//     auto texturePixel = TextureMap("texture.ppm");
    return tMap.pixels[t_x + t_y * tMap.width];
}

// Drawing a line on the canvas
void drawLine(CanvasPoint from, CanvasPoint to, std::vector<std::vector<float>>& distance, DrawingWindow &window, Colour colour) {
//    std::cout << "drawLine is called with " << from << to << std::endl;
    uint32_t colourSet = colourPacking(colour);

    float xDistance = to.x - from.x;
    float yDistance = to.y - from.y;
    float zDistance = to.depth - from.depth;
    float numberOfSteps = max(abs(xDistance)+1, abs(yDistance)+1);

    float xStepSize = xDistance/numberOfSteps;
    float yStepSize = yDistance/numberOfSteps;
    float zStepSize = zDistance/numberOfSteps;

    for (float i = 0.0; i < numberOfSteps; i++) {
        float x = round(from.x + (xStepSize * i));
        float y = round(from.y + (yStepSize * i));
        float z = 1 / (from.depth + (zStepSize * i));

        if(distance[y][x] < z) {
//            std::cout << "z is smaller" << std::endl;
            window.setPixelColour(round(x), round(y), colourSet);
            distance[y][x]  = z;
        }
//        window.setPixelColour(round(x), round(y), colourSet);
    }
}

void texture_drawLine(CanvasPoint from, CanvasPoint to, DrawingWindow &window, TextureMap& tMap) {
    float xDistance = to.x - from.x;
    float yDistance = to.y - from.y;
    float numberOfSteps = max(abs(xDistance) + 1, abs(yDistance) + 1);
    float xStepSize = xDistance / numberOfSteps;
    float yStepSize = yDistance / numberOfSteps;

    float x_textDistance = to.texturePoint.x - from.texturePoint.x;
    float y_textDistance = to.texturePoint.y - from.texturePoint.y;
    float x_textStepSize = x_textDistance / numberOfSteps;
    float y_textStepSize = y_textDistance / numberOfSteps;

    for (float i = 0.0; i < numberOfSteps; i++) {
        float x = round(from.x + (xStepSize * i));
        float y = round(from.y + (yStepSize * i));

        float x_text = from.texturePoint.x + x_textStepSize * i;
        float y_text = from.texturePoint.y + y_textStepSize * i;

        window.setPixelColour(x, y, textureMap(tMap, x_text, y_text));
    }
}

void drawTriangle(CanvasTriangle triangle, std::vector<std::vector<float>>& distance, Colour triColour, DrawingWindow &window) {
//    std::cout << "drawTriangle is called" << std::endl;
    drawLine(triangle.v0(), triangle.v1(), distance,window, triColour);
    drawLine(triangle.v1(), triangle.v2(), distance,window, triColour);
    drawLine(triangle.v2(), triangle.v0(), distance,window, triColour);
}

void randomTriangle(DrawingWindow &window, std::vector<std::vector<float>>& distance) {
    drawTriangle(randomVertices(), distance, randomColour(), window);
}

void fillInTriangle(DrawingWindow& window, CanvasTriangle triangle, std::vector<std::vector<float>>& distance, Colour fillColour) {
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
    float z_difference = (triangle.v2().depth) - (triangle.v0().depth);
    float ratio = x_difference/y_difference;
    float smallTriBottom = (triangle.v1().y - triangle.v0().y) * ratio;

    // calculate the extra point
    CanvasPoint extraPoint = CanvasPoint((triangle.v0().x + smallTriBottom), triangle.v1().y, triangle.v1().depth);

//    float diff_1 = triangle.v1().y - triangle.v0().y + 1; ??
    float diff_1 = triangle.v1().y - triangle.v0().y;
    float diff_2 = triangle.v2().y - triangle.v1().y;

    // getting the position of the x coordinate to draw the line horizontally
    std::vector<float> sideV0ToExtra = interpolateSingleFloats(triangle.v0().x, extraPoint.x, diff_1);
    std::vector<float> sideV0ToV1 = interpolateSingleFloats(triangle.v0().x, triangle.v1().x, diff_1);
    std::vector<float> sideV1ToV2 = interpolateSingleFloats(triangle.v1().x, triangle.v2().x, diff_2);
    std::vector<float> sideExtraToV2 = interpolateSingleFloats(extraPoint.x, triangle.v2().x, diff_2);

//    std::cout << "interpolating is done" << std::endl;

    for (size_t i = 0; i < sideV0ToExtra.size(); i++) {
        // adding the y-coordinates with the side x-coordinate to get the position of the diagonal line
        auto estimatedY_from = interpolation(sideV0ToExtra[i], triangle.v0().x, extraPoint.x, triangle.v0().depth, triangle.v1().depth);
        auto estimatedY_to = interpolation(sideV0ToV1[i], triangle.v0().x, triangle.v1().x, triangle.v0().depth, triangle.v1().depth);
        drawLine(CanvasPoint(sideV0ToExtra[i], i + triangle.v0().y, estimatedY_from), CanvasPoint(sideV0ToV1[i], i + triangle.v0().y, estimatedY_to), distance, window, fillColour);
    }

    for (size_t i = 0; i < sideExtraToV2.size(); i++) {
        // adding the y-coordinates with the side x-coordinate to get the position of the diagonal line
        auto estimatedY_from = interpolation(sideExtraToV2[i], extraPoint.x, triangle.v2().x, extraPoint.depth, triangle.v2().depth);
        auto estimatedY_to = interpolation(sideV1ToV2[i], triangle.v1().x, triangle.v2().x, triangle.v1().depth, triangle.v2().depth);
        drawLine(CanvasPoint(sideExtraToV2[i], i + triangle.v1().y, estimatedY_from), CanvasPoint(sideV1ToV2[i], i + triangle.v1().y, estimatedY_to), distance, window, fillColour);
    }

    drawTriangle(triangle, distance,fillColour, window);
}

void texture_fillInTriangle(DrawingWindow& window, CanvasTriangle triangle, std::vector<std::vector<float>>& distance, TextureMap& textureMap) {
    if (triangle.v0().y > triangle.v1().y) {
        swap(triangle.vertices[0], triangle.vertices[1]);
    }
    if (triangle.v0().y > triangle.v2().y) {
        swap(triangle.vertices[0], triangle.vertices[2]);
    }
    if (triangle.v1().y > triangle.v2().y) {
        swap(triangle.vertices[1], triangle.vertices[2]);
    }

    float x_difference = (triangle.v2().x) - (triangle.v0().x);
    float y_difference = (triangle.v2().y) - (triangle.v0().y);
    float ratio = x_difference/y_difference;
    float smallTriBottom = (triangle.v1().y - triangle.v0().y) * ratio;
    CanvasPoint extraPoint = CanvasPoint((triangle.v0().x + smallTriBottom), triangle.v1().y, triangle.v1().depth);

    float diff_1 = triangle.v1().y - triangle.v0().y;
    float diff_2 = triangle.v2().y - triangle.v1().y;

    std::vector<float> sideV0ToExtra = interpolateSingleFloats(triangle.v0().x, extraPoint.x, diff_1);
    std::vector<float> sideV0ToV1 = interpolateSingleFloats(triangle.v0().x, triangle.v1().x, diff_1);
    std::vector<float> sideV1ToV2 = interpolateSingleFloats(triangle.v1().x, triangle.v2().x, diff_2);
    std::vector<float> sideExtraToV2 = interpolateSingleFloats(extraPoint.x, triangle.v2().x, diff_2);

    // linking between triangle vertices and positions on a texture map
    float x_extraPoint = interpolation(extraPoint.x, triangle.v2().x, triangle.v0().x, triangle.v2().texturePoint.x, triangle.v0().texturePoint.x);
    float y_extraPoint = interpolation(extraPoint.y, triangle.v2().y, triangle.v0().y, triangle.v2().texturePoint.y, triangle.v0().texturePoint.y);

    for (size_t i = 0; i < diff_1; i++) {
        CanvasPoint position_1(sideV0ToExtra[i], i + triangle.v0().y);
        CanvasPoint position_2(sideV0ToV1[i], i + triangle.v0().y);
        position_1.texturePoint = TexturePoint(interpolation(sideV0ToExtra[i], extraPoint.x, triangle.v0().x, x_extraPoint, triangle.v0().texturePoint.x),
                                               interpolation(i + triangle.v0().y, extraPoint.y, triangle.v0().y, y_extraPoint, triangle.v0().texturePoint.y));
        position_2.texturePoint = TexturePoint(interpolation(sideV0ToV1[i], triangle.v0().x, triangle.v1().x, triangle.v0().texturePoint.x, triangle.v1().texturePoint.x),
                                               interpolation(i + triangle.v0().y, triangle.v0().y, triangle.v1().y, triangle.v0().texturePoint.y, triangle.v1().texturePoint.y));

        texture_drawLine(position_1, position_2, window, textureMap);
    }

    for (size_t i = 0; i < diff_2; i++) {
        CanvasPoint position_1(sideExtraToV2[i], i + triangle.v1().y);
        CanvasPoint position_2(sideV1ToV2[i], i + triangle.v1().y);
        position_1.texturePoint = TexturePoint(interpolation(sideExtraToV2[i], extraPoint.x, triangle.v2().x, x_extraPoint, triangle.v2().texturePoint.x),
                                               interpolation(i + triangle.v1().y, extraPoint.y, triangle.v2().y, y_extraPoint, triangle.v2().texturePoint.y));
        position_2.texturePoint = TexturePoint(interpolation(sideV1ToV2[i], triangle.v2().x, triangle.v1().x, triangle.v2().texturePoint.x, triangle.v1().texturePoint.x),
                                               interpolation(i + triangle.v1().y, triangle.v2().y, triangle.v1().y, triangle.v2().texturePoint.y, triangle.v1().texturePoint.y));

        texture_drawLine(position_1, position_2,window, textureMap);
    }
    drawTriangle(triangle, distance, Colour(255, 255, 255), window);
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
            std::cout << "triangles last element" << triangles.back() << std::endl;
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

//void camRotation(glm::vec3& position, glm::mat3 rotatingMatrix) {
//    position *= rotatingMatrix;
//}

// Projecting on to the image plane
// initial camera position: (0.0, 0.0, 4.0) -> store in vec3 variable
// vertexPosition: 3D position of a single vertex (passed in as a vec3)
// focal length: distance = 2.0 (constant)
CanvasPoint getCanvasIntersectionPoint(glm::vec3 vertexPosition, float posRange = 1) {

    vertexPosition = cameraPosition - vertexPosition;

    //std::cout << vertexPosition << std::endl;
    float u = focalLength * (vertexPosition[0] / vertexPosition[2]) * (-1) * posRange + (WIDTH / 2);
    float v = focalLength * (vertexPosition[1] / vertexPosition[2]) * posRange + (HEIGHT / 2);
    float depth = vertexPosition[2];

    return CanvasPoint(u, v, depth);
}


void wireframe(std::vector<ModelTriangle> modelTriangles, std::vector<std::vector<float>>& distance, DrawingWindow& window) {
    for(ModelTriangle modelTriangle : modelTriangles) {
        auto v_0 = getCanvasIntersectionPoint(modelTriangle.vertices[0], 240);
        auto v_1 = getCanvasIntersectionPoint(modelTriangle.vertices[1], 240);
        auto v_2 = getCanvasIntersectionPoint(modelTriangle.vertices[2], 240);

        drawTriangle(CanvasTriangle(v_0, v_1, v_2), distance, modelTriangle.colour, window);
    }
}

void wireframeColour(std::vector<ModelTriangle> modelTriangles, std::vector<std::vector<float>>& distance, DrawingWindow& window) {
    CanvasTriangle triangle;
    for(auto modelTriangle : modelTriangles) {
        auto v_0 = getCanvasIntersectionPoint(modelTriangle.vertices[0], 180);
        auto v_1 = getCanvasIntersectionPoint(modelTriangle.vertices[1], 180);
        auto v_2 = getCanvasIntersectionPoint(modelTriangle.vertices[2], 180);

        triangle = CanvasTriangle(v_0, v_1, v_2);

        fillInTriangle(window, triangle, distance, modelTriangle.colour);
    }

//    std::cout << "done wireframeColour" << std::endl;
}

void CameraRotation(glm::vec3& cameraPosition, glm::mat3 rotationMat) {
    cameraPosition = cameraPosition * rotationMat;
}


void draw(DrawingWindow &window) {
//	window.clearPixels();
}

void handleEvent(SDL_Event event, std::vector<std::vector<float>>& distance, DrawingWindow &window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
        else if (event.key.keysym.sym == SDLK_u) {
            randomTriangle(window, distance);
        }
        else if (event.key.keysym.sym == SDLK_f) {
            fillInTriangle(window, randomVertices(), distance, randomColour());
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

    std::vector<std::vector<float>> distance(HEIGHT);

    for (size_t i = 0; i < HEIGHT; i++) {
        distance[i] = std::vector<float> (WIDTH);
    }
    // projecting W3 textureMap
    CanvasPoint p1(160, 10);
    p1.texturePoint = TexturePoint(195, 5);
    CanvasPoint p2(300, 230);
    p2.texturePoint = TexturePoint(395, 380);
    CanvasPoint p3(10, 150);
    p3.texturePoint = TexturePoint(65, 330);
    TextureMap tex = TextureMap("texture.ppm");
//    texture_fillInTriangle(window, CanvasTriangle(p1, p2, p3), distance, tex);

    std::vector<ModelTriangle> obj = readOBJFile("cornell-box.obj", 0.35);

    std::cout << obj.size() << std::endl;
    //std::cout << i << std::endl;
    //std::cout << obj[i] << std::endl;  --> don't need this?

    //projecting the box
    wireframeColour(obj, distance, window);

    std::cout << "done" << std::endl;

    SDL_Event event;

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, distance, window);
        draw(window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();

//        std::vector<float> result;
//        result = interpolateSingleFloats(2.2, 8.5, 7);
//        for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
//        std::cout << std::endl;
    }
}