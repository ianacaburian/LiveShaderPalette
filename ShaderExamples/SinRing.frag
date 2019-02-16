#version 150

uniform ivec4   uf_componentID_layout;      // ( x = Component ID, y = Layout type { 0 = Tiled, 1 = Rows, 2 = Columns }, z = Number of panels, w = 0 )
uniform ivec4   uf_resolution;              // ( x = Panel width, y = Panel height, z = Panels area width, w = Panels area height )
uniform float   uf_rendering_scale;         // ( openGLContext::getRenderingScale() )
uniform vec2    uf_event_position;          // ( x = Event x-position, y = Event y-position )
uniform vec4    uf_mouse_position;          // ( x = Mouse up x-position, y = Mouse up y-position, z = Mouse down x-position, w = Mouse down y-position )
uniform ivec4   uf_mouse_time;              // ( x = Mouse event time, y = Mouse up time, z = Mouse down time, 0 )
uniform vec2    uf_periodic_time;           // ( x = Sin time, y = Saw time )
uniform ivec4   uf_flags;                   // ( x = Mouse type { 0 = Move, 1 = Enter, 2 = Exit, 3 = Down, 4 = Drag, 5 = Up, 6 = DoubleClick, 7 = WheelMove, 8 = Magnify },
                                            //   y = Mouse button is down, z = Button toggle flag, w = Right mouse button, 0 )
uniform vec3    uf_mouse_options;           // ( x = Mouse wheel delta-x, y = Mouse wheel delta-y, z = Mouse magnify scale factor )

out vec4 out_color;

void main()
{
    // Radius follows the y position of the mouse.
    // Arrange 20 or more of these shaders in columns while mouse hovering for the best effect.
    
    float distance_factor = uf_periodic_time.x;
    vec2 p = gl_PointCoord.xy / vec2(distance_factor, distance_factor * 0.5);
    float thickness = .01;
    float radius_factor = uf_event_position.y / uf_resolution.y;
    float intensity = thickness/abs(radius_factor - length(p));
    out_color = vec4(intensity * 0.8, intensity * 0.1, intensity, .5);

}

