# LiveShaderPalette

  - Live code your animated OpenGL fragment shaders.
  - Interactive graphics design made easier with ready made uniform routings for mouse/touch input events.
  - Arrange panels by rows, columns or in a tiled layout fashion for quick side-by-side comparison editing.
  - With a high number of panels in fullscreen, shader code quality can be profiled or benchmarked. 
  - Includes separate window console displaying the state of all uniforms and any OpenGL compilation errors.

## Installation
  - Either download and run one of the pre-built binaries or clone and build with JUCE.
## Quick start
1. Run the app.
2. Copy the shaders from this repo's ShaderExamples folder into the app's CWD folder (which upon first launch gets generated in your Documents folder).
3. Click "Refresh Folder"
## How to use
##### Load files via folder  
  - Click the "Refresh folder" button to reload all panels using the shader files currently in the app's "CWD" folder.
>Fragment file extension must be ".frag".   
##### Load files via drag-and-drop
  - To load a single file into all panels, drag onto the tool bar area.
  - To load into a single panel, drag onto any panel. 
##### Live compile
  - After loading a shader file into a panel, open it in an IDE then click the "Live Compile" button to display any changes that are saved while editing in the IDE (see Demos).
>Mouse/touch input is uninterrupted when Live Compile is disabled.
##### Panel Layout
  - Choose a layout by clicking either "Tiled", "Rows", or "Columns". Tiled only accepts round square root integers; Rows and Columns accepts a minimum of 2 panels.
  - Enter the number of panels to display in the text field at the top left.
 
## Demos
![](https://github.com/ianacaburian/LiveShaderPalette/blob/master/Demos/LiveCompiling.gif =200x)    
![](https://github.com/ianacaburian/LiveShaderPalette/blob/master/Demos/Layout.gif)    
![](https://github.com/ianacaburian/LiveShaderPalette/blob/master/Demos/FileLoading.gif)    
## Future
  - Add save, load, undo, redo.
  - Load texture resources.
  - Support vertex shaders.
> Pull requests welcome!
## Reference
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
##### Uniforms

- `ivec4 uf_componentID_layout`    
    x = Component ID    
    y = Layout type { 0 = Tiled, 1 = Rows, 2 = Columns }    
    z = Number of panels    
    w = (empty)    
    
- `ivec4 uf_resolution`    
    x = Panel width    
    y = Panel height    
    z = Panels area width    
    w = Panels area height    
  
- `float uf_rendering_scale`    
    openGLContext::getRenderingScale() (see JUCE docs)      
- `vec2 uf_event_position`    
    x = Event x-position    
    y = Event y-position    
- `vec4 uf_mouse_position`    
    x = Mouse up x-position    
    y = Mouse up y-position  
    z = Mouse down x-position    
    w = Mouse down y-position  
- `ivec4 uf_mouse_time`    
    x = Mouse event time    
    y = Mouse up time    
    z = Mouse down time    
    w = (empty)    
- `vec2 uf_periodic_time`    
    x = Sin time    
    y = Saw time    
- `ivec4 uf_flags`    
    x = Mouse type    
        { 0 = Move, 1 = Enter, 2 = Exit, 3 = Down, 4 = Drag, 5 = Up, 6 = Double Click, 7 = Wheel Move, 8 = Magnify }    
    y = Mouse button is down    
    z = Button toggle flag    
    w = Right mouse button    
- `vec3 uf_mouse_options`    
    x = Mouse wheel delta-x    
    y = Mouse wheel delta-y    
    z = Mouse magnify scale factor    
    
> The shader examples include this reference.
