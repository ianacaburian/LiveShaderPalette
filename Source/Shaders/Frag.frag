//#version 330 core
// On macOS, only shader version 150 works, 330 isn’t supported
#version 150

uniform vec4    uf_componentID_layout;  // { Component ID, Layout Type [ 0 = Tiled, 1 = Rows, 2 = Columns ], Number of panels, 0 }
uniform vec4    uf_screen_size;         // { Panel width, Panel height, Screen width, Screen height }
uniform float   uf_rendering_scale;
uniform int     uf_mouse_type;          // [ 0 = Move, 1 = Enter, 2 = Exit, 3 = Down, 4 = Drag, 5 = Up, 6 = DoubleClick, 7 = WheelMove, 8 = Magnify ]
uniform vec4    uf_mouse_position;      // { Mouse x-position, Mouse y-position, Mouse down x-position, Mouse down y-position }
uniform vec4    uf_time;                // { Mouse event time, Mouse down time, Sin time, Saw time }
uniform vec4    uf_flags;               // { Mouse button is down, Button toggle flag, Right mouse button, 0 }
uniform vec2    uf_mouse_options;       // if (uf_mouse_type == 7) { delta-x, delta-y }; if (uf_mouse_type == 8) { Scale factor, 0. }; else { 0., 0. }

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
    
    
//    vec2 p = gl_PointCoord.xy / uf_sin_time;
//    float thickness = 0.02;
//    float radius = 0.5;
//    float intensity = thickness/abs(radius - length(p));
//
//    out_color = vec4(0., intensity, intensity, .5);
    
//        float result = 0.;
//    //    if (sin_time <= gl_PointCoord.x && gl_PointCoord.x < sin_time + 0.1) {
//        vec2 res = uf_panel.xy * uf_panel.z;
//    //    if (res.x * 0.25 <= gl_FragCoord.x && gl_FragCoord.x < res.x * 0.25 + res.x * 0.5
//    //        && res.y * 0.25 <= gl_FragCoord.y && gl_FragCoord.y < res.y * 0.25 + res.y * 0.5) {
//        if (uf_sin_time<= gl_PointCoord.y && gl_PointCoord.y < (uf_sin_time + .1)) {
//            result = 1.;
//        }
//        out_color = vec4(0., result, 0., 1.);

    float distance = 0.5;
    vec2 p = gl_PointCoord.xy / distance;
    float thickness = .01;
    float radius = 0.5;
    float intensity = thickness/abs(radius - length(p));
    
    out_color = vec4(0., intensity, 0., .5);

}

