#include "stdafx.h"
#include "Transform.h"
#include "Utility.h"

#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>

using namespace glm;

//Transform Transform::origin;

void Transform::updateCurrentMatrix()
{
	if (isDirty) {
		const mat4x4 scaleMatrix = Utility::scaleMatrix(m_ParentScale * m_LocalScale);
		const mat4x4 rotMatrix = glm::mat4_cast(m_ParentRotation) * glm::mat4_cast(m_LocalRotation);
		const mat4x4 posMatrix = Utility::translationMatrix(m_ParentPosition + m_LocalPosition);

		this->currentMatrix = posMatrix * rotMatrix * scaleMatrix;

		isDirty = false;
	}
}


void Transform::addChildTransform(Transform* child) {
	m_Children.push_back(child);
	child->setParentPosition(m_ParentPosition + m_LocalPosition);
	child->setParentRotation(m_ParentRotation * m_LocalRotation);
	child->setParentScale(m_ParentScale * m_LocalScale);
}

void Transform::translate(const glm::vec3 offset) {
	setLocalPosition(m_LocalPosition + offset);
}

void Transform::setDirty()
{
	this->isDirty = true;
}

glm::mat4x4 Transform::getObjectMatrix()
{
	updateCurrentMatrix();
	return this->currentMatrix;
}

Transform::Transform()
{
	this->m_LocalPosition = vec3(0, 0, 0);
	this->m_LocalEulerAngles = vec3(0, 0, 0);
	this->m_LocalRotation = quat(m_LocalEulerAngles);
	this->m_LocalScale = vec3(1, 1, 1);

	this->m_ParentPosition = vec3(0, 0, 0);
	this->m_ParentRotation = quat();
	this->m_ParentScale = vec3(1, 1, 1);

	this->m_Children = std::vector<Transform*>();

	isDirty = true;
	updateCurrentMatrix();
}

Transform::Transform(glm::vec3 _position, glm::vec3 _eulerAngles, glm::vec3 _scale)
{
	this->m_LocalPosition = _position;
	this->m_LocalEulerAngles = _eulerAngles;
	this->m_LocalRotation = quat(m_LocalEulerAngles);

	this->m_ParentPosition = vec3(0, 0, 0);
	this->m_ParentRotation = quat();
	this->m_ParentScale = vec3(1, 1, 1);

	this->m_LocalScale = _scale;
	this->m_Children = std::vector<Transform*>();

	isDirty = true;
	updateCurrentMatrix();
}

Transform::~Transform()
{

}

std::string Transform::toString() const {
	std::string ret = std::string("Transform[");
	ret.append(glm::to_string(m_LocalPosition));
	ret.append(", ");
	ret.append(glm::to_string(m_LocalEulerAngles));
	ret.append(", ");
	ret.append(glm::to_string(m_LocalScale));
	ret.append("]");

	return ret;
}
