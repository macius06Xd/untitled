# Dimension3D

![Release](https://img.shields.io/badge/Release-v1.0-blueviolet)
![Language](https://img.shields.io/badge/Language-C%2B%2B-0052cf)
![Size](https://img.shields.io/badge/Size-329Ko-f12222)
![Open Source](https://badges.frapsoft.com/os/v2/open-source.svg?v=103)

<br>

This repository contains the source code of a simple graphics library (2D, 3D and windows). It looks like **[SFML](https://www.sfml-dev.org/)** for 3D or **[ThreeJS](https://threejs.org/)** for C++.

<br>

<p align="center">
	<img src="https://i.imgur.com/qzPqvit.png" width="700">
</p>

<br>

# Summary

* **[Summary](#summary)**
* **[Features](#features)**
	* [Windows](#windows-)
	* [2D](#2d-)
	* [3D](#3d-)
	* [Simplified OpenGL objects](#simplified-opengl-objects-)
	* [Utils](#utils-)
* **[Install](#install)**
	* [Dependecies](#dependecies)
		* [Compilers](#compilers-)
		* [Libraries](#libraries-)
	* [Integrating into your project](#integrating-into-your-project-not-recommended) *(not recommended)*
	* [Using the skeleton project](#using-the-skeleton-project-recommended) *(recommended)*
* **[Demo](#demo)**
	* [Code example](#code-example)
	* [Release](#release)
* **[Documentation](#documentation)**
* **[Notes](#notes)**
* **[Credits](#credits)**

<br>

# Features

### Windows :

* Interactive windows with buttons, sliders... *(see **[ImGui](https://github.com/ocornut/imgui)** for more information)*

* Scene windows for display

### 2D :

* Rectangles, circles, custom shapes, images... *(see **[SFML](https://www.sfml-dev.org/)** for more information)* on a scene or the main window

* A drag controller to move the view with the mouse

### 3D :

* Cubes, spheres, cylinders, cones, custom meshes...

* Phong materials, Textures...

* Ambient, directional and point lights

* Perspective and orthographic cameras

* Drag, fly and orbit controllers

### Simplified OpenGL objects :

* Shaders, framebuffers, vertexbuffers, textures...

### Utils :

* Colors, vectors, small functions...

<br>

# Install

### Dependecies

* #### Compilers and softwares :
	* **[CMake](https://cmake.org/)**
	* **[GCC](https://gcc.gnu.org/)** for **Linux**
	* **[Clang](https://clang.llvm.org/)** for **MacOS**
	* **[Visual Studio](https://visualstudio.microsoft.com/)** (the software or the compiler only) for **Windows** *(recommended)*
	* **[MinGW](https://www.mingw-w64.org/)** for **Windows** *(not recommended)*

* #### Libraries :
	* **[GLM](http://glm.g-truc.net/0.9.6/api/index.html)**
	* **[GLEW](http://glew.sourceforge.net/)**
	* **[SFML](https://www.sfml-dev.org/)**
	* **[ImGui](https://github.com/ocornut/imgui)**
	* **[ImGui-SFML](https://github.com/eliasdaler/imgui-sfml)**

### Integrating into your project *(not recommended)*

Link all the **[dependecies](#dependecies)** to your project and compile the **Dimension3D library** as if it was part of your project.

### Using the skeleton project *(recommended)*

Download the **[skeleton project](https://github.com/angeluriot/Graphics_skeleton)**, delete the **[main.cpp](https://github.com/angeluriot/Graphics_skeleton/blob/master/sources/main.cpp)** demo and put your project files in the **[sources folder](https://github.com/angeluriot/Graphics_skeleton/tree/master/sources)**.

Then follow the instructions of the **skeleton project** **[here](https://github.com/angeluriot/Graphics_skeleton#install)**.

<br>

# Demo

### Code example

```cpp
#include <dim/dimension3D.hpp>

int main()
{
    // Open the main window and initialize the libraries
    dim::Window::open("App", 0.75f);

    // Create the scenes
    dim::Scene scene("Scene");

    // Cameras and controllers
    scene.set_controller(dim::OrbitController());

    dim::PerspectiveCamera cam;
    cam.set_position(dim::Vector3(0.f, 0.f, 3.f));
    scene.set_camera(cam);

    // Add lights
    scene.add_light(dim::DirectionalLight(dim::Vector3(-1.f, -1.f, -1.f)));

    // Create objects
    dim::Object object_1(dim::Mesh::Sphere(256, 256), dim::Material(dim::Color(1.f, 0.1f, 0.1f), 0.1f, 0.5f, 0.6f, 30.f));

    dim::Object object_2(dim::Mesh::Cone(256), dim::Material(dim::Color(0.1f, 1.f, 0.1f), 0.1f, 0.5f, 0.6f, 30.f));
    object_2.move(dim::Vector3(1.1f, 0.f, 0.f));

    dim::Object object_3(dim::Mesh::Cylinder(256), dim::Material(dim::Color(0.1f, 0.1f, 1.f), 0.1f, 0.5f, 0.6f, 30.f));
    object_3.move(dim::Vector3(-1.1f, 0.f, 0.f));

    // The example button color
    float color[4] = { 1.f, 1.f, 1.f, 1.f };

    // Main loop
    while (dim::Window::running)
    {
        // Check events
        sf::Event sf_event;
        while (dim::Window::poll_event(sf_event))
        {
            dim::Window::check_events(sf_event);
            scene.check_events(sf_event);
        }

        // Clear the screen
        dim::Window::clear();
        scene.clear();

        // Update interactive stuff
        dim::Window::update();
        scene.update();

        // Draw the objects
        scene.draw(object_1);
        scene.draw(object_2);
        scene.draw(object_3);

        // Display the scenes to the window
        scene.display();

        // The ImGui window and button example
        ImGui::Begin("Menu");
        ImGui::Button("Button");
        ImGui::ColorPicker3("Color picker", color, ImGuiColorEditFlags_PickerHueWheel);
        ImGui::End();

        // Display the window to the screen
        dim::Window::display();
    }

    // Close the main window and shut down the libraries
    dim::shut_down();
    return EXIT_SUCCESS;
}
```

### Release

You can also see the **[releases](https://github.com/angeluriot/Graphics_skeleton/releases)** of the **[skeleton project](https://github.com/angeluriot/Graphics_skeleton)** to test the example code without compiling.

<br>

# Documentation

You can read the documentation by consulting the **[wiki](https://github.com/angeluriot/Dimension3D/wiki)**.

<br>

# Notes

* This is a small project of a single beginner programmer so there may be bugs if you do not use the library as indicated

<br>

# Credits

* [**Angel Uriot**](https://github.com/angeluriot) : Creator of the library.
