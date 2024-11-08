precision mediump float;

#define PI 3.14159265359

vec3 col1 = vec3(0.216, 0.471, 0.698); // blue
vec3 col2 = vec3(1.00, 0.329, 0.298); // yellow
vec3 col3 = vec3(0.867, 0.910, 0.247); // red
vec3 col4 = vec3(0.2, 0.5, 0.7);

float disk(vec2 r, vec2 center, float radius) {
    return 1.0 - smoothstep(radius - 0.008, radius + 0.008, length(r - center));
}

float noise(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float fbm(vec2 p) {
    float value = 0.0;
    float scale = 0.5;
    for (int i = 0; i < 4; i++) {
        value += scale * noise(p);
        p *= 2.0;
        scale *= 0.5;
    }
    return value;
}

uniform float iTime;
uniform vec2 iResolution;
uniform bool enableFBM;

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    float t = iTime * 2.0;
    vec2 r = (2.0 * fragCoord - iResolution) / iResolution.y;
    r *= 1.0 + 0.1 * sin(r.x * 5.0 + iTime) + 0.1 * sin(r.y * 3.0 + iTime);
    r *= 1.0 + 0.3 * length(r);

    float side = 0.5;
    vec2 r2 = mod(r, side);
    vec2 r3 = r2 - side / 2.0;
    float i = floor(r.x / side) + 2.0;
    float j = floor(r.y / side) + 4.0;
    float ii = r.x / side + 2.0;
    float jj = r.y / side + 4.0;

    vec3 pix = vec3(1.0);

    float rad, disks;

    rad = 0.15 + 0.05 * sin(t + ii * jj);
    disks = disk(r3, vec2(0.0, 0.0), rad);
    pix = mix(pix, col2, disks);

    float speed = 2.0;
    float tt = iTime * speed + 0.1 * i + 0.08 * j;
    float stopEveryAngle = PI / 2.0;
    float stopRatio = 0.7;
    float t1 = (floor(tt) + smoothstep(0.0, 1.0 - stopRatio, fract(tt))) * stopEveryAngle;

    float x = -0.07 * cos(t1 + i);
    float y = 0.055 * (sin(t1 + j) + cos(t1 + i));
    rad = 0.1 + 0.05 * sin(t + i + j);
    disks = disk(r3, vec2(x, y), rad);
    pix = mix(pix, col1, disks);

    if (enableFBM) {
        float noiseValue = fbm(r * 10.0 + iTime * 0.5);
        pix += noiseValue * 0.2 * col4;
    }

    rad = 0.2 + 0.05 * sin(t * (1.0 + 0.01 * i));
    disks = disk(r3, vec2(0.0, 0.0), rad);
    pix += 0.2 * col3 * disks * sin(t + i * j + i);

    pix -= smoothstep(0.3, 5.5, length(r));
    fragColor = vec4(pix, 1.0);
}

void main() {
    mainImage(gl_FragColor, gl_FragCoord.xy);
}
