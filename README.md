# LiveShaderPalette

Run your interactive animated OpenGL fragment shaders while editing.

  - "Live compile" allows the shader file to be recompiled at an adjustable rate (0.5 to 10sec) to streamline quick shader editing.
  - Interactive mouse/touch input animation via juce::MouseListener interface.
  - Drag and drop shader files directly onto panels for side-by-side comparison editing.
  - Arrange panels by rows, columns or in a tiled layout fashion.
  - Benchmark shader performance with the ms/frame display when using a large number of panels.
  - Separate window console displays the state of the fragment's uniforms for easy debugging.

## Installation
  - Either download and run one of the pre-built binaries or clone and build with JUCE.
## How to use
##### Panel Layout
  - Enter the number of panels to display at the top left.
##### Load files via drag-and-drop
  - To load a single file into all panels, drag onto the tool bar area.
  - To load into a single panel, drag onto any panel.
##### Load files via folder
  - Fragment files are loaded from the "LiveShaderPalette/CWD" folder located in your User Documents folder (this will be automatically created on first launch).    
  - Click the "Refresh folder" button to reload all panels using the files currently in "CWD".
>Fragment files must end with a ".frag" file extension.    
##### Console
  - The lower area displays the OpenGL shader compiler errors.
  - Live compile rate: The rate that the app will recompile the current files.
  - Period: The rate that the sin and saw time functions use for animation speed.
  - Rendering scale: All console coordinates (screen/panel size and mouse positions) are given in JUCE screen coordinates; multiply these by the renderingScale if OpenGL coordinates are required.
  - Component ID: The ID of the panel that's associated with the current mouse event.
  - Mouse positions: Mouse position is the current position whereas mouse down position is the position since the mouse was last pressed down; useful for drag events.
  - Toggle flag: Simulates a toggle button; useful for designing fragment shaders for toggleable button components.
  - Wheel move delta: An (x, y) vector where x represents horizontal wheel movement and y, vertical movement.
  - Magnify scale factor: The zoom scale that results from pinching touch gestures.
 
