#ifndef TRANSFORM_INCLUDED_H
#define TRANSFORM_INCLUDED_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct transform
{
	enum // Indexes
	{
		RightX = 0, UpX = 1, ForwardX = 2, TransX = 3,
		RightY = 4, UpY = 5, ForwardY = 6, TransY = 7,
		RightZ = 8, UpZ = 9, ForwardZ = 10, TransZ = 11,
		W = 12, W1 = 13, W2 = 14, W3 = 15
	};

	transform() {
		matData = glm::mat4();
		setData(matData);
	}
	explicit transform(const glm::mat4& mat) {
		matData = mat;
		setData(matData);
	}
	explicit transform(const glm::quat& rotation) {
		matData = glm::mat4_cast(rotation);
		setData(matData);
	}
	explicit transform(const glm::mat3& rotation) {
		transform();
		matData *= glm::mat4(rotation);
		setData(matData);
	}
	transform(const glm::quat& rotation, const glm::vec3& translation) {
		matData = glm::translate(matData, translation);
		matData *= glm::mat4_cast(rotation);
		setData(matData);
	}
	transform(const glm::mat3& rotation, const glm::vec3& translation) {
		matData = glm::translate(matData, translation);
		matData *= glm::mat4(rotation);
		setData(matData);
	}
	transform(const glm::quat& rotation, const glm::vec3& translation, const glm::vec3& scale) {
		matData = glm::translate(matData, translation);
		matData *= glm::mat4_cast(rotation);
		matData *= glm::scale(matData, scale);
		setData(matData);
	}
	transform(const glm::mat3& rotation, const glm::vec3& translation, const glm::vec3& scale) {
		matData = glm::translate(matData, translation);
		matData *= glm::mat4(rotation);
		matData *= glm::scale(matData, scale);
		setData(matData);
	}
	transform(const glm::vec3& right, const glm::vec3& up, const glm::vec3& forward, const glm::vec4& translation) {
		transform();
		SetRight(right);
		SetUp(up);
		SetForward(forward);
		SetTranslation(glm::vec3(translation.x, translation.y, translation.z));
	}

	transform(float f0, float f1, float f2, float f3,
		float f4, float f5, float f6, float f7,
		float f8, float f9, float f10, float f11,
		float f12, float f13, float f14, float f15) {

		data[0] = f0;
		data[1] = f1;
		data[2] = f2;
		data[3] = f3;
		data[4] = f4;
		data[5] = f5;
		data[6] = f6;
		data[7] = f7;
		data[8] = f8;
		data[9] = f9;
		data[10] = f10;
		data[11] = f11;
		data[12] = f12;
		data[13] = f13;
		data[14] = f14;
		data[15] = f15;
		setMat(data);
	}

	bool operator==(const transform& m) const {
		if (matData == m.matData)
			return true;
		else
			return false;
	}
	bool operator!=(const transform& m) const {
		if (matData != m.matData)
			return true;
		else
			return false;
	}

	transform& operator*=(const transform& m) {
		matData *= m.matData;
		setData(matData);
		return *this;
	}
	transform& operator*=(float scalar) {
		matData *= scalar;
		setData(matData);
		return *this;
	}
	transform& operator/=(float divisor) {
		matData /= divisor;
		setData(matData);
		return *this;
	}
	transform& operator+=(const transform& m) {
		matData += m.matData;
		setData(matData);
		return *this;
	}
	transform& operator-=(const transform& m) {
		matData -= m.matData;
		setData(matData);
		return *this;
	}

	transform operator*(const transform& m) const {
		glm::mat4 tempM = matData * m.matData;
		return transform(tempM);
	}
	transform operator+(const transform& m) const {
		glm::mat4 tempM = matData + m.matData;
		return transform(tempM);
	}
	transform operator-(const transform& m) const {
		glm::mat4 tempM = matData - m.matData;
		return transform(tempM);
	}
	//transform operator*(const transform& m) const;
	transform operator*(float scalar) const {
		glm::mat4 tempM = matData * scalar;
		return transform(tempM);
	}
	transform operator/(float divisor) const {
		glm::mat4 tempM = matData / divisor;
		return transform(tempM);
	}
	transform operator-() const {
		glm::mat4 tempM = matData * -1.0f;
		return transform(tempM);
	}

	float operator[](unsigned index) const {
		return data[index];
	}
	float& operator[](unsigned index) {
		return data[index];
	}

	void Transpose() {
		matData = glm::transpose(matData);
		setData(matData);
	}
	void Inverse() {
		matData = glm::inverse(matData);
		setData(matData);
	}
	// Computes the inverse assuming standard homogeneous matrix format.
	// [ R     T ]
	// [ 0 0 0 1 ]
	void FastInverse() {
		matData = glm::inverse(matData);
		setData(matData);
	}
	transform GetTranspose() const {
		glm::mat4 temp = glm::transpose(matData);
		return(transform(temp));
	}
	transform GetInverse() const {
		glm::mat4 temp = glm::inverse(matData);
		return(transform(temp));
	}
	// Computes the inverse assuming standard homogeneous matrix format.
	// [ R     T ]
	// [ 0 0 0 1 ]
	transform GetFastInverse() const {
		glm::mat4 temp = glm::inverse(matData);
		return(transform(temp));
	}

	void Scale(const glm::vec3& scale) {
		matData = glm::scale(matData, scale);
		setData(matData);
	}
	void Scale(float scale) {
		matData = glm::scale(matData, glm::vec3(scale, scale, scale));
		setData(matData);
	}

	void Rotate(const glm::vec3& axis, float degrees) {
		matData = glm::rotate(matData, degrees, axis);
		setData(matData);
	}
	void RotateX(float degrees){
		matData = glm::rotate(matData, degrees, glm::vec3(1, 0, 0));
		setData(matData);
	}
	void RotateY(float degrees){
		matData = glm::rotate(matData, degrees, glm::vec3(0, 1, 0));
		setData(matData);
	}
	void RotateZ(float degrees) {
		matData = glm::rotate(matData, degrees, glm::vec3(0, 0, 1));
		setData(matData);
	}

	void Translate(const glm::vec3& translation) {
		data[TransX] += translation.x;
		data[TransY] += translation.y;
		data[TransZ] += translation.z;
		setMat(data);
	}

	void SetRight(const glm::vec3& v) {
		data[RightX] = v.x;
		data[RightY] = v.y;
		data[RightZ] = v.z;
		setMat(data);
	}
	void SetUp(const glm::vec3& v) {
		data[UpX] = v.x;
		data[UpY] = v.y;
		data[UpZ] = v.z;
		setMat(data);
	}
	void SetForward(const glm::vec3& v) {
		data[ForwardX] = v.x;
		data[ForwardY] = v.y;
		data[ForwardZ] = v.z;
		setMat(data);
	}
	void SetTranslation(const glm::vec3& v) {
		data[TransX] = v.x;
		data[TransY] = v.y;
		data[TransZ] = v.z;
		setMat(data);
	}

	glm::vec3 GetRight() const {
		return glm::vec3(data[RightX], data[RightY], data[RightZ]);
	}
	glm::vec3 GetUp() const {
		return glm::vec3(data[UpX], data[UpY], data[UpZ]);
	}
	glm::vec3 GetForward() const {
		return glm::vec3(data[ForwardX], data[ForwardY], data[ForwardZ]);
	}
	glm::vec3 GetTranslation() const {
		return glm::vec3(data[TransX], data[TransY], data[TransZ]);
	}

	static const transform Identity;

	static transform PerspectiveProjection(float fovyDegrees, float aspect, float zNear, float zFar) {
		return transform(glm::perspective(fovyDegrees, aspect, zNear, zFar));
	}
	static transform InversePerspectiveProjection(float fovyDegrees, float aspect, float zNear, float zFar) {
		return transform(glm::inverse(glm::perspective(fovyDegrees, aspect, zNear, zFar)));
	}

	static transform OrthographicProjection(float left, float right, float top, float bottom, float zNear, float zFar) {
		return transform(glm::ortho(left, right, top, bottom, zNear, zFar));
	}
	static transform InverseOrthographicProjection(float left, float right, float top, float bottom, float zNear, float zFar) {
		return transform(glm::inverse(glm::ortho(left, right, top, bottom, zNear, zFar)));
	}

	static transform LookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& upVector) {
		return transform(glm::lookAt(position, target, upVector));
	}

	void setData(glm::mat4 mat) {
		data[0] = mat[0][0];
		data[1] = mat[1][0];
		data[2] = mat[2][0];
		data[3] = mat[3][0];
		data[4] = mat[0][1];
		data[5] = mat[1][1];
		data[6] = mat[2][1];
		data[7] = mat[3][1];
		data[8] = mat[0][2];
		data[9] = mat[1][2];
		data[10] = mat[2][2];
		data[11] = mat[2][2];
		data[12] = mat[0][3];
		data[13] = mat[1][3];
		data[14] = mat[2][3];
		data[15] = mat[3][3];
	}

	void setMat(float dat[16]) {
		matData[0][0] = dat[0];
		matData[1][0] = dat[1];
		matData[2][0] = dat[2];
		matData[3][0] = dat[3];
		matData[0][1] = dat[4];
		matData[1][1] = dat[5];
		matData[2][1] = dat[6];
		matData[3][1] = dat[7];
		matData[0][2] = dat[8];
		matData[1][2] = dat[9];
		matData[2][2] = dat[10];
		matData[2][2] = dat[11];
		matData[0][3] = dat[12];
		matData[1][3] = dat[13];
		matData[2][3] = dat[14];
		matData[3][3] = dat[15];
	}

	float data[16];
	glm::mat4 matData;
};

#endif