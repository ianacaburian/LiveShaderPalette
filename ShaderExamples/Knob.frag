uniform ivec4   u_componentID_layout;   // ( x = Component ID, y = Layout type { 0 = Tiled, 1 = Rows, 2 = Columns }, z = Number of panels, w = 0 )
uniform vec4    u_bounds;               // ( x = Panel x-position, y = Panel y-position, z = Panel width, w = Panel height )
uniform ivec4   u_resolution;           // ( x = Panel width, y = Panel height, z = Panels area width, w = Panels area height )
uniform float   u_rendering_scale;      // ( openGLContext::getRenderingScale() )
uniform vec2    u_event_position;       // ( x = Event x-position, y = Event y-position )
uniform vec4    u_mouse_position;       // ( x = Mouse up x-position, y = Mouse up y-position, z = Mouse down x-position, w = Mouse down y-position )
uniform ivec4   u_mouse_time;           // ( x = Mouse event time, y = Mouse up time, z = Mouse down time, 0 )
uniform vec2    u_periodic_time;        // ( x = Sin time, y = Saw time )
uniform ivec4   u_flags;                // ( x = Mouse type { 0 = Move, 1 = Enter, 2 = Exit, 3 = Down, 4 = Drag, 5 = Up, 6 = DoubleClick, 7 = WheelMove, 8 = Magnify },
                                        //   y = Mouse button is down, z = Button toggle flag, w = Right mouse button, 0 )
uniform vec3    u_mouse_options;        // ( x = Mouse wheel delta-x, y = Mouse wheel delta-y, z = Mouse magnify scale factor )

out vec4 out_color;

void main()
{
    // Click and drag vertically to rotate the knob.
    // Credit to thebookofshaders.com for the template.
    
    vec2 st = (gl_PointCoord.xy + vec2(1, 1)) / 2.;
    vec2 pos = vec2(0.5)-st;
    float mouse = u_mouse_position.y / u_resolution.y;
    float r = length(pos)*2.0;
    float a = atan(pos.y,pos.x) - (u_mouse_position.y * 0.01);
    float f = smoothstep(-1.5,0.2, cos(a*10))*0.2+0.7;
    vec3 color = vec3( 1. -smoothstep(f,f+0.02,r) );
    
    float p =  length(gl_PointCoord.xy);
    float f1 = cos(a * 10);
    vec3 color1 = vec3(1.8 -smoothstep(f1,f1,r * (p -2) * 0.9) );
    
    out_color = vec4(color.x / color1.x * 0.4, 0., color.x / color1.x * 0.8, 1.0);
}

