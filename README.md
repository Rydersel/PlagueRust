
# DISCLAIMER: 
**I am not responsible for anything done with this code. It is for educational purposes only. I do not condone cheating in video games, 
all code has been submitted to EAC after the study concluded.**


# PlagueRust - External Recoil Cheat for Rust

Decided to make this code public because its a year old.

**What does it mean that the program is external?**
```
This means that the program runs completely independently from the game,
never hooking into the game process or editing any of its memory.
```
This code is part of a study done on Rust's Anti-Cheat. More info can be found here: ([Study On Anti-Cheats](https://github.com/Rydersel/StudyOnAnticheats/wiki))

## Description

Rust is a multiplayer survival video game developed by Facepunch Studios. The objective of Rust is to survive in the wilderness using gathered or stolen materials. Players must successfully manage their hunger, thirst, and health, or risk dying. Despite the presence of hostile animals such as bears and wolves, the primary threat to the player is other players due to the game being solely multiplayer. (Via Wikipedia).

There are a variety of different weapons in Rust from Assault rifles like the AK47 to small pistols like the revolver. Each of these weapons has a (mostly) fixed recoil spread which raises the overall skill ceiling of the game. Many fights in the game are decided by which player is better able to control said recoil. 

On a base level, the goal of this program is to simply automatically adjust for each individual weapon's recoil making each weapon fire in a perfectly straight line. As you will see though this is a lot easier said than done.

## Main Challenges:

**-Evading detection by the game's Anti-Cheat(EAC)**

**-Adjusting for randomness caused by weapon spread**

**-Scailing recoil patterns based on mouse placement and screen resolution**

**-Adjusting recoil pattern depending on bullets in clip**


## Features


### **Main Features:**
```
-Recoil Control for all weapons
-Variable X and Y recoil randomization
-Support for all weapon attachnments
-Auto Weapon Detection
-Auto Ammo Detection
-Fully customizable keybinds for all functions
```
### **Misc Features:**
```
-Auto Smelt (Small and Large Furnaces)
-Auto Codelock
-Auto Upgrade
-Anti AFK
-Full UI Customization
-Automatic Firing for Semi-Automatic weapons
-5 Different Crosshair Overlays
-Custom Firing Sounds
```
### **Work In Progress Features:**
```
-External Aimbot (Detects enemy positions based on a trained nural net model)
-Player highlighting (Places a bright box around enemies to make them easier to spot in darker enviroments)
```





