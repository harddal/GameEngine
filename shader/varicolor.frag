uniform float fTime;
uniform vec2 iResolution;

void main(void)
{
    vec2 uv = gl_FragCoord / iResolution.xy;
    vec3 col = 0.5 + 0.5 * cos(fTime + uv.xyx + vec3(0.0, 2.0, 4.0));
    gl_FragColor = vec4(col, 1.0);
}