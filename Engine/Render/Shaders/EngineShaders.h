/* Here are located sources for engine shaders */

namespace Engine {
    const char* DEBUG_VERT = R"(
        #version 130

        in vec2 vertexPosition; // Vertex position from VBO
        in vec4 vertexColor;    // Vertex color from VBO

        // Output data for fragment shader
        out vec2 fragmentPosition;
        out vec4 fragmentColor;

        uniform mat4 P;

        void main() {
            gl_Position.xy = (P * vec4(vertexPosition, 0.0, 1.0)).xy;
            gl_Position.z = 0.0;
            
            // Coordinates are normalized
            gl_Position.w = 1.0;
            
            fragmentPosition = vertexPosition;
            fragmentColor = vertexColor;
        }
    )";

    const char* DEBUG_FRAG = R"(
        #version 130

        in vec2 fragmentPosition;
        in vec4 fragmentColor;

        // This is the 3 component float vector that gets outputted to the screen for each pixel.
        out vec4 color;

        void main() {
            color = fragmentColor;
        }
    )";
}
