#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "Entity.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

float lastTicks = 0;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

class Ball : public Entity {
public:
	glm::vec3 position;
	glm::vec3 movement;
	float width = 0.5;
	float height = 0.5;
	float speed;
	float xmove;
	float ymove;

	Ball() {
		position = glm::vec3(0);
		movement = glm::vec3(0);
		speed = 1.0;
		xmove = 2.0;
		ymove = 3.0;
	}

	void Render(ShaderProgram* program) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
		program->SetModelMatrix(modelMatrix);

		float vertices[] = { -0.125, -0.125, 0.125, -0.125, 0.125, 0.125, 0.125, 0.125, -0.125, 0.125, -0.125, -0.125 };
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}

	bool AreColliding(const Entity& entity) {
		float xdiff = fabs(entity.position.x - position.x);
		float ydiff = fabs(entity.position.y - position.y);

		return (xdiff - ((width + entity.width) / 2.0f)) < 0 &&
			(ydiff - ((height + entity.height) / 2.0f)) < 0;

	}

	void Move(const Entity& player1, const Entity& player2) {
		movement = glm::vec3(0);
		if (-4.875 >= position.x || position.x >= 4.875f) {
			movement = glm::vec3(0);
			return;
		}
		else if (AreColliding(player1) || AreColliding(player2)) {
			xmove *= -1;
		}
		movement += glm::vec3(xmove, 0.0f, 0.0f);
		if (position.y >= 3.625f || position.y <= -3.625f) {
			ymove *= -1;
		}
		movement += glm::vec3(0.0f, ymove, 0.0f);
	}

	void Update(float deltaTime) {
		position += movement * speed * deltaTime;
	}
};

Entity player1;
Entity player2;
Ball ball;

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	SDL_Event event;

#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 480);
	program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	player1.speed = 2.5;
	player2.speed = 2.5;
	ball.speed = 1.0;

	player1.position = glm::vec3(-4.875f, 0.0f, 1.0f);
	player2.position = glm::vec3(4.875f, 0.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_w:
				player1.Stop();
			case SDLK_UP:
				player2.Stop();
			case SDLK_s:
				player1.Stop();
			case SDLK_DOWN:
				player2.Stop();
			}
		}
	
	}
	//where we'll see if the players are holding down the keys
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_W]) {
		player1.MoveUp();
	}
	else if (keys[SDL_SCANCODE_S]) {
		player1.MoveDown();
	}
	if (keys[SDL_SCANCODE_UP]) {
		player2.MoveUp();
	}
	else if (keys[SDL_SCANCODE_DOWN]) {
		player2.MoveDown();
	}
}

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;
	player1.Update(deltaTime);
	player2.Update(deltaTime);
	ball.Move(player1, player2);
	ball.Update(deltaTime);
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	//render the paddles and ball
	player1.Render(&program);
	player2.Render(&program);
	ball.Render(&program);
	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}	