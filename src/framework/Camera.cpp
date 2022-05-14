/*
  Copyright (C) 2022 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
  3. The names of the authors may not be used to endorse or promote
	 products derived from this software without specific prior
	 written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "miGUI.h"

#include "framework/mgf.h"
#include "framework/Camera.h"
#include "framework/Math.h"

using namespace mgf;

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::EditorUpdate()
{
	if ((m_editorCameraType != CameraEditorType::Perspective) || m_forceOrtho)
	{
		float zoom = m_positionPlatform.w;
		math::makeOrthoRHMatrix(m_projectionMatrix,
			zoom * m_aspect,
			zoom,
			-m_far,
			m_far);
	}
	else
	{
		math::makePerspectiveRHMatrix(m_projectionMatrix, m_fov, m_aspect, m_near, m_far);
	}

	m_projectionMatrix.m_data[0].x = -m_projectionMatrix.m_data[0].x;

	Mat4 MX(Quat(m_rotationPlatform.x, 0.f, 0.f));
	Mat4 MY(Quat(0.f, m_rotationPlatform.y, 0.f));
	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));

	m_positionCamera = v3f(0.f, m_positionPlatform.w, 0.f);
	m_positionCamera = math::mul(m_positionCamera, (MY * MX));
	m_positionCamera += v3f(m_positionPlatform.x, m_positionPlatform.y, m_positionPlatform.z);

	Mat4 T;
	T.m_data[3].x = -m_positionCamera.x;
	T.m_data[3].y = -m_positionCamera.y;
	T.m_data[3].z = -m_positionCamera.z;
	T.m_data[3].w = 1.f;

	Mat4 P(Quat(v4f(-m_rotationPlatform.x + math::degToRad(-90.f), 0.f, 0.f, 1.f)));
	Mat4 Y(Quat(v4f(0.f, -m_rotationPlatform.y + math::degToRad(0.f), 0.f, 1.f)));
	Mat4 R(Quat(v4f(0.f, 0.f, m_rotationPlatform.z, 1.f)));

	m_viewMatrix = (R * (P * Y)) * T;
	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;

	m_viewMatrixInvert = m_viewMatrix; m_viewMatrixInvert.invert();
	auto pi = m_projectionMatrix; pi.invert();
	m_viewProjectionInvertMatrix = m_viewMatrixInvert * pi;
	m_frust.CalculateFrustum(m_projectionMatrix, m_viewMatrix);

	m_direction = Direction::NorthEast;

	if (m_rotationPlatform.y >= 2.7488936 && m_rotationPlatform.y <= 3.5342917)
	{
		m_direction = Direction::North;
	}
	else if (m_rotationPlatform.y >= 5.8904862 || m_rotationPlatform.y <= 0.3926991)
	{
		m_direction = Direction::South;
	}
	else if (m_rotationPlatform.y >= 1.1780972 && m_rotationPlatform.y <= 1.9634954)
	{
		m_direction = Direction::West;
	}
	else if (m_rotationPlatform.y >= 4.3196899 && m_rotationPlatform.y <= 5.1050881)
	{
		m_direction = Direction::East;
	}
	else if (m_rotationPlatform.y >= 1.9634954 && m_rotationPlatform.y <= 2.7488936)
	{
		m_direction = Direction::NorthWest;
	}
	else if (m_rotationPlatform.y >= 0.3926991 && m_rotationPlatform.y <= 1.1780972)
	{
		m_direction = Direction::SouthWest;
	}
	else if (m_rotationPlatform.y >= 5.1050881 && m_rotationPlatform.y <= 5.8904862)
	{
		m_direction = Direction::SouthEast;
	}
}

void Camera::Reset()
{
	m_near = 0.01f;
	m_far = 2500.f;
	m_fov = 0.683264f;
	m_aspect = 800.f / 600.f;
	m_positionPlatform = v4f(0.f, 0.f, 0.f, 15.f);
	switch (m_editorCameraType)
	{
	case CameraEditorType::Perspective:
		m_rotationPlatform = v3f(math::degToRad(-45.f), 0.f, 0.f);
		break;
	case CameraEditorType::Bottom:
		m_rotationPlatform = v3f(math::degToRad(-180.f), 0.f, 0.f);
		break;
	case CameraEditorType::Left:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(-90.f), 0.f);
		break;
	case CameraEditorType::Right:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(90.f), 0.f);
		break;
	case CameraEditorType::Back:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(180.f), 0.f);
		break;
	case CameraEditorType::Front:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(0.f), 0.f);
		break;
	case CameraEditorType::Top:
		m_rotationPlatform = v3f();
		break;
	}

	mgPoint rp;
	EditorRotate(&rp, 1.f);
}

void Camera::EditorPanMove(mgPoint* mouseDelta, float timeDelta)
{
	float speed = 10.f * (m_positionPlatform.w * 0.01f);

	v4f vec(
		speed * -mouseDelta->x * timeDelta,
		0.f,
		speed * -mouseDelta->y * timeDelta,
		0.f);
	Mat4 MX(Quat(m_rotationPlatform.x, 0.f, 0.f));
	Mat4 MY(Quat(0.f, m_rotationPlatform.y, 0.f));
	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));
	vec = math::mul(vec, MY * MX);
	m_positionPlatform += vec;
}

void Camera::EditorRotate(mgPoint* mouseDelta, float timeDelta)
{
	const float speed = 0.69f * timeDelta;
	m_rotationPlatform.x += mouseDelta->y * speed;
	m_rotationPlatform.y += mouseDelta->x * speed;

	if (m_rotationPlatform.y < 0.f) m_rotationPlatform.y = m_rotationPlatform.y + math::PIPI;

	if (m_rotationPlatform.x > math::PIPI) m_rotationPlatform.x = 0.f;
	if (m_rotationPlatform.y > math::PIPI) m_rotationPlatform.y = 0.f;
	//if (m_rotationPlatform.x < -math::PIPI) m_rotationPlatform.x = 0.f;
	//if (m_rotationPlatform.y < -math::PIPI) m_rotationPlatform.y = 0.f;


	//if (m_type != miViewportCameraType::Perspective)
	//	m_viewport->SetViewportName(L"Orthogonal");

	//Update();
}

void Camera::EditorZoom(int wheelDelta)
{
	float mult = 1.f;
	//if (g_app->m_inputContext->m_kbm == miKeyboardModifier::Shift)
		//mult = 3.f;

	if (wheelDelta > 0)
		m_positionPlatform.w *= 0.9f * (1.f / mult);
	else
		m_positionPlatform.w *= 1.1f * mult;

	if (m_positionPlatform.w < 0.01f)
		m_positionPlatform.w = 0.01f;

	//Update();
}

void Camera::EditorChangeFOV(mgPoint* mouseDelta, float timeDelta)
{
	m_fov += mouseDelta->x * timeDelta;
	if (m_fov < 0.01f)
		m_fov = 0.01f;
	if (m_fov > math::PI)
		m_fov = math::PI;
}

void Camera::EditorRotateZ(mgPoint* mouseDelta, float timeDelta)
{
	m_rotationPlatform.z += mouseDelta->x * timeDelta;
}
