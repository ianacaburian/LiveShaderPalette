# LiveShaderPalette

Run your interactive animated OpenGL fragment shaders while editing.

  - "Live compile" allows the shader file to be recompiled at an adjustable rate to streamline quick shader editing.
  - Interactive mouse/touch input animation via juce::MouseListener interface.
  - Drag and drop shader files directly onto panels for side-by-side comparison editing.
  - Arrange panels by rows, columns or in a tiled layout fashion.
  - Benchmark shader performance with the ms/frame display when using a large number of panels.
  - Separate window console displays the state of the fragment's uniforms for easy debugging.

## Installation
  - Either download and run one of the pre-built binaries or clone and build with JUCE.
  - To get started quickly, run the app, then copy the shaders from this repo's ShaderExamples folder into the app's CWD folder (located in your Documents folder, generated automatically upon first launch).
  
## How to use
>Fragment file extension must be ".frag".   
##### Load files via folder  
  - Click the "Refresh folder" button to reload all panels using the files currently in "CWD".
##### Load files via drag-and-drop
  - To load a single file into all panels, drag onto the tool bar area.
  - To load into a single panel, drag onto any panel. 
##### Panel Layout
  - Enter the number of panels to display at the top left.
##### Console
  - The lower area displays the OpenGL shader compiler errors.
  - Live compile rate: The rate that the app will recompile the current files.
  - Period: The rate that the sin and saw time functions use for animation speed.
  - Rendering scale: All console coordinates (screen/panel size and mouse positions) are given in JUCE screen coordinates; multiply these by the renderingScale if OpenGL coordinates are required.
  - Component ID: The ID of the panel that's associated with the current mouse event.
  - Mouse positions: Mouse position is the current position whereas mouse down position is the position since the mouse was last pressed down; useful for drag events.
  - Toggle flag: Simulates a toggle button; useful for designing toggleable button components.
  - Wheel move delta: An (x, y) vector where x represents horizontal wheel movement and y, vertical movement.
  - Magnify scale factor: The zoom scale that results from pinching touch gestures.
 
##### Uniforms reference
> The following can be pasted directly at the start of your fragment shader source file.
`uniform ivec4   uf_componentID_layout;  // { Component ID, Layout Type [ 0 = Tiled, 1 = Rows, 2 = Columns ], Number of panels, 0 }`
`uniform ivec4   uf_resolution;          // { Panel width, Panel height, Panels area width, Panels area height }`
`uniform float   uf_rendering_scale;     // { openGLContext::getRenderingScale() }`
`uniform int     uf_mouse_type;          // [ 0 = Move, 1 = Enter, 2 = Exit, 3 = Down, 4 = Drag, 5 = Up, 6 = DoubleClick, 7 = WheelMove, 8 = Magnify ]`
`uniform vec4    uf_mouse_position;      // { Mouse x-position, Mouse y-position, Mouse down x-position, Mouse down y-position }`
`uniform vec4    uf_time;                // { Mouse event time, Mouse down time, Sin time, Saw time }`
`uniform ivec4   uf_flags;               // { Mouse button is down, Button toggle flag, Right mouse button, 0 }`
`uniform vec3    uf_mouse_options;       // { Mouse wheel delta-x, Mouse wheel delta-y, Mouse magnify scale factor }`

