* Logging System -> use the current one?
* Juce integration
* Editor, screen picking ->
* Serialization
* Compute shaders







**Notes**

* An AudioListenerComponent will handle the preview of the room
* AudioSourceComponent is just the audio source. It can be played and placed within the room. 
  Settings available should be:
  \* Position
  \* Rotation
  \* Volume
* As soon as the audio source is changed, the engine will start computing the preview file (and gray out the big play button).
  This won't happen every frame but rather when the user stops editing.
