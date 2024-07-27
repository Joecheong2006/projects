#shader vertex
#version 330 core

uniform mat4 view;
uniform vec3 points[2];

void main()
{
    gl_Position = view * vec4(points[gl_VertexID].xyz, 1);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 frag_color;
uniform vec3 color;

void main()
{
    frag_color = vec4(color.rgb, 1);
}