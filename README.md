# JaggyJiggy

JaggyJiggy is an 3D engine ispired in Unity, developed for an motors asigment by three students from CITM-UPC University from the degree of Design and development of Videogames.
With this engine you can make your own games from scratch! 
Has a family friendly interface in which a small physics engine has been developed in which you can see the collisions between different objects, forces which are applied to it, etc.

Github: <a href="https://github.com/ercanon/3D-Motor-2021">3D-Motor-2021 engine</a>
 
# Team: Chaotic Evil

![Image11](docs/raul.jpg)

Raul Cano: <a href=" https://github.com/ercanon"> 
+ Github: https://github.com/ercanon</a>
+ Role: Code

![Image12](docs/alex.png)

Alex Gesti:<a href="https://github.com/alexgesti">  
+ Github: https://github.com/alexgesti</a>
+ Role: Code

![Image13](docs/pau.png)

Pau Motta:<a href=" https://github.com/paumotta"> 
+ Github: https://github.com/paumotta</a>
+ Role: Designer.

# Task completed

## Raul Cano

+ Buffer administration 
+ Importer
+ Files Systems
+ Serialization
+ Fisics

## Alex Gesti

## Pau Motta

+ Web designer.

# Core Subsystems

## GameObjects

GameObjects are entities structured after the Entity Component Structure, they can have the following components:

+ Transform -> Represents, changes & allows to edit a gameobject position, rotation and scaling.

+ Mesh -> A 3D mesh that can be printed out to screen.

+ Material -> An image that can be applied to a mesh, using its UV channel to properly apply to the mesh.

+ Camera -> A camera that can have its values customized and used to display the scene.

+ Collider -> A collider and physics simulator that allows to change the collider shape.

## Resource Manager 

The Resource Manager is in charge of managing the GameObjects and components resources, such as the images, the 3D models, etc. If we had the same texture 2 times loaded in the scene for instance, the Resource Manager will make sure that we don’t load again.

When a user drops a file to the engine, the resource manager will load the files with information and the own format files that we need.

## UI

The UI was done using ImGui and provides an editor to use and tweak all the facilities of the engine (from an engine user perspective). The module that manages the UI does so in a way that makes it incredibly easy to create and fast.

# Assigment sub-systems

# Showcase

# How to use

- Open "JaggyJiggy.exe" to start.
- Then choose to drag and drop a FBX or choose any of our geometry.
- With that "object" created, select him and move, rotate or scale.

## Controls

- Left click in Object: Select object.
- WASD: Movement.
- Movement + SHIFT: speed x2.
- F: focus the camera around the selected object.
- Mouse wheel + Alt: Zoom.
- 1: Create and shot a ball.
- Esc: Close the engine.

# License & Copyright

MIT License

Copyright (c) 2021 Chaotic Evil

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWAR


### Fork from: https://github.com/solidajenjo/Engine3D
