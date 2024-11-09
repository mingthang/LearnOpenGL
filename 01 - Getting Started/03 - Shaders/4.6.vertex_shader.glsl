#version 460 core

layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec3 aColor; 

out vec3 vertexColor; 

uniform float time; 

vec3 lerp(vec3 pointA, vec3 pointB, float t)
{
    return pointA + (pointB - pointA) * t;
}

// Multi-stop gradient function
vec3 linearGradient(vec3 stops[4], float value) {
    const float stopLength = 1.0 / (4.0 - 1.0); 
    const float valueRatio = value / stopLength; 
    const int stopIndex = int(floor(valueRatio)); 

    if (stopIndex >= 3) {
        return stops[3]; 
    }

    float stopFraction = mod(valueRatio, 1.0); 
    return lerp(stops[stopIndex], stops[stopIndex + 1], stopFraction); 
}

void main()
{   
    vec3 stops[4];
    // Define the colors for each vertex
    if (aColor == vec3(1.0, 0.0, 0.0)) { 
        stops[0] = vec3(1.0, 0.0, 0.0); // Red
        stops[1] = vec3(0.0, 1.0, 0.0); // Green
        stops[2] = vec3(0.0, 0.0, 1.0); // Blue
        stops[3] = vec3(1.0, 0.0, 0.0); // Red
    } else if (aColor == vec3(0.0, 1.0, 0.0)) { 
        stops[0] = vec3(0.0, 1.0, 0.0); // Green
        stops[1] = vec3(0.0, 0.0, 1.0); // Blue
        stops[2] = vec3(1.0, 0.0, 0.0); // Red
        stops[3] = vec3(0.0, 1.0, 0.0); // Green
    } else if (aColor == vec3(0.0, 0.0, 1.0)) { 
        stops[0] = vec3(0.0, 0.0, 1.0); // Blue
        stops[1] = vec3(1.0, 0.0, 0.0); // Red
        stops[2] = vec3(0.0, 1.0, 0.0); // Green
        stops[3] = vec3(0.0, 0.0, 1.0); // Blue
    }

    float cycleTime = mod(time, 1.0f);  // [0.0, 1.0]

    vertexColor = linearGradient(stops, cycleTime); 
    gl_Position = vec4(aPos, 1.0f); 
}
