# WaveSimulation
A 2 dimensional simulation of a platform loaded with solar panels floating on ocean waves.
So far, the simulation is intended to test the stability of the platform when floating on waves, to test what amplitudes and wavelengths different platforms can cope with (with variable dimensions, and variable amount of solar panels), with the platform having 'failed' if the water reaches above the top of the platform.

This simulation uses GLFW and GLAD.
GLFW: https://www.glfw.org/index.html
GLAD: https://glad.dav1d.de/
The required includes and libraries can be found in the Include and Libs files. Also, opengl32.lib and glfw3.lib must be linked with the project.

To improve the simulation, some of the next steps are as follows:
1. Make sure that the shaders are included in the build. This issue does not currently impact how the program works in any significant way, but may become problematic in future.
2. Ensure that the simulation still runs accurately if water does reach above the platform, as it is the method of calculating the forces does not check for this since in this case the platform will have 'failed' anyway, but this could prove useful for other purposes.
3. Make the simulation check for failure automatically.
4. Alter the calculations to apply to the platform designed by HelioRec, and ensure that it is clearly visible in 3D.
5. Change the force and couple calculations such that they keep working once the top of the platform is underwater, or the platform has been inverted. These were not taken into account initially since once the platform had gone underwater it was considered to have ‘failed’. However, having the simulation be able to cope with these situations could prove useful in future.
6. Alter the force calculations, and rendering, to work for waves not travelling perpendicular to a principal moment of inertia of the platform.
7. Take into account horizontal as well as vertical forces.
8. Make it easy to change wave and platform parameters whilst running the program.
9. Make it easy to change the shape or configuration of solar panels for the platform being tested: as it is the platform is a cuboid, and the panels are modelled as a uniform lamina on top of it. This is because in practise these things could further optimise the platform design, so it is important to test them.
10. Alter the method used for interpolating between time steps for increased accuracy/efficiency.
11. Output data directly into a file for analysis using, for example, MATLAB or Excel.




