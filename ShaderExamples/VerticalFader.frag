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
    // Simulation of a vertical audio mixer fader.
    // Click and drag to raise or lower the position of the horizontal line.
    float thickness = .005;
//    vec2 position = vec2(gl_PointCoord.x, gl_PointCoord.y - 1 + 2 * u_mouse_position.y / u_resolution.y);
    vec2 position = vec2(0., gl_FragCoord.y / u_resolution.y - 2 + 2 * u_mouse_position.y / u_resolution.y);
    float intensity = thickness/abs(length(vec2(0., .5) * position));
    out_color = vec4(intensity * 0.1, intensity * 0.4, intensity * 0.5, .5);
}

