#version 460 core

layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec3 aColor; 
layout(location = 2) in vec2 aTexCoord;

out vec3 vertexColor; 
out vec2 TexCoord;

uniform float time; 

vec3 lerp(vec3 pointA, vec3 pointB, float t)
{
    return pointA + (pointB - pointA) * t;
}

// Multi-stop gradient function
vec3 linearGradient(vec3 stops[5], float value) {
    const float stopLength = 1.0 / (5.0 - 1.0); 
    const float valueRatio = value / stopLength; 
    const int stopIndex = int(floor(valueRatio)); 

    if (stopIndex >= 4) {
        return stops[4]; 
    }

    float stopFraction = mod(valueRatio, 1.0); 
    return lerp(stops[stopIndex], stops[stopIndex + 1], stopFraction); 
}

void main()
{   
    vec3 stops[5];
	if (aColor == vec3(1.0, 0.0, 0.0)) { 
		stops[0] = vec3(0.9, 0.0, 0.0); // Slightly darker red
		stops[1] = vec3(1.0, 0.8, 0.0); // Gold yellow
		stops[2] = vec3(0.1, 0.7, 0.3); // Lush green
		stops[3] = vec3(0.0, 0.4, 1.0); // Bright blue
		stops[4] = vec3(1.0, 0.0, 0.0); // Red
	} else if (aColor == vec3(0.0, 1.0, 0.0)) { 
		stops[0] = vec3(0.1, 0.7, 0.3); // Lush green
		stops[1] = vec3(0.0, 0.4, 1.0); // Blue
		stops[2] = vec3(0.9, 0.0, 0.0); // Red
		stops[3] = vec3(1.0, 0.8, 0.0); // Yellow
		stops[4] = vec3(0.1, 0.7, 0.3); // Green
	} else if (aColor == vec3(0.0, 0.0, 1.0)) { 
		stops[0] = vec3(0.0, 0.4, 1.0); // Blue
		stops[1] = vec3(0.9, 0.0, 0.0); // Red
		stops[2] = vec3(1.0, 0.8, 0.0); // Yellow
		stops[3] = vec3(0.1, 0.7, 0.3); // Green
		stops[4] = vec3(0.0, 0.4, 1.0); // Blue
	} else if (aColor == vec3(1.0, 1.0, 0.0)) {
		stops[0] = vec3(1.0, 0.8, 0.0); // Yellow
		stops[1] = vec3(0.1, 0.7, 0.3); // Green
		stops[2] = vec3(0.0, 0.4, 1.0); // Blue
		stops[3] = vec3(0.9, 0.0, 0.0); // Red
		stops[4] = vec3(1.0, 0.8, 0.0); // Yellow
	}

    float cycleTime = mod(time, 1.0f);  // [0.0, 1.0]

    vertexColor = linearGradient(stops, cycleTime); 
    gl_Position = vec4(aPos, 1.0f); 
    TexCoord = aTexCoord;
}
