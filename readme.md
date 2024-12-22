# CSCI420 Programming Assignment 2: Simulating a Roller Coaster

**Assignment Link:** [Simulating a Roller Coaster](https://odedstein.com/teaching/hs-2024-csci-420/assign2/)

**Demo Video 1:** [magicmountain.sp](https://youtu.be/GnHh8fWVEcY)

**Demo Video 2:** [circular.sp](https://youtu.be/dBucBFpXcys)

**Final Grade:** 120 out of 100

## Introduction

This assignment utilizes Catmull-Rom splines together with OpenGL's core profile shader-based lighting and texture mapping to simulate a roller coaster. Two shaders were implemented: one for texture mapping to render the background, and another for Phong shading to render the roller coaster rails. The simulation provides a first-person view, offering an immersive experience as if riding the coaster.

## Potential Improvements

Although this section typically appears at the end of a README file, I want to emphasize it upfront as I submit my assignment. This project proved challenging as the more functions I implemented, the more I realized the potential for enhancements. Compared to the first assignment, this open-ended project could vastly expand to mimic the real world. I encountered numerous issues, often uncertain which to address first. What became clear is that understanding potential improvements is as valuable as implementing prescribed functionalities. Here are areas for future development:

### Rails
- **Rail Collision:** Implement collision handling for the rails.
- **Rail Color:** Currently, rail color is determined solely by Phong shading. Ideally, there should be a parameter to set the basic color of the rails, which would then be modified by Phong shading.
- **Bezier Splines:** Although Catmull-Rom splines facilitate control point manipulation and are straightforward to implement, using Bezier splines might offer better handling of circular paths, which are common in roller coasters.

### Pillars
- **Pillar Shapes:** Real-world roller coasters feature various pillar shapes even within a single design.
- **Pillar Collision:** Implement collision avoidance between pillars and rails.

### SP File
- **File Readability:** Enhance the SP file format to support comments for better readability.
- **Control Points Management:** Instead of manually entering control points in the SP file, implement helper functions in a separate C++ file for dynamic adjustment and collision management.

### Camera
- **Bird View:** Expand the implementation to offer multiple bird's-eye views of the coaster.
- **Moving Speed:** Allow for variable speeds rather than a constant speed setting.
- **Interactive Controls:** Enable camera orientation adjustments (e.g., turning left or right) via keyboard controls during the ride.

## Controls

### Keyboard Commands
- **esc:** Exit the simulation.
- **x:** Take a screenshot.
- **c:** Capture 900 consecutive screenshots.
- **b:** Toggle bird view.
- **+-:** Zoom in camera in bird view.
- **+-:** Zoom out camera in bird view.
- **w:** Increase the coaster's speed.
- **s:** Decrease the coaster's speed.

## Usage

To run the program, use the following command:
```bash
./hw1 <spline file>
```

Examples:
```bash
./hw1 splines/magicmountain.sp
./hw1 splines/circular.sp
./hw1 splines/circular2.sp
./hw1 splines/rollerCoaster.sp
```

## Additional Implementations
- **Double Rails with Ties:** Render a double rail with ties in between.
- **Pillar Rendering:** Pillars occur beneath ties but less frequently.
- **Circular Rail, Tie, and Pillar:** All rail, tie, and pillar structures are rendered with a circular cross-section instead of the traditional rectangular shape, enhancing the visual aesthetics.
- **Circular Track:** `circular.sp` and `circular2.sp` create circular tracks that closes with C1 continuity.
- **Real-World Mimicry:** `magicmountain.sp` mimics real-world roller coasters, similar to Magic Mountain.
- **Skybox:** A rendered skybox enhances the immersive experience.
- **Manual Speed Control:** Users can manually control the coaster's speed using the keyboard.
- **Automated Screenshot Capture:** The system is capable of capturing up to 900 consecutive screenshots to document the entire ride.

### Helper Files
- **Math Operations:** Added `mathOperation` helper file which includes a Catmull-Rom spline matrix and basic operations like normalize, crossProduct, multiplyMatrices, computeTangent, computeBinormal, and computeNormal.
- **Pipeline Program Enhancements:** Modified `pipelineProgram` helper file to include additional functions such as `SetUniformVariableMatrix3fv`, `SetUniformVariable3fv`, `SetUniformVariable3f`, and `SetUniformVariable4f`, enhancing uniform variable handling.


## Sample Images
- Ride view of magicmountain.sp
![sample image](<hw1/sample images/screenshot.jpg>)
- Bird view of magicmountain.sp
![sample image](<hw1/sample images/screenshot2.jpg>)
- Milestone image 1
![mode 3 with color](<hw1/sample images/Assignment 2 Milestone.jpg>)
- Milestone image 2
![mode 3 with color](<hw1/sample images/Assignment 2 Milestone_2.jpg>)