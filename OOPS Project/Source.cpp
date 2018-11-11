// OOPS Assignment

/*////////////////////////////
	   Include files   
////////////////////////////*/
#include <iostream>
using namespace std;

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// Load images/textures
#include<SOIL.h>
// Time
#include<time.h>
// Windows
#include<Windows.h>

/*////////////////////////////
       Global Variables
////////////////////////////*/
GLfloat vertices[80000];
GLuint indices[80000];

int countPoints = 0;
int countIndices = 0;
float boxSize;	// BoxSize in terms of OpenGL normalized co-ordiantes
int Xbox = 15, Ybox = 15;
int numMines = 20;

// Window Width and Height in pixels
int windowWidth, windowHeight;

// enum decalration
enum boxState {
	MINE = -1, EMPTY = 0
};

enum clickState {
	UNCLICKED = 0, CLICKED = 1, MINED = 2
};

/*////////////////////////////
      Class Declarations
////////////////////////////*/

class Board;

class Box {
public:
	int x, y;	// Position of the box/cell in the grid
	boxState state;
	int numSurroundingMines;	// Number of mines surrounding the cell
	clickState click;

	Box() {
		state = EMPTY;
		click = UNCLICKED;
		numSurroundingMines = 0;
	}

	void processClick(); // Left Click
	void processClick(int cell); // Right Click
};

class Board {
public:
	int xsize, ysize;	// Size of the grid
	Box *grid;			// 2D grid/array of cells, but in a single pointer
	int GameEnd = 0;
	void initializeVertices();
	void initializeBoard();
	int endGame();
	void calcXY(int, int, int&, int&);
}board;

void Box::processClick() {
	// Don't do anything if the box is already clicked
	if (click == CLICKED) return;

	click = CLICKED;

	// If box contains a mine, end the game
	//if (state == MINE) {
	//	board.GameEnd = 1;
	//	// Open all the boxes
	//	for (int i = 0; i < board.xsize * board.ysize; i++) {
	//		board.grid[i].click = CLICKED;
	//		return;
	//	}
	//}
	// Disable clicks
	// DO something here

	int i = y * board.xsize + x;

	// Open all other empty boxes around, if this box is empty without numbers
	if (state == EMPTY && numSurroundingMines == 0) {
		// Left
		if (x > 0) {	// Check array bounds
			if (board.grid[i - 1].state == EMPTY && numSurroundingMines == 0) board.grid[i - 1].processClick();
		}
		// Right
		if (x < board.xsize - 1) {
			if (board.grid[i + 1].state == EMPTY && numSurroundingMines == 0) board.grid[i + 1].processClick();
		}
		// Top
		if (y > 0) {
			if (board.grid[i - board.xsize].state == EMPTY && numSurroundingMines == 0) board.grid[i - board.xsize].processClick();
		}
		// Bottom
		if (y < board.xsize - 1) {
			if (board.grid[i + board.xsize].state == EMPTY && numSurroundingMines == 0) board.grid[i + board.xsize].processClick();
		}
		// Top Left
		if (y > 0 && x > 0) {
			if (board.grid[i - board.xsize - 1].state == EMPTY && numSurroundingMines == 0) board.grid[i - board.xsize - 1].processClick();
		}
		// Top Right
		if (y > 0 && x < board.xsize - 1) {
			if (board.grid[i - board.xsize + 1].state == EMPTY && numSurroundingMines == 0) board.grid[i - board.xsize + 1].processClick();
		}
		// Bottom Left
		if (y < board.xsize - 1 && x > 0) {
			if (board.grid[i + board.xsize - 1].state == EMPTY && numSurroundingMines == 0) board.grid[i + board.xsize - 1].processClick();
		}
		// Bottom Right
		if (y < board.xsize - 1 && x < board.xsize - 1) {
			if (board.grid[i + board.xsize + 1].state == EMPTY && numSurroundingMines == 0) board.grid[i + board.xsize + 1].processClick();
		}
	}
}

void Box::processClick(int dummy) {
	// Mark square as mined if it has not already been clicked, alternate between mined and unclicked states
	if (click == UNCLICKED) {
		click = MINED;
	}
	else if (click == MINED) {
		click = UNCLICKED;
	}
}

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
void calcXY(int, int, int&, int&);
GLFWwindow* initWindow();

// Shaders
const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec3 color;\n"
"layout(location = 2) in vec2 texCoord;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0f);\n"
"ourColor = color;\n"
"TexCoord = texCoord;\n"
"}\n\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\n\0";

