/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>


#include <locale.h>
#include <iostream>
#include <cmath>


MainWidget::MainWidget(QWidget *parent) :
    QGLWidget(parent),
    angularSpeed(0)
{
	QGLFormat newFormat = this->format();
	newFormat.setSampleBuffers(true);
	newFormat.setSamples(16);
	this->setFormat(newFormat);

}

MainWidget::~MainWidget()
{
    deleteTexture(texture);
}

//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
	mousePressPosition = QVector2D(e->pos());

	// cm: data not used anymore!
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
	QVector2D diff = QVector2D(e->pos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;

	// cm: data not used anymore!
}
//! [0]

//! [1]
void MainWidget::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
		//rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Update scene
		//updateGL();

		// cm: not used anymore, we read the rotation from the serial interface!
    }
}
//! [1]

void MainWidget::initializeGL()
{


    initializeGLFunctions();
	qglClearColor(Qt::red);
    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
	glEnable(GL_CULL_FACE);
//! [2]

    geometries.init();

    // Use QBasicTimer because its faster than QTimer
	//timer.start(12, this);

}

//! [3]
void MainWidget::initShaders()
{
    // Override system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();

    // Restore system locale
    setlocale(LC_ALL, "");
}
//! [3]

//! [4]
void MainWidget::initTextures()
{
	// Load cube.png image
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	texture = bindTexture(QImage(":/cube.png"));

	// Set nearest filtering mode for texture minification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Set bilinear filtering mode for texture magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrap texture coordinates by repeating
	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Load cube.png image
    glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	texture = bindTexture(QImage(":/rotors.jpg"));

    // Set nearest filtering mode for texture minification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Set bilinear filtering mode for texture magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void MainWidget::setRotation(float w, float x, float y, float z, int fAx, int fAy, int fAz)
{
	// Update rotation
	rotation.setScalar(w);
	rotation.setVector(x, y, z);

	double sqw = w*w;
	double sqx = x*x;
	double sqy = y*y;
	double sqz = z*z;
	double dPI = 3.14159265359;

	double eulerZ = (atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw)) * (180.0/dPI));
	double eulerX = (atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw)) * (180.0/dPI));
	double eulerY = (asin(-2.0 * (x*z - y*w)) * (180.0/dPI));

	std::cout << "euler X: " << eulerX << ", Y: " << eulerY << ", Z: " << eulerZ << std::endl;

	const float fScale = 0.0001f;


	acceleration += QVector3D((fAx-10) * fScale, (fAy-120) * fScale, (fAz+950)*fScale);

	// Update translation
	translation += QVector3D((fAx-10) * fScale, (fAy-120) * fScale, (fAz+950)*fScale);

	// Update scene
	updateGL();
}
//! [4]

//! [5]
void MainWidget::resizeGL(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
	const qreal zNear = 1.0, zFar = 100.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;
	matrix.rotate(20.0f, 1.0f, 0.0f, 0.0f);
	matrix.translate(0.0, -2.4, -9.0);
	matrix.rotate(270.0f, 1.0f, 0.0f, 0.0f);

	QMatrix4x4 rotatedMatrix(matrix);
	//rotatedMatrix.translate(translation);
	rotatedMatrix.rotate(rotation);

    // Set modelview-projection matrix
	rotatedMatrix.scale(2.0, 2.0, 0.01);
	program.setUniformValue("mvp_matrix", projection * rotatedMatrix);
//! [6]


	// Use texture unit 0 which contains cube.png
	program.setUniformValue("texture", 1);

	// Draw cube geometry
	glCullFace(GL_BACK);
	geometries.drawCubeGeometry(&program);

	matrix.scale(5.0);

	glCullFace(GL_FRONT);	
	program.setUniformValue("mvp_matrix", projection * matrix);

    // Use texture unit 0 which contains cube.png
	program.setUniformValue("texture", 0);

    // Draw cube geometry
    geometries.drawCubeGeometry(&program);

}
