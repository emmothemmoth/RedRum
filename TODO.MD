* Room component with Icon, transformation arrows
* Listener Component, Audio Source Component
* Screen picking
* Editor commands, moving stuff in scene
* Drag \& Drop audio files and meshes into scene
* Serialization
* Ray casted audio with compute shaders + line trace debug drawing
* Global Play, pause, stop buttons
* Export section







**Notes**

* AudioEngine should run on its own thread
* AudioEngine handles all logic and notifies back to Listener once preview is done
* AudioListenerComponent is a transformation and Icons
* Room component is just a boundary with settings
  Settings should be:
  \* Material absorption (a number between 0-1. Rest is reflection)
  \* Material coloration (filtering)
  \* Material "roughness" (random reflection directions)
* AudioSourceComponent is the audio source. It can be played and placed within the room.
  Settings available should be:
  \* Position
  \* Rotation
  \* Volume



* All stuff in the scene can be one of three states: Ready(edited) or Pending(being edited).
  Once the state changes, the AudioEngine will update the preview. 



* As soon as anything changes in the editor, the engine will start computing the preview file (and gray out the big play button). 