const GLchar* fragmentShaderSourceGreen =
"#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(0.3f, 0.3f, 0.3f, 1.0f);\n"
"}\n\0";

const GLchar* fragmentShaderSourceYellow = 
"#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(0.6f, 0.6f, 0.6f, 1.0f);\n"
"}\n\0";

const GLchar* fragmentShaderSourceWhite = 
"#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(0.9f, 0.9f, 0.9f, 1.0f);\n"
"}\n\0";

const GLchar* fragmentShaderSourceImage =
"#version 330 core\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"out vec4 color;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"color = texture(ourTexture, TexCoord) * vec4(0.6, 0.6, 0.6, 1.0f);\n"
"}\n\0";

const GLchar* fragmentShaderSourceMine =
"#version 330 core\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"out vec4 color;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"color = texture(ourTexture, TexCoord) * vec4(0.8, 0.7, 0.7, 1.0f);\n"
"}\n\0";

// Initialize vertices array for OpenGL
void Board::initializeVertices() {


	float startX = -0.9;
	float startY = +0.9;

	boxSize = 1.8 / Ybox;


	for (int j = 0; j < Xbox; j++) {
		for (int i = 0; i < Ybox; i++) {

			//////////////////
			/* Outer Square */
			//////////////////

			// Top left point

			// X coordinate
			vertices[countPoints++] = startX + (i * boxSize);
			// Y coordinate
			vertices[countPoints++] = startY - (j * boxSize);
			// Z coordinate
			vertices[countPoints++] = 0;
			// Colours - Red
			vertices[countPoints++] = 0;
			// Colours - Green
			vertices[countPoints++] = 0;
			// Colours - Blue
			vertices[countPoints++] = 0;
			// Texture coordinates
			vertices[countPoints++] = 0;
			vertices[countPoints++] = 1;

			// Top right point

			// X coordinate
			vertices[countPoints++] = startX + ((i + 1) * boxSize);
			// Y coordinate
			vertices[countPoints++] = startY - (j * boxSize);
			// Z coordinate
			vertices[countPoints++] = 0;
			// Colours - Red
			vertices[countPoints++] = 0;
			// Colours - Green
			vertices[countPoints++] = 0;
			// Colours - Blue
			vertices[countPoints++] = 0;
			// Texture coordinates
			vertices[countPoints++] = 1;
			vertices[countPoints++] = 1;

			// Bottom right point

			// X coordinate
			vertices[countPoints++] = startX + ((i + 1) * boxSize);
			// Y coordinate
			vertices[countPoints++] = startY - ((j + 1) * boxSize);
			// Z coordinate
			vertices[countPoints++] = 0;
			// Colours - Red
			vertices[countPoints++] = 0;
			// Colours - Green
			vertices[countPoints++] = 0;
			// Colours - Blue
			vertices[countPoints++] = 0;
			// Texture coordinates
			vertices[countPoints++] = 1;
			vertices[countPoints++] = 0;

			// Bottom left point

			// X coordinate
			vertices[countPoints++] = startX + (i  * boxSize);
			// Y coordinate
			vertices[countPoints++] = startY - ((j + 1) * boxSize);
			// Z coordinate
			vertices[countPoints++] = 0;
			// Colours - Red
			vertices[countPoints++] = 0;
			// Colours - Green
			vertices[countPoints++] = 0;
			// Colours - Blue
			vertices[countPoints++] = 0;
			// Texture coordinates
			vertices[countPoints++] = 0;
			vertices[countPoints++] = 0;


			//////////////////
			/* Inner Square */
			//////////////////

			// Top left point

			float borderSize = 0.02;

			// X coordinate
			vertices[countPoints++] = startX + (i * boxSize) + (borderSize * boxSize);
			// Y coordinate
			vertices[countPoints++] = startY - (j * boxSize) - (borderSize * boxSize);
			// Z coordinate
			vertices[countPoints++] = 0;
			// Colours - Red
			vertices[countPoints++] = 0.8;
			// Colours - Green
			vertices[countPoints++] = 0.8;
			// Colours - Blue
			vertices[countPoints++] = 0;
			// Texture coordinates
			vertices[countPoints++] = 0;
			vertices[countPoints++] = 1;

			// Top right point

			// X coordinate
			vertices[countPoints++] = startX + ((i + 1) * boxSize) - (borderSize * boxSize);
			// Y coordinate
			vertices[countPoints++] = startY - (j * boxSize) - (borderSize * boxSize);
			// Z coordinate
			vertices[countPoints++] = 0;
			// Colours - Red
			vertices[countPoints++] = 0.8;
			// Colours - Green
			vertices[countPoints++] = 0.8;
			// Colours - Blue
			vertices[countPoints++] = 0;
			// Texture coordinates
			vertices[countPoints++] = 1;
			vertices[countPoints++] = 1;

			// Bottom right point

			// X coordinate
			vertices[countPoints++] = startX + ((i + 1) * boxSize) - (borderSize * boxSize);
			// Y coordinate
			vertices[countPoints++] = startY - ((j + 1) * boxSize) + (borderSize * boxSize);
			// Z coordinate
			vertices[countPoints++] = 0;
			// Colours - Red
			vertices[countPoints++] = 0.8;
			// Colours - Green
			vertices[countPoints++] = 0.8;
			// Colours - Blue
			vertices[countPoints++] = 0;
			// Texture coordinates
			vertices[countPoints++] = 1;
			vertices[countPoints++] = 0;

			// Bottom left point

			// X coordinate
			vertices[countPoints++] = startX + (i  * boxSize) + (borderSize * boxSize);
			// Y coordinate
			vertices[countPoints++] = startY - ((j + 1) * boxSize) + (borderSize * boxSize);
			// Z coordinate
			vertices[countPoints++] = 0;
			// Colours - Red
			vertices[countPoints++] = 0.8;
			// Colours - Green
			vertices[countPoints++] = 0.8;
			// Colours - Blue
			vertices[countPoints++] = 0;
			// Texture coordinates
			vertices[countPoints++] = 0;
			vertices[countPoints++] = 0;


			// Indices for drawing
			indices[countIndices++] = 0 + (countPoints / 8) - 8;
			indices[countIndices++] = 1 + (countPoints / 8) - 8;
			indices[countIndices++] = 2 + (countPoints / 8) - 8;
			indices[countIndices++] = 2 + (countPoints / 8) - 8;
			indices[countIndices++] = 3 + (countPoints / 8) - 8;
			indices[countIndices++] = 0 + (countPoints / 8) - 8;
			// Inner square
			indices[countIndices++] = 4 + (countPoints / 8) - 8;
			indices[countIndices++] = 5 + (countPoints / 8) - 8;
			indices[countIndices++] = 6 + (countPoints / 8) - 8;
			indices[countIndices++] = 6 + (countPoints / 8) - 8;
			indices[countIndices++] = 7 + (countPoints / 8) - 8;
			indices[countIndices++] = 4 + (countPoints / 8) - 8;
		}
	}
	//for (int i = 0; i < 12; i++) cout << indices[i] << endl;
};

