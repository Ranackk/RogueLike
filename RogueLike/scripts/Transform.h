#pragma once
#include <vector>

class Transform
{
	// Always correct
	glm::vec3 m_LocalPosition;
	glm::quat m_LocalRotation;
	glm::vec3 m_LocalScale;

	glm::vec3 m_LocalEulerAngles; // Not sure yet if this var will be clean or not

								// Might be dirty as can be affected by outside transforms!
	bool isDirty = true;
	glm::vec3 m_ParentPosition;
	glm::quat m_ParentRotation;
	glm::vec3 m_ParentScale;

	glm::mat4x4 currentMatrix;

	std::vector<Transform*> m_Children;


public:
	void translate(glm::vec3 offset);

	void setDirty();
	glm::mat4x4 getObjectMatrix();
	Transform();
	Transform::Transform(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale);
	~Transform();
	std::string toString() const;
	void updateCurrentMatrix();

	void addChildTransform(Transform* child);

	//////////////////////////////// Getter & Setters //////////////////////////////////////////
	glm::vec3 getLocalPosition() {
		updateCurrentMatrix();
		return m_LocalPosition;
	}

	glm::quat getLocalRotation() {
		updateCurrentMatrix();
		return m_LocalRotation; 
	}

	glm::vec3 getLocalScale() {
		updateCurrentMatrix();
		return m_LocalScale; 
	}

	glm::vec3 getPosition() {
		updateCurrentMatrix();
		return m_ParentPosition + m_LocalPosition;
	}

	glm::quat getRotation() {
		updateCurrentMatrix();
		return m_ParentRotation * m_LocalRotation;
	}
	glm::vec3 getScale() {
		updateCurrentMatrix();
		return m_ParentScale * m_LocalScale;
	}

	void setLocalRotation(const glm::quat val) {
		m_LocalRotation = val;
		m_LocalEulerAngles = glm::eulerAngles(val);

		for (auto it = m_Children.begin(); it < m_Children.end(); ++it) {
			(*it)->setParentRotation(val);
		}
		setDirty();
		updateCurrentMatrix();
	}
	void setLocalPosition(const glm::vec3 val) {
		m_LocalPosition = val;

		for (auto it = m_Children.begin(); it < m_Children.end(); ++it) {
			(*it)->setParentPosition(val);
		}
		setDirty();
		updateCurrentMatrix();
	}
	void setLocalScale(const glm::vec3 val) {
		m_LocalScale = val;

		for (auto it = m_Children.begin(); it < m_Children.end(); ++it) {
			(*it)->setParentScale(val);
		}
		setDirty();
		updateCurrentMatrix();
	}
	void setLocalEulerAngles(const glm::vec3 _angles)
	{
		this->m_LocalEulerAngles = _angles;
		this->m_LocalRotation = glm::quat(m_LocalEulerAngles);

		for (auto it = m_Children.begin(); it < m_Children.end(); ++it) {
			(*it)->setParentRotation(m_LocalRotation);
		}
		setDirty();
		updateCurrentMatrix();
	}

	void setParentPosition(const glm::vec3 val) {
		this->m_ParentPosition = val;
		setDirty();
		updateCurrentMatrix();
	}
	void setParentRotation(const glm::quat val) {
		this->m_ParentRotation = val;
		setDirty();
		updateCurrentMatrix();
	}
	void setParentScale(const glm::vec3 val) {
		this->m_ParentScale = val;
		setDirty();
		updateCurrentMatrix();
	}

};
