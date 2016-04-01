# fs-sound
Sound sampling studies, create original wave sound with pure PHP

## Installation on Ubuntu
Download the fs-sound zip file
Extract the zip in the directory ycdou want to record your wave files
Make an alias in your .bash_aliases
```
alias='/my/audio/directory/fs-sound/src/./fs-sound.php'
```
Then fs-sound.php file must be "chmod" to be executed
```
cd /my/audio/directory/fs-sound/src
chmod+x fs-sound.php
```
## Usage
```
fs-sound <filename> <frequence> <duration>
```
Example :
```
fs-sound audio.wav 440.0 5.0
```
That will generate a sound with 440Hz frequency during 5 seconds record in a audio.wav audio file  
You can verify you file after create your audio sound with
```
cd /my/audio/directory/fs-sound/src
file audio.wav
```

## Listen the sound

On ubuntu you sox to play the created sound
```
sudo apt-get install sox
```
Then
```
cd /my/audio/directory/fs-sound/src
play audio.wav
```
