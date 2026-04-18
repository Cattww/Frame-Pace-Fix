## Frame Pace Fix



GD has this annoying issue where if the game lags for a moment (like when loading a level, switching startpos, or respawning), the next frame tries to catch up all that lost time at once. That’s what causes those weird stutters and physics going crazy for a split second.



This mod basically prevents that from happening.



# What it does



**Smooths out frame time so spikes don’t hit all at once**

**Gets rid of the lag at the start of a level**

**Fixes the freeze when switching startpos in practice**

**Makes respawns feel consistent instead of dropping FPS for a moment**

**Cuts down particle lag a bit on heavy levels**



# How it works



It caps how big a frame can be, then smooths things out over time instead of letting one bad frame mess everything up. 



Also forces the first few frames after a reset to stay stable so the game doesn’t freak out. 



Works in both play mode and the editor.

