uniform float u_Time;

const float PI = 3.14159265359;

#define Depth gl_FragDepth

uniform vec3 u_CameraPosition;

bool IsFrontFace() { return gl_FrontFacing; }
#define TwoSidedSign ((float(gl_FrontFacing) - 0.5) * 2.0)