# hair
By Mike Ravella, Brandon Montell, and Andrew DiMarco

[Github Repo](https://github.com/mravella/hair)
[Presentation](https://docs.google.com/presentation/d/1gfVZSzJfieSUaOHV9Q5lMgny7vGcaSve-XR9sljlIDI/edit?usp=sharing)

For our project we simulated and rendered hair in real-time. Features are listed below:

### Simulation
- Wind and gravitational forces
- Hair collisions with mesh
- Collisions and friction between hair

### Generating geometry
- Hairs are generated on the GPU from a small number of simulated guide hairs
- Hairs are turned into billboarded triangle strips
- Many OpenGL techniques used, including tessellation shader, geometry shader, and transform feedback

### Rendering
- Shadows using shadow mapping with percentage-closer filtering
- Shadows using deep opacity mapping
- Transparency using depth peeling
- Anti-aliasing using 4-sample supersampling-antialiasing
- Color variation between hairs

### User interface
- Interactive adjustment of many parameters
- Scene editor that allows user to draw custom hair placement and combing direction