// calcXY function
void Board::calcXY(int mousex, int mousey, int& gridx, int& gridy) {
	int boxSizePixels = (windowHeight / 2) * boxSize;

	// Subtract blank space to the left and right
	mousex -= (windowWidth - (Xbox * boxSizePixels)) / 2;
	mousey -= (windowHeight - (Ybox * boxSizePixels)) / 2;
	// Divide by size of every box
	mousex += boxSizePixels;
	mousey += boxSizePixels;
	mousex /= boxSizePixels;
	mousey /= boxSizePixels;
	// Check validity of click
	// cout << endl << mousex << " " << mousey;
	if (mousex >= 1 && mousex <= Xbox && mousey >= 1 && mousey <= Ybox) {
		gridx = mousex - 1;
		gridy = mousey - 1;
		return;
	}
	gridx = gridy = -1;
}

// Allocate dynamic memory for cells and initialize the board
void Board::initializeBoard() {
	// Set board size
	board.xsize = Xbox;
	board.ysize = Ybox;

	// Allocate memory
	board.grid = new Box[(board.xsize) * (board.ysize)];

	// Set X & Y of each box
	for (int i = 0; i < board.xsize; i++) {
		for (int j = 0; j < board.ysize; j++) {
			board.grid[j * board.xsize + i].x = i;
			board.grid[j * board.xsize + i].y = j;
		}
	}

	srand(time(NULL));
	// Plant mines randomly on the grid!
	for (int i = 0; i < numMines; ) {

		int randomNumber = rand() % (board.xsize * board.ysize);
		if (board.grid[randomNumber].state == EMPTY) {
			board.grid[randomNumber].state = MINE;
			i++;
		}
	}

	// Calculate the number of mines around each empty box
	for (int i = 0; i < board.xsize * board.ysize; i++) {
		// Skip if the box has a mine
		if (board.grid[i].state == MINE) continue;

		int count = 0;

		int column = i % board.xsize;
		int row = i / board.xsize;

		// Left
		if (column > 0) {	// Check array bounds
			if (board.grid[i - 1].state == MINE) count++;
		}
		// Right
		if (column < board.xsize - 1) {
			if (board.grid[i + 1].state == MINE) count++;
		}
		// Top
		if (row > 0) {
			if (board.grid[i - board.xsize].state == MINE) count++;
		}
		// Bottom
		if (row < board.xsize - 1) {
			if (board.grid[i + board.xsize].state == MINE) count++;
		}
		// Top Left
		if (row > 0 && column > 0) {
			if (board.grid[i - board.xsize - 1].state == MINE) count++;
		}
		// Top Right
		if (row > 0 && column < board.xsize - 1) {
			if (board.grid[i - board.xsize + 1].state == MINE) count++;
		}
		// Bottom Left
		if (row < board.xsize - 1 && column > 0) {
			if (board.grid[i + board.xsize - 1].state == MINE) count++;
		}
		// Bottom Right
		if (row < board.xsize - 1 && column < board.xsize - 1) {
			if (board.grid[i + board.xsize + 1].state == MINE) count++;
		}

		board.grid[i].numSurroundingMines = count;
		//cout << endl << row << " " << column << ": " << count;
	}
}

