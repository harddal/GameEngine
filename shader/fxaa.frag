void main()
{
	ivec2 texturePosition = ivec2(gl_FragCoord.x, gl_FragCoord.y);
	vec4 colorSample0 = texelFetch(tex, texturePosition, 0);

	vec4 colorSample1 = texelFetch(tex, texturePosition, 1);
	vec4 colorSample2 = texelFetch(tex, texturePosition, 2);
	vec4 colorSample3 = texelFetch(tex, texturePosition, 3);

	vec4 antialiased = (colorSample0 + colorSample1 + colorSample2 + colorSample3) / 4.0f;

	// do fancy post processing stuff...

	gl_FragColor = antialiased;
}