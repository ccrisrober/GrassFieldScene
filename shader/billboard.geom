/*
 *	MIT License
 *
 * Copyright (c) <2016> <Cristian Rodríguez Bernal (maldicion069)>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#version 430
layout (points) in;
layout (triangle_strip, max_vertices = 8) out;

out vec2 TexCoord;

uniform mat4 modelViewProj;

uniform float time;
uniform float beta;

#define PI 3.1415

vec2 size = vec2(1.0, 2.0);

void generateQuad(in mat4 Transf, int sign) {
	vec4 Position = gl_in[0].gl_Position;
	gl_Position = Position;
	gl_Position.xy += vec2(sign * size.x, -size.y / 2.0);
	gl_Position = Transf * gl_Position;
	TexCoord = vec2(0.0, 0.0);
	EmitVertex();
	
	gl_Position = Position;
	gl_Position.xy += vec2(sign * size.x, size.y / 2.0);
	gl_Position = Transf * gl_Position;
	TexCoord = vec2(0.0, 1.0);
	EmitVertex();
	gl_Position = Position;
	gl_Position.xy += vec2(0.0, -size.y / 2.0);
	gl_Position = Transf * gl_Position;
	TexCoord = vec2(1.0, 0.0);
	EmitVertex();
	gl_Position = Position;
	gl_Position.xy += vec2(0.0, size.y / 2.0);
	gl_Position = Transf * gl_Position;
	TexCoord = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}

void main()  {
	vec4 Position = gl_in[0].gl_Position;
	// Alpha: Aperture quad angle.
	float alpha = radians(-65.0) + 0.9*abs(sin(time));

	// Translate quad to origin
	mat4 Tgo = mat4(
		vec4(1.0, 0.0, 0.0, 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(-Position.xyz, 1.0));

	// Translate quad to original position
	mat4 Tini = mat4(
		vec4(1.0, 0.0, 0.0, 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(Position.xyz, 1.0));

	// Rotate quads with Y axis rotation. Give appearance of moving quads (left quad)
	mat4 RotQ = mat4(
		vec4(cos(alpha), 0.0, -sin(alpha), 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(sin(alpha), 0.0, cos(alpha), 0.0),
		vec4(0.0, 0.0, 0.0, 1.0));

	// Rotate quads with Z axis rotation. "Flying" direction
	mat4 RotZ = mat4(
		vec4(cos(beta), sin(beta), 0.0, 0.0),
		vec4(-sin(beta), cos(beta), 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(0.0, 0.0, 0.0, 1.0));

	// Final transform matrix
	mat4 Transf = modelViewProj * RotZ * Tini * RotQ * Tgo;

	// Generate quad from original point´s position. Finally transform with Transf matrix.
	generateQuad(Transf, -1);

	// Alpha: Aperture angle for simetric quad
	alpha = PI - alpha;
	// Rotate quads with Y axis rotation. Give appearance of moving quads (right quad)
	RotQ = mat4(
		vec4(cos(alpha), 0.0, -sin(alpha), 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(sin(alpha), 0.0, cos(alpha), 0.0),
		vec4(0.0, 0.0, 0.0, 1.0));

	Transf = modelViewProj * RotZ * Tini * RotQ * Tgo;
	generateQuad(Transf, 1);
}