# Escape Protocol

## Game Description

**Escape Protocol** is a 2D side-scrolling jetpack-runner game created using the **iGraphics** library in C/C++. The player flies through three levels using a jetpack, dodging obstacles, collecting fuel and coins, and (from Level 2 onward) shooting obstacles and drones with a gun. The project demonstrates real-time graphics programming concepts like collision detection, animation, user input handling, a save/load system, and a scripted drone boss fight.

## Features
- Three levels with increasing difficulty (700 m, 500 m and 400 m).
- Jetpack flight physics with running, ascending and falling animations.
- Obstacles with different hitbox types, coins, fuel packs, shield and magnet power-ups.
- Gun with limited ammo, destructible obstacles and rocket hazards (Level 2 and 3).
- Level 3 drone encounters: Drone Gauntlet, Rocket Drone Intrusion and the Final Drone Fight with health bars.
- Bonus coin-collection phase after the main run (Level 2 and 3).
- Player profiles with save/load system and ranking (savegame.txt).
- Main menu, level select, pause menu and background music / sound effects.



## Project Details
IDE: Visual studio 2010/2013

Language: C,C++.

Platform : Windows PC.

Genre : 2D action runner


## How to Run the Project

Make sure you have the following installed:
- **Visual Studio 2013**
- **MinGW Compiler** (if needed)
- **iGraphics Library** (included in this repository)


Open the project in Visual Studio 2013
- Open Visual Studio 2013.
- Go to File → Open → Project/Solution.
- Locate and select the .sln file from the cloned repository.
- Click Build → Build Solution
- Run the program by clicking Debug → Start Without Debugging


## How to Play

### **Controls**
| Action | Keyboard | Mouse |
|--------|----------|-------|
| **Fly up (jetpack)** | `Space` (hold) | `Left Click` (hold) |
| **Fall** | Release `Space` | Release `Left Click` |
| **Shoot (Level 2 and 3)** | - | `Left Click` (hold for auto-fire) |
| **Pause / Back** | `Esc` | - |
| **Resume / Confirm** | `Enter` | - |
| **Menu navigation** | `↑` `↓` and `Enter` | Hover and click |
| **Show hitboxes (debug)** | `H` | - |


### **Game Rules**

- Reach the end of each level: Level 1 = 700 m, Level 2 = 500 m, Level 3 = 400 m.
- Flying drains fuel. Collect fuel packs (+35 fuel) to keep flying.
- Hitting an obstacle or rocket ends the run, unless a shield is active.
- Level 2 and 3: the obstacle with the special mark drains half your fuel instead of killing you.
- Shield: protects you for 20 m and appears every 100 m.
- Magnet (Level 3): automatically pulls coins and fuel for 5 seconds.
- Ammo: each pickup gives +10 bullets (max 20 in Level 2, max 30 in Level 3). Some obstacles can be destroyed by shooting them.
- Level 3 Drone Gauntlet (300 m left): dodge two drones for 30 seconds. Each bullet hit takes 20 health (100 total).
- Level 3 Rocket Drone (100 m left): a drone fires rockets that kill instantly, shoot each rocket twice to destroy it.
- Final Fight: destroy both drones (30 bullet hits each) to win. Ammo pickups give +30 (max 50).
- Complete a level to unlock the next one and save your score.


## Project Contributors

1. Md. Fatmi Ahsan Shoeb (00725105101009)
2. Mohammad Jahirul Islam (00725105101014)
3. Promit Biswas Deb (00725105101021)


## Screenshots

### **Menu**
<img src="" width="200" height="200">

### **Level 1**
<img src="" width="200" height="200">

### **Level 2 (Bonus Phase)**
<img src="" width="200" height="200">

### **Level 3 (Final Fight)**
<img src="" width="200" height="200">

## Youtube Link
[CSE 1200 Project: Escape Protocol](https://www.youtube.com/)

## Project Report
[Project Report: Escape Protocol](https://drive.google.com/drive/u/1/my-drive)
