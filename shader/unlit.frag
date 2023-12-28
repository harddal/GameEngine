uniform sampler2D tDiffuse;

varying vec2 vTexCoord;

void main (void)
{
    gl_FragColor = texture2D(tDiffuse, vTexCoord);
}