// Detects if game has ended
int Board::endGame() {
	for (int i = 0; i < board.xsize * board.ysize; i++) {
		if (board.grid[i].state == EMPTY && board.grid[i].click == UNCLICKED) {
			return 0;
		}
	}
	return 1;
}

// The MAIN function, from here we start the application and run the game loop
int main() {
	GLFWwindow* window = initWindow();

	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint fragmentShaderGreen = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint fragmentShaderWhite = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint fragmentShaderImage = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint fragmentShaderMine = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glShaderSource(fragmentShaderGreen, 1, &fragmentShaderSourceGreen, NULL);
	glShaderSource(fragmentShaderYellow, 1, &fragmentShaderSourceYellow, NULL);
	glShaderSource(fragmentShaderWhite, 1, &fragmentShaderSourceWhite, NULL);
	glShaderSource(fragmentShaderImage, 1, &fragmentShaderSourceImage, NULL);
	glShaderSource(fragmentShaderMine, 1, &fragmentShaderSourceMine, NULL);
	glCompileShader(fragmentShader);
	glCompileShader(fragmentShaderGreen);
	glCompileShader(fragmentShaderYellow);
	glCompileShader(fragmentShaderWhite);
	glCompileShader(fragmentShaderImage);
	glCompileShader(fragmentShaderMine);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	glGetShaderiv(fragmentShaderGreen, GL_COMPILE_STATUS, &success);
	glGetShaderiv(fragmentShaderYellow, GL_COMPILE_STATUS, &success);
	glGetShaderiv(fragmentShaderWhite, GL_COMPILE_STATUS, &success);
	glGetShaderiv(fragmentShaderImage, GL_COMPILE_STATUS, &success);
	glGetShaderiv(fragmentShaderMine, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	GLuint shaderProgramGreen = glCreateProgram();
	GLuint shaderProgramYellow = glCreateProgram();
	GLuint shaderProgramWhite = glCreateProgram();
	GLuint shaderProgramImage = glCreateProgram();
	GLuint shaderProgramMine = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glAttachShader(shaderProgramGreen, vertexShader);
	glAttachShader(shaderProgramGreen, fragmentShaderGreen);
	glLinkProgram(shaderProgramGreen);
	glAttachShader(shaderProgramYellow, vertexShader);
	glAttachShader(shaderProgramYellow, fragmentShaderYellow);
	glLinkProgram(shaderProgramYellow);
	glAttachShader(shaderProgramWhite, vertexShader);
	glAttachShader(shaderProgramWhite, fragmentShaderWhite);
	glLinkProgram(shaderProgramWhite);
	glAttachShader(shaderProgramImage, vertexShader);
	glAttachShader(shaderProgramImage, fragmentShaderImage);
	glLinkProgram(shaderProgramImage);
	glAttachShader(shaderProgramMine, vertexShader);
	glAttachShader(shaderProgramMine, fragmentShaderMine);
	glLinkProgram(shaderProgramMine);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(fragmentShaderGreen);
	glDeleteShader(fragmentShaderYellow);
	glDeleteShader(fragmentShaderWhite);
	glDeleteShader(fragmentShaderImage);
	glDeleteShader(fragmentShaderMine);

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	// Set Up vertex data
	board.initializeVertices();

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registeGreen VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	// Load and create a texture 
	GLuint texture[10];
	GLuint textureFlag;
	GLuint textureMine;

	// Load all 9 textures from 1 to 9
	for (int i = 1; i <= 9; i++) {
		glGenTextures(1, &texture[i]);
		glBindTexture(GL_TEXTURE_2D, texture[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
		// Set our texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load, create texture and generate mipmaps
		int width, height;
		char fileName[12] = "./img/0.jpg";
		fileName[6] = i + 48;	// Integer to character
		unsigned char* image = SOIL_load_image(fileName, &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	}

    glGenTextures(1, &textureFlag);
    glBindTexture(GL_TEXTURE_2D, textureFlag); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("./img/Flag.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    

    glGenTextures(1, &textureMine);
    glBindTexture(GL_TEXTURE_2D, textureMine);
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    image = SOIL_load_image("./img/Mine.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);


	// Initialize the board
	board.initializeBoard();

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if game has been completed
		if (board.GameEnd == 0) {
			if (board.endGame() == 1) {
				for (int i = 0; i < board.xsize * board.ysize; i++) {
					board.grid[i].click = CLICKED;
				}
				board.GameEnd = 1;
				MessageBox(NULL, "Congratulations! You win!", "Minesweeper", NULL);
			}
		}

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Bind Textures using texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[5]);
		glUniform1i(glGetUniformLocation(shaderProgramImage, "ourTexture1"), 0);

		
		glBindVertexArray(VAO);
		glUseProgram(shaderProgramWhite);
		for (int i = 0, j = 0; i < countIndices && j < board.xsize * board.ysize; i += 12, j++) {
			glUseProgram(shaderProgramWhite);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * sizeof(GLuint)));

			if (board.grid[j].click == UNCLICKED) {
				//if (board.grid[j].state != MINE) {
				glUseProgram(shaderProgramGreen);
				//}
				//else {
				//glUseProgram(shaderProgramWhite);
				//}
			}
			else {
				// Game not over
				if (board.GameEnd == 0) {
					if (board.grid[j].click == MINED) {
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, textureFlag);
						glUniform1i(glGetUniformLocation(shaderProgramImage, "ourTexture1"), 0);
						glUseProgram(shaderProgramImage);
					}
					else if (board.grid[j].numSurroundingMines > 0) {
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, texture[board.grid[j].numSurroundingMines]);
						glUniform1i(glGetUniformLocation(shaderProgramImage, "ourTexture1"), 0);
						glUseProgram(shaderProgramImage);
					}
					else glUseProgram(shaderProgramYellow);
				}
				// Game over
				else {
					if (board.grid[j].state == MINE) {
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, textureMine);
						glUniform1i(glGetUniformLocation(shaderProgramMine, "ourTexture1"), 0);
						glUseProgram(shaderProgramMine);
					}
					else if (board.grid[j].numSurroundingMines > 0) {
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, texture[board.grid[j].numSurroundingMines]);
						glUniform1i(glGetUniformLocation(shaderProgramImage, "ourTexture1"), 0);
						glUseProgram(shaderProgramImage);
					}
					else glUseProgram(shaderProgramYellow);
				}
			}
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((i + 6) * sizeof(GLuint)));
			
		}

		// Unbind vertex array
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_ENTER) {
		for (int i = 0; i < board.xsize * board.ysize; i++) {
			board.grid[i].click = CLICKED;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode) {
	double mousex = 0, mousey = 0;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &mousex, &mousey);

		// Calculate x, y coordinates of the grid/board
		int gridx = 0, gridy = 0;
		board.calcXY(mousex, mousey, gridx, gridy);

		if (gridx == -1 || gridy == -1) return;

		board.grid[gridy * board.xsize + gridx].processClick(gridx);
		
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &mousex, &mousey);

		// Calculate x, y coordinates of the grid/board
		int gridx = 0, gridy = 0;
		board.calcXY(mousex, mousey, gridx, gridy);
		//cout << endl << gridx << endl << gridy << endl;

		if (gridx == -1 || gridy == -1) return;
		
		// If a mine is clicked, display all boxes and end the game
		if (board.grid[gridy * board.xsize + gridx].state == MINE) {
			for (int i = 0; i < board.xsize * board.ysize; i++) {
				board.grid[i].click = CLICKED;
			}
			board.GameEnd = 1;
			MessageBox(NULL, "Game Over!", "Minesweeper", NULL);
		}

		// Process mouseclick at the clicked box
		board.grid[gridy * board.xsize + gridx].processClick();
	}
}

GLFWwindow* initWindow() {
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the requiGreen options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OOPS Project", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	//cout << windowWidth << endl << windowHeight;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, windowWidth, windowHeight, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport((windowWidth - windowHeight) / 2, 0, windowHeight, windowHeight);

	return window;
}