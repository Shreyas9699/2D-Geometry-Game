# Geometry Wars Game
This repository houses a 2D game inspired by Geometry Wars, built using the SFML library and the Entity-Component-System (ECS) design paradigm.

![Game Play](https://github.com/Shreyas9699/2D-Geometry-Game/assets/63774467/8ce7897c-e876-4ed7-9b09-dfbf233a954e)


## Project Overview
**Programming Language**: C++
**Graphics Library**: SFML (Simple and Fast Multimedia Library)
**Design Pattern**: Entity-Component-System (ECS)

## ECS Game Programming
ECS is a software desgin Paradigm.
ECS stands for - Entity, Components, Systems
+ _Entity_ : Any Object in the Game, e.g. Player, platform, tile, Bullet, Enemy etc
+ _Component_: Attriubte/properties of any entiry, e.g. Position, Speed, etc
+ _Systems_: Code/logic that drive behavior, e.g. Movement, Rendering, Sound, Physics

## Getting Started
If you intend to set up and build the project from scratch, follow the configuration instructions provided on the [SFML-VC-Config page](https://www.sfml-dev.org/tutorials/2.6/start-vc.php)
This will guide you through the process of installing and configuring SFML for use with Visual Studio C++.

## Pre-built Executable
For immediate gameplay, you can directly download the pre-built executable files (`GeometryWars.exe`) from the Windows_Executable folder within this repository. <br>
Windows_Executable : has .exe file, download this entire folder to play the game.

## Project Structure 
+ **Source Code**: Contains the C++ source files implementing various game components like Vectors, entities, components, systems, and the main game loop.
+ **Assets**: Stores game resources such as audio files (sound effects), images, and configuration files.
+ README.md: This file (you're reading it!).

## Gameplay
This section details the core gameplay mechanics of your Geometry Wars game:

#### Player:
+ Shape: An Octagon
    - Movement: Freely navigates the 2D plane using keyboard or gamepad controls (e.g., WASD for keyboard).
    - Primary Weapon: Shoots bullets upon left click. These bullets can be standard projectiles or have different effects depending on power-ups collected.
    - Special Weapon:
          - Activated by right click at regular intervals (15 sec).
          - Spams bullets from each vertices of player
+ Player will die and re-swpan upon Colloiding with the enemy
      
#### Enemies:
+ Variety of Shapes: Diverse enemy types add visual appeal and challenge.
+ Used random to to get different shapes in different colors and positions
+ Upon hitting an enemy, smaller fragments of enemies start flying out from each vertices of enemy.
+ Enemies have lifeSpan, after which they get destoyed and is same with smaller fragments of enemies

#### Scoring:
+ Points awarded for destroying enemies.
+ Double point awarded upon destroying fragments of enemies.
+ Point of each enemy is also allocated randomly.<br>

**Note**: Game can be paused and played by pressing P


![Speicla Weapon in use](https://github.com/Shreyas9699/2D-Geometry-Game/assets/63774467/ef4f528a-cd2f-4234-85d4-412ab2e3ca21)

