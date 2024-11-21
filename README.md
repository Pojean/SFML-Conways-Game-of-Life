# SFML Conway's Game of Life

Simulation rules: 
If a cell has less than 2 or more than 3 neighbours, it dies.
If it has exactly 3 neighbours and isn't alive, it comes alive.

Keybinds:
P: Pause the simulation (the simulation starts paused for the purpose of placing cells)
Left click: if the cell is empty, set it to alive, if it is alive, set it to dead
Right click hold: paint living cell trail
Middle mouse click: Set entire column to alive
Shift + Middle mouse: Set entire row to alive
Alt + Middle mouse: Set column + row to alive

Z: change framerate limit (input in command prompt)
X: resize grid (input in command prompt, should be divisible by 2)
R: reset the grid to all dead
F1: generate random noise pattern
