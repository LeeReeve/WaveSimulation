# WaveSimulation
A 2 dimensional simulation of a platform loaded with solar panels floating on ocean waves.
So far, the simulation is intended to test the stability of the platform when floating on waves, to test what amplitudes and wavelengths different platforms can cope with (with variable dimensions, and variable amount of solar panels), with the platform having 'failed' if the water reaches above the top of the platform.

This simulation uses GLFW and GLAD.
GLFW: https://www.glfw.org/index.html
GLAD: https://glad.dav1d.de/
The required includes and libraries can be found in the Include and Libs files. Also, opengl32.lib and glfw3.lib must be linked with the project.

To improve the simulation, some of the next steps are as follows:
1. Make sure that the shaders are included in the build. This issue does not currently impact how the program works in any significant way, but may become problematic in future.
2. Make the simulation check for failure automatically.
3. Ensure that the simulation still runs accurately if water does reach above the platform, as it is the method of calculating the forces does not check for this since in this case the platform will have 'failed' anyway, but this could prove useful for other purposes.
4. Extend the simulation to 3 dimensions.
5. Make it easy to change the shape or configuration of solar panels for the platform being tested: as it is the platform is a cuboid, and the panels are modelled as a uniform lamina on top of it. This is because in practise these things could further optimise the platform design, so it is important to test them.
6. Allow variables to be changed during or before the simulation, without editing those variables in the C++.
