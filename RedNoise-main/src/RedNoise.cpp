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
#include <RayTriangleIntersection.h>
#include <chrono>
#include <thread>
#include <cstdio>


#define WIDTH 320
#define HEIGHT 320

using namespace std;



glm::vec3 cameraPosition(0, 0, 4);
//glm::vec3 lightPosition = glm::vec3(0, 0.5, 1);
glm::vec3 lightPosition = glm::vec3(0, 0.4, 0.6);
float focalLength = 2.0;
std::vector<std::vector<float>> distance(HEIGHT);
glm::mat3 cameraOrientation = glm::mat3(1, 0, 0,
                                        0, 1, 0,
                                        0, 0, 1);
int render = 0;

// returns an evenly spaced list (as a vector) of size numberOfValues
std::vector<float> interpolateSingleFloats(float from, float to, size_t numberOfValues) {
    std::vector<float> res;
    // if numberOfValues is 0, then return empty vector
    if (numberOfValues == 0)  return res;
    float betweenValue = (to - from)/float(numberOfValues);
    for (int i = 0; i <= numberOfValues; i++) res.push_back(from + i * betweenValue);
    return res;
}

// (x, y, z)
vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, size_t numberOfVectors) {
    std::vector<glm::vec3> res;
    if (numberOfVectors == 0)  return res;
    glm::vec3 betweenValue = (to - from)/glm::vec3(numberOfVectors - 1);
    for (int i = 0; i <= numberOfVectors; i++) res.push_back(from + glm::vec3(i) * betweenValue);
    return res;
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


// Drawing a line on the canvas
void drawLine(CanvasPoint from, CanvasPoint to, std::vector<std::vector<float>>& distance, DrawingWindow &window, Colour colour) {
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

        if( x >= 0 && y >= 0 && y < distance.size() && x < distance[y].size() && distance[y][x] < z) {
            window.setPixelColour(round(x), round(y), colourSet);
            distance[y][x]  = z;
        }
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
    float ratio = x_difference/y_difference;
    float smallTriBottom = (triangle.v1().y - triangle.v0().y) * ratio;

    // calculate the extra point
    CanvasPoint extraPoint = CanvasPoint((triangle.v0().x + smallTriBottom), triangle.v1().y,
                                         interpolation(triangle.v1().y, triangle.v0().y, triangle.v2().y, triangle.v0().depth, triangle.v2().depth));

//    float diff_1 = triangle.v1().y - triangle.v0().y + 1; ??
    float diff_1 = triangle.v1().y - triangle.v0().y + 1;
    float diff_2 = triangle.v2().y - triangle.v1().y + 1;

    // getting the position of the x coordinate to draw the line horizontally
    std::vector<float> sideV0ToExtra = interpolateSingleFloats(triangle.v0().x, extraPoint.x, diff_1);
    std::vector<float> sideV0ToV1 = interpolateSingleFloats(triangle.v0().x, triangle.v1().x, diff_1);
    std::vector<float> sideV1ToV2 = interpolateSingleFloats(triangle.v1().x, triangle.v2().x, diff_2);
    std::vector<float> sideExtraToV2 = interpolateSingleFloats(extraPoint.x, triangle.v2().x, diff_2);

//    std::cout << "interpolating is done" << std::endl;

    for (size_t i = 0; i < sideV0ToExtra.size(); i++) {
        // adding the y-coordinates with the side x-coordinate to get the position of the diagonal line
        auto estimatedY_from = interpolation(sideV0ToExtra[i], triangle.v0().x, extraPoint.x, triangle.v0().depth, extraPoint.depth);
        auto estimatedY_to = interpolation(sideV0ToV1[i], triangle.v0().x, triangle.v1().x, triangle.v0().depth, triangle.v1().depth);
        drawLine(CanvasPoint(sideV0ToExtra[i], i + triangle.v0().y, estimatedY_from), CanvasPoint(sideV0ToV1[i], i + triangle.v0().y, estimatedY_to), distance, window, fillColour);
    }

    for (size_t i = 0; i < sideExtraToV2.size(); i++) {
        // adding the y-coordinates with the side x-coordinate to get the position of the diagonal line
        auto estimatedY_from = interpolation(sideExtraToV2[i], extraPoint.x, triangle.v2().x, extraPoint.depth, triangle.v2().depth);
        auto estimatedY_to = interpolation(sideV1ToV2[i], triangle.v1().x, triangle.v2().x, triangle.v1().depth, triangle.v2().depth);
        drawLine(CanvasPoint(sideExtraToV2[i], i + triangle.v1().y, estimatedY_from), CanvasPoint(sideV1ToV2[i], i + triangle.v1().y, estimatedY_to), distance, window, fillColour);
    }

//    drawTriangle(triangle, distance, fillColour, window);
}

// Reading the mtl file and getting the information of colours for the palette
std::map<std::string, Colour> readMTLFile(const std::string& filename) {
    std::ifstream readFile(filename);
    std::string line;
    std::map<std::string, Colour> getPalette;
    std::string key;
    Colour getColour;

    while (!readFile.eof()) {
        std::getline(readFile, line);
        auto tokens = split(line, ' ');

        if (tokens[0] == "newmtl") {
            key = tokens[1];
        }
        else if (tokens[0] == "Kd") {
            getPalette[key] = Colour(std::stof(tokens[1]) * 255, std::stof(tokens[2]) * 255, std::stof(tokens[3]) * 255);
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
    std::vector<glm::vec3> vertices;
    std::vector<ModelTriangle> triangles;
    Colour currentColour;
    std::map<std::string, Colour> palette;

    while (std::getline(readFile, line)) {
        auto tokens = split(line, ' ');
        if (tokens[0] == "v") {
            auto one = std::stof(tokens[1]) * scaleFloat;
            auto two = std::stof(tokens[2]) * scaleFloat;
            auto three = std::stof(tokens[3]) * scaleFloat;
            objVertices.emplace_back(one, two, three);
        }
        else if (tokens[0] == "f") {
            ModelTriangle t1 = ModelTriangle(objVertices[std::stoi(tokens[1]) - 1], objVertices[std::stoi(tokens[2]) - 1], objVertices[std::stoi(tokens[3]) - 1], currentColour);
            triangles.push_back(t1);
        }
        else if (tokens[0] == "usemtl") {
            // call readMTLFile() to access the palette
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

void lookAt() {
    // forward
    cameraOrientation[2] = glm::normalize(cameraPosition);
    // up
    cameraOrientation[1] = glm::normalize(glm::cross(cameraOrientation[2], cameraOrientation[0]));
    // right
    cameraOrientation[0] = glm::normalize(glm::cross(glm::vec3(0, 1, 0), cameraOrientation[2]));
}

// Projecting on to the image plane
// initial camera position: (0.0, 0.0, 4.0) -> store in vec3 variable
// vertexPosition: 3D position of a single vertex (passed in as a vec3)
// focal length: distance = 2.0 (constant)
CanvasPoint getCanvasIntersectionPoint(glm::vec3 vertexPosition, float posRange = 1) {
    glm::vec3 newCamVec = (cameraPosition - vertexPosition) * cameraOrientation;

    float u = focalLength * (newCamVec.x / -newCamVec.z);
    float v = focalLength * (newCamVec.y / newCamVec.z);
    CanvasPoint newCamPosition = CanvasPoint(u * posRange + WIDTH / 2, v * posRange + HEIGHT / 2);
    newCamPosition.depth = newCamVec.z;
    return newCamPosition;
}

void wireframe(std::vector<ModelTriangle> modelTriangles, std::vector<std::vector<float>>& distance, DrawingWindow& window) {
    window.clearPixels();
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
}

// won't activate when lookAt() is activated
void respectiveOrientationToX(double degree) {
    double radian = degree * (M_PI / 180.0);
    glm::mat3 matrix = glm::mat3(1, 0, 0,
                                 0, cos(radian), -sin(radian),
                                 0, sin(radian), cos(radian));
    cameraOrientation = matrix * cameraOrientation;
    lookAt();
}

// won't activate when lookAt() is activated
void respectiveOrientationToY(double degree) {
    double radian = degree * (M_PI / 180.0);
    glm::mat3 matrix = glm::mat3(cos(radian), 0, sin(radian),
                                 0, 1, 0,
                                 -sin(radian), 0, cos(radian));
    cameraOrientation = matrix * cameraOrientation;
    lookAt();
}

void camRotation() {
    double radian = 1 * (M_PI / 180.0);
    glm::mat3 rotatingMatrix = glm::mat3(cos(radian), 0, sin(radian),
                                 0, 1, 0,
                                 -sin(radian), 0, cos(radian));
    cameraPosition = cameraPosition * rotatingMatrix;
//    return true;
}

// d = (r - s) / t
// d = SPVector + possibleSolution[0](t) - possibleSolution[1](u) - possibleSolution[2](v)

glm::vec3 getRayDirection(int pixelWidth, int pixelHeight, float focalLength, float posRange) {
    glm::vec3 calcRayDirection;
    calcRayDirection.x = (pixelWidth - (float(WIDTH)/2)) * 1.0 / posRange;
    calcRayDirection.y = (pixelHeight - (float(HEIGHT)/2)) * -1.0 / posRange;
    calcRayDirection.z = - focalLength;

    return cameraOrientation * calcRayDirection;
}

// return details of the closest intersected triangle - calculate the actual position of the intersection point
RayTriangleIntersection getClosestIntersection(glm::vec3& rayDirection, std::vector<ModelTriangle> triangle, glm::vec3 position, int triangleIndex = -1) {
    RayTriangleIntersection closestIntersection = RayTriangleIntersection(glm::vec3(0, 0, 0), FLT_MAX, triangle[0], -1);
    float t = FLT_MAX;
    for (size_t i = 0; i < triangle.size(); i++) {
        glm::vec3 e0 = triangle[i].vertices[1] - triangle[i].vertices[0];
        glm::vec3 e1 = triangle[i].vertices[2] - triangle[i].vertices[0];
        glm::vec3 SPVector = position - triangle[i].vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        //[t, u, v]
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        if (possibleSolution[0] < t) {
        if ((possibleSolution[1] >= 0.0) && (possibleSolution[1] <= 1.0)
            && (possibleSolution[2] >= 0.0) && (possibleSolution[2] <= 1.0)
            && (possibleSolution[1] + possibleSolution[2]) <= 1.0) {
            if (possibleSolution[0] > 0 && triangleIndex != i) {
                t = possibleSolution[0];
                // calculate the actual position of the intersection point
                // r = s + t * d
                closestIntersection.intersectionPoint =
                        triangle[i].vertices[0] + possibleSolution[1] * e0 + possibleSolution[2] * e1;
                closestIntersection.intersectedTriangle = triangle[i];
                closestIntersection.triangleIndex = i;
                closestIntersection.distanceFromCamera = possibleSolution[0];
            }
            }
        }
    }
    return closestIntersection;
}

float proximityLighting(glm::vec3 trianglePoint, glm::vec3 normal) {
    // lightPosition - trianglePoint -> right order?
    float brightLength = glm::length(lightPosition - trianglePoint);
    float lightIntensity = 15 / (4 * M_PI * brightLength * brightLength);
    if (lightIntensity > 1) lightIntensity = 1;
    return lightIntensity;
}

// dot product of the normal and light direction vectors
// used to adjust the brightness of each pixel
float calcAngleOfLightIncidence(glm::vec3 trianglePoint, glm::vec3 normal) {
    glm::vec3 lightDirection = glm::normalize(lightPosition - trianglePoint);
    float angleIncidence = std::max(0.0f, glm::dot(normal, lightDirection));
    if (angleIncidence > 1) angleIncidence = 1;

    return angleIncidence;
}



void drawRayTracingScene(std::vector<ModelTriangle> triangle, float posRange, float focalLength, DrawingWindow& window) {
    window.clearPixels();

    for (size_t i = 0; i < HEIGHT; i++) {
        // ask this part
        std::fill(::distance[i].begin(), ::distance[i].end(), INT32_MIN);
    }

    for (size_t h = 0; h < HEIGHT; h++) {
        for (size_t w = 0; w < WIDTH; w++) {
            glm::vec3 rayDirection = glm::normalize(getRayDirection(w, h, focalLength, posRange) - cameraPosition);
            RayTriangleIntersection closestIntersectTriangle = getClosestIntersection(rayDirection, triangle, cameraPosition);

            glm::vec3 lightDirection = glm::normalize(lightPosition - closestIntersectTriangle.intersectionPoint);
            RayTriangleIntersection intersectedLightPoint = getClosestIntersection(lightDirection, triangle, closestIntersectTriangle.intersectionPoint, closestIntersectTriangle.triangleIndex);

            ModelTriangle getTriangle = closestIntersectTriangle.intersectedTriangle;
            glm::vec3 edge_1 = getTriangle.vertices[1] - getTriangle.vertices[0];
            glm::vec3 edge_2 = getTriangle.vertices[2] - getTriangle.vertices[0];
            getTriangle.normal = glm::cross(edge_1, edge_2);

            if (intersectedLightPoint.distanceFromCamera >= glm::distance(lightPosition, closestIntersectTriangle.intersectionPoint) && closestIntersectTriangle.triangleIndex != intersectedLightPoint.triangleIndex) {  // shadow

    //            float angleOfIncidence = glm::dot(glm::normalize(singleLightSourcePosition - intersectPoint.intersectionPoint), triangle1.normal);
                float angleOfIncidence = calcAngleOfLightIncidence(closestIntersectTriangle.intersectionPoint, getTriangle.normal);
                float brightness = proximityLighting(closestIntersectTriangle.intersectionPoint, getTriangle.normal);
                if (brightness < 0) brightness = 0;
                float light = brightness;
                if (light < 0) light = 0;

//                std::cout << "angleOfIncidence: " << angleOfIncidence << std::endl;

                Colour getColour = getTriangle.colour;
                getColour.red *= light;
                getColour.blue *= light;
                getColour.green *= light;

//                std::cout << "light: " << light << std::endl;
//                std::cout << "brightness: " << brightness << std::endl;

                if(closestIntersectTriangle.distanceFromCamera != FLT_MAX) {
                    window.setPixelColour(w, h, colourPacking(getColour));
                }
            } // if

        }
    }
}

void rasterising_draw(DrawingWindow &window) {
	window.clearPixels();
    for (size_t i = 0; i < HEIGHT; i++) {
        std::fill(::distance[i].begin(), ::distance[i].end(), INT32_MIN);
    }
    std::vector<ModelTriangle> obj = readOBJFile("cornell-box.obj", 0.35);
    wireframeColour(obj, ::distance, window);  //    projecting the box
    //camRotation();
    lookAt();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void wireframe_draw(DrawingWindow &window) {
    window.clearPixels();
    for (size_t i = 0; i < HEIGHT; i++) {
        std::fill(::distance[i].begin(), ::distance[i].end(), INT32_MIN);
    }
    std::vector<ModelTriangle> obj = readOBJFile("cornell-box.obj", 0.35);
    wireframe(obj, ::distance, window);
    //camRotation();
    lookAt();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void rayTrace_draw(DrawingWindow &window) {
    window.clearPixels();
    for (size_t i = 0; i < HEIGHT; i++) {
        std::fill(::distance[i].begin(), ::distance[i].end(), INT32_MIN);
    }

    float posRange = 60;
    std::vector<ModelTriangle> obj = readOBJFile("cornell-box.obj", 0.35);
    drawRayTracingScene(obj, posRange, ::focalLength, window);
    //camRotation();
    lookAt();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void handleEvent(SDL_Event event, std::vector<std::vector<float>>& distance, DrawingWindow &window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) cameraPosition[0] = cameraPosition[0] + 0.1;
        else if (event.key.keysym.sym == SDLK_RIGHT) cameraPosition[0] = cameraPosition[0] - 0.1;
        else if (event.key.keysym.sym == SDLK_UP) cameraPosition[1] = cameraPosition[1] - 0.1;
        else if (event.key.keysym.sym == SDLK_DOWN) cameraPosition[1] = cameraPosition[1] + 0.1;
        else if (event.key.keysym.sym == SDLK_z) cameraPosition[2] = cameraPosition[2] + 0.1;
        else if (event.key.keysym.sym == SDLK_a) cameraPosition[2] = cameraPosition[2] - 0.1;
        else if (event.key.keysym.sym == SDLK_x) respectiveOrientationToX(1);
        else if (event.key.keysym.sym == SDLK_s) respectiveOrientationToX(-1);
        else if (event.key.keysym.sym == SDLK_y) respectiveOrientationToY(1);
        else if (event.key.keysym.sym == SDLK_h) respectiveOrientationToY(-1);
        else if (event.key.keysym.sym == SDLK_u) randomTriangle(window, distance);
        else if (event.key.keysym.sym == SDLK_f) fillInTriangle(window, randomVertices(), distance, randomColour());
        else if (event.key.keysym.sym == SDLK_w) ::render = 1;
        else if (event.key.keysym.sym == SDLK_r) ::render = 2;
        else if (event.key.keysym.sym == SDLK_t) ::render = 3;
        else if (event.key.keysym.sym == SDLK_p) lightPosition[0] += 0.1;
        else if (event.key.keysym.sym == SDLK_o) lightPosition[0] -= 0.1;
        else if (event.key.keysym.sym == SDLK_l) lightPosition[1] += 0.1;
        else if (event.key.keysym.sym == SDLK_k) lightPosition[1] -= 0.1;
        else if (event.key.keysym.sym == SDLK_m) lightPosition[2] += 0.1;
        else if (event.key.keysym.sym == SDLK_n) lightPosition[2] -= 0.1;
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
        std::cout << "saving" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

    for (size_t i = 0; i < HEIGHT; i++) {
        ::distance[i] = std::vector<float> (WIDTH);
    }

    std::vector<ModelTriangle> obj = readOBJFile("cornell-box.obj", 0.35);

    std::cout << "done" << std::endl;

    SDL_Event event;

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, ::distance, window);

        switch (render) {
            case 1:
                wireframe_draw(window);
                break;
            case 2:
                rasterising_draw(window);
                break;
            case 3:
                rayTrace_draw(window);
                break;
        }

        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}