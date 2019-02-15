# LiveShaderPalette

  - Live code your animated OpenGL fragment shaders.
  - Interactive graphics design made easier with ready made uniform routings for mouse/touch input events.
  - Arrange panels by rows, columns or in a tiled layout fashion for quick side-by-side comparison editing.
  - With a high number of panels in fullscreen, shader code quality can be profiled or benchmarked. 
  - Includes separate window console displaying the state of all uniforms and any OpenGL compilation errors.

## Installation
  - Either download and run one of the pre-built binaries or clone and build with JUCE.
## Quick start:
- Run the app.
- Copy the shaders from this repo's ShaderExamples folder into the app's CWD folder (which upon first launch gets generated in your Documents folder).
- Click "Refresh Folder"
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
  - Rendering scale: All console coordinates (screen/panel size and mouse positions) are provided in JUCE screen coordinates; multiply these by the rendering scale if OpenGL coordinates are required.
  - Component ID: The ID of the panel that's associated with the current mouse event.
  - Mouse positions: Mouse position is the current position whereas mouse down position is the position since the mouse was last pressed down; useful for drag events.
  - Toggle flag: Simulates a toggle button; useful for designing toggleable button components.
  - Wheel move delta: An (x, y) vector where x represents horizontal wheel movement and y, vertical movement.
  - Magnify scale factor: The zoom scale that results from pinching touch gestures.
 
## Demos

## Uniforms reference

- `ivec4 uf_componentID_layout`    
    x = Component ID    
    y = Layout Type [ 0 = Tiled, 1 = Rows, 2 = Columns ]    
    z = Number of panels    
    w = (empty)    
    
- `ivec4 uf_resolution`    
    x = Panel width    
    y = Panel height    
    z = Panels area width    
    w = Panels area height    
  
- `float uf_rendering_scale`    
    openGLContext::getRenderingScale() (see JUCE docs)    
- `int uf_mouse_type`    
    0 = Move    
    1 = Enter    
    2 = Exit    
    3 = Down     
    4 = Drag    
    5 = Up    
    6 = Double Click    
    7 = Wheel Move    
    8 = Magnify    
- `vec2 uf_mouse_position`    
    x = Mouse x-position    
    y = Mouse y-position    
- `vec4 uf_mouse_click_position`    
    x = Mouse up x-position    
    y = Mouse up y-position  
    z = Mouse down x-position    
    w = Mouse down y-position  
- `vec4 uf_time`    
    x = Mouse event time    
    y = Mouse down time    
    z = Sin time    
    w = Saw time    
- `ivec4 uf_flags`    
    x = Mouse button is down    
    y = Button toggle flag    
    z = Right mouse button    
    w = (empty)    
- `vec3 uf_mouse_options`    
    x = Mouse wheel delta-x    
    y = Mouse wheel delta-y    
    z = Mouse magnify scale factor    
    
> See the shader examples for a code version of this reference.
