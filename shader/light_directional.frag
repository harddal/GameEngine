uniform sampler2D ColorTex;     
uniform sampler2D NormalTex;    
uniform sampler2D DepthTex;   
uniform vec3 Direction;
uniform vec3 Color;

void main()
{
    vec4 vNormal= texture2D(NormalTex, gl_TexCoord[0].xy);

    //reconstruct normal
    vNormal.xy*= 2.0;
    vNormal.xy-= 1.0;
    vNormal.z= -sqrt( -(vNormal.x*vNormal.x) - (vNormal.y*vNormal.y) + 1.0 );

    //calculate the light
    vec4 lightDir= vec4(normalize(Direction), 0.0);
    float light= max(dot(lightDir, vNormal), 0.0);

    if(light > 0.0)
    {
        gl_FragColor= light * vec4(Color, 0.0) * texture2D(ColorTex, gl_TexCoord[0].xy);
    }
    else discard;
}
