# LiveShaderPalette

  - Live code your animated OpenGL fragment shaders.
  - Interactive graphics design made easier with ready made uniform routings for mouse/touch input events.
  - Arrange panels by rows, columns or in a tiled layout fashion for quick side-by-side comparison editing.
  - With a high number of panels in fullscreen, shader code quality can be profiled or benchmarked. 
  - Includes separate window console displaying the state of all uniforms and any OpenGL compilation errors.

#### Click "Live Compile" then edit and save your shader in an IDE and the app will display your changes.
<img src="https://github.com/ianacaburian/LiveShaderPalette/blob/master/Demos/LiveCompile.gif">    

#### Enter the number of panels and select a layout to arrange your shaders for comparison or testing.
<img src="https://github.com/ianacaburian/LiveShaderPalette/blob/master/Demos/AdjustLayout.gif">    

#### Copy .frag files into the app's "CWD" folder and click "Refresh Folder" to load them.
<img src="https://github.com/ianacaburian/LiveShaderPalette/blob/master/Demos/LoadFiles.gif">  

## Installation
  - Either download the pre-built binary or clone and build with JUCE.
## Quick start
1. Run the app.
2. Copy the shaders from this repo's ShaderExamples folder into the app's CWD folder (which upon first launch gets generated in your Documents folder).
3. Click "Refresh Folder"
## Usage
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
  
## Future
  - Build for Windows 10 (requires refactor?)
    - Replace GL:: with OpenGLContext::extensions
    - Remove explicit vertex array specification (unsupported by JUCE?)
  - Add save, load, undo, redo.
  - Load texture resources.
  - Support vertex shaders.
> Feature suggestions and pull requests welcome!
## Reference
##### Differences between OpenGL and JUCE coordinate systems.
  - JUCE coordinates: origin at top-left, y-axis ascends downwards.
  - OpenGL coordinates: origin at bottom-left, y-axis ascends upwards.
  - Mouse events and `u_resolution` are in JUCE coordinates.
  - `u_bounds` is scaled to OpenGL coordinates.
###### Tips
> - Load the MouseCube.frag in the ShaderExamples folder for the simplest demo on how to work with both coordinate systems.
> - When using multiple panels, the shader thread `st = (gl_FragCoord.xy - u_bounds.xy) / u_bounds.zw;`.
> - Normalizing JUCE position uniforms such as `u_event_position` should involve `u_resolution`, e.g. `position = u_event_position.xy / u_resolution.xy` as opposed to `u_event_position.xy / u_bounds.zw`.
  
##### Console
  - The lower area displays the OpenGL shader compiler errors.
  - Live compile rate: The rate that the app will recompile the current files.
  - Period: The rate that the sin and saw time functions use for animation speed.
  - Rendering scale: given by OpenGLContext::getRenderingScale(). Can be used to convert between JUCE coordinates and OpenGL coordinates.
  - Component ID: The ID of the panel that's associated with the current mouse event.
  - Mouse positions: Event position is the current position whereas mouse up/down position is the position since the mouse was last pressed up or down; useful for traditional click and drag component simulation.
  - Toggle flag: Simulates a toggle button; useful for designing toggleable button components.
  - Wheel move delta: An (x, y) vector where x represents horizontal wheel movement and y, vertical movement.
  - Magnify scale factor: The zoom scale that results from pinching touch gestures.
##### Uniforms

- `ivec4 u_componentID_layout`    
    x = Component ID    
    y = Layout type { 0 = Tiled, 1 = Rows, 2 = Columns }    
    z = Number of panels    
    w = (empty)    
  
- `vec4 u_bounds`    
    x = Panel x-position    
    y = Panel y-position  
    z = Panel width    
    w = Panel height    
    
- `ivec4 u_resolution`    
    x = Panel width    
    y = Panel height    
    z = Panels area width    
    w = Panels area height    
  
- `float u_rendering_scale`    
    openGLContext::getRenderingScale() (see JUCE docs)      
- `vec2 u_event_position`    
    x = Event x-position    
    y = Event y-position    
- `vec4 u_mouse_position`    
    x = Mouse up x-position    
    y = Mouse up y-position  
    z = Mouse down x-position    
    w = Mouse down y-position  
- `ivec4 u_mouse_time`    
    x = Mouse event time    
    y = Mouse up time    
    z = Mouse down time    
    w = (empty)    
- `vec2 u_periodic_time`    
    x = Sin time    
    y = Saw time    
- `ivec4 u_flags`    
    x = Mouse type    
        { 0 = Move, 1 = Enter, 2 = Exit, 3 = Down, 4 = Drag, 5 = Up, 6 = Double Click, 7 = Wheel Move, 8 = Magnify }    
    y = Mouse button is down    
    z = Button toggle flag    
    w = Right mouse button    
- `vec3 u_mouse_options`    
    x = Mouse wheel delta-x    
    y = Mouse wheel delta-y    
    z = Mouse magnify scale factor    
    
> The shader examples include this reference.
