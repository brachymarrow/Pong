#include "Entity.h"

Entity::Entity()
{
	position = glm::vec3(0);
	speed = 0;
	movement = glm::vec3(0);
	width = 0.25;
	height = 1.5;
}

void Entity::Update(float deltaTime)
{
	position += movement * speed * deltaTime;
}

void Entity::Render(ShaderProgram* program) {
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	program->SetModelMatrix(modelMatrix);

	float vertices[] = { -0.125, -0.75, 0.125, -0.75, 0.125, 0.75, 0.125, 0.75, -0.125, 0.75, -0.125, -0.75 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::MoveUp(){
	if (position.y < 3.625f) {
		movement = glm::vec3(0.0f, 1.0f, 0.0f);
	}
}

void Entity::MoveDown() {
	if (position.y > -3.625f) {
		movement = glm::vec3(0.0f, -1.0f, 0.0f);
	}
}

void Entity::Stop() {
	movement = glm::vec3(0);
}

