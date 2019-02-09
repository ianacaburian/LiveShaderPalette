//#version 330 core
// On macOS, only shader version 150 works, 330 isn’t supported
#version 150

uniform float uf_sin_time;
uniform vec4 uf_panel;
out vec4 out_color;

void main()
{
//    vec2 st = (gl_PointCoord.xy + vec2(1, 1)) / 2.;
//    vec3 border_color = vec3(0.0);
//
//    // bottom-left
//    vec2 bl = step(vec2(0.1),st);
//    float pct = bl.x * bl.y;
//
//    // top-right
//    vec2 tr = step(vec2(0.1),1.0-st);
//    pct *= tr.x * tr.y;
//
//    border_color = vec3(pct);
//
//    vec2 p = st * resolution / distance;
//    float thickness = 0.02;
//    float radius = 0.5;
//    float intensity = thickness/abs(radius - length(p));
//
//    out_color = vec4(intensity, 0., intensity, .5);
//    vec2 p = (gl_PointCoord.xy) / (uf_sin_time / (uf_panel.y * 2));
//    float thickness = 0.02;
//    float radius = 0.5;
//    float intensity = thickness/abs(radius - length(p));
//
//    out_color = vec4(0., intensity, intensity, .5);
    
        float result = 0.;
    //    if (sin_time <= gl_PointCoord.x && gl_PointCoord.x < sin_time + 0.1) {
        vec2 res = uf_panel.xy * uf_panel.z;
    //    if (res.x * 0.25 <= gl_FragCoord.x && gl_FragCoord.x < res.x * 0.25 + res.x * 0.5
    //        && res.y * 0.25 <= gl_FragCoord.y && gl_FragCoord.y < res.y * 0.25 + res.y * 0.5) {
        if (uf_sin_time<= gl_PointCoord.y && gl_PointCoord.y < (uf_sin_time + .1)) {
            result = 1.;
        }
        out_color = vec4(0., result, 0., 1.);

}

