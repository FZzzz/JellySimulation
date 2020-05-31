#version 410

in vec3 light_dir;

uniform mat4 view;

out vec4 frag_color;

void main()
{
    vec3 n;
    n.xy = gl_PointCoord.xy * 2.f - 1.f;
    n.z = 1.f - dot(n.xy, n.xy);
 
    if(n.z < 0.f) 
        discard;
    
    n.z = sqrt(n.z);
    /*transform with modelview matrix*/
    //vec3 m = normalize(gl_NormalMatrix * n);
    float diffuse = dot(light_dir, n);
    frag_color.xyz = vec3(diffuse);
    frag_color.w = 1.f;
}