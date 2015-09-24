Book Eater
==============

Imagine you have dropped a book down the local library's return chute. Naturally, it should play a sound encouraging you to come again.

Book Eater is my wife's fantastic idea (and she is the voice in the audio clips).

![RedBoard with Audio Amplifier and Sharp IR sensor](https://raw.githubusercontent.com/nseidle/Book_Eater/master/Book_Eater_Stackup.jpg "SparkFun RedBoard with amplifier and Sharp IR sensor")

The system monitors the IR sensor for a large change (delta). When/if it sees a big change it randomly plays one of 7 MP3s (never playing the same one twice in a row). There is a test routine (press 't') so you can see the output of the analog sensor.

Materials used:

* [SparkFun RedBard](https://www.sparkfun.com/products/12757)
* [Sharp IR Proximity Sensor](https://www.sparkfun.com/products/242)
* [SparkFun MP3 Shield](https://www.sparkfun.com/products/12660)
* [SparkFun ProtoShield](https://www.sparkfun.com/products/11665)
* [Arduino Header Kit](https://www.sparkfun.com/products/11417)
* [SparkFun Audio Amplifier](https://www.sparkfun.com/products/11044)
* [MicroSD Card](https://www.sparkfun.com/products/11609) (Any size up to 64GB behemoths should do)
* [10k Trimpot for volume](https://www.sparkfun.com/products/9806)
* [Couple 10k resistors](https://www.sparkfun.com/products/11508)
* Wall mountable, [marine grade waterproof speaker](http://www.amazon.com/gp/product/B001ES8X9M)
* [Headers](https://www.sparkfun.com/products/116) for various connections to proto shield 
* Some [2-pin jumper connectors](https://www.sparkfun.com/products/9914)
* Some [3-pin jumper connectors](https://www.sparkfun.com/products/9915)
* [9V Wall Adapter](https://www.sparkfun.com/products/298)
* I used some heatshrink and wire as well but I'll assume you have this...

![Close up of the protoshield with various wires and connectors](https://raw.githubusercontent.com/nseidle/Book_Eater/master/Book_Eater_Closeup.jpg)

To prep the audio amp for external control: cut the small jumper, solder in two 10k PTH resistors, and then the 10k trim pot. This will allow user controllable volume. It is *very* loud and *very* clean with a good speaker and a wall power supply. Powering the MP3 player and audio amp via USB will introduce buzzing; a 9V (or higher) wall supply should clean it up. This is because the higher voltage will allow the onboard linear voltage regulator to regulate down to a nice, clean, 5V.

The RedBoard connects to the MP3 Shield which then stacks the protoshield. The MP3 shield "-" and "R" pins are wired to the "-" and "+" pins of the "In" port of the audio amp. The out pins of the audio amp are connected to the speaker. The audio amp's S or shutdown pin is connected to A1 so that we can shut it down when not playing an MP3.

The Sharp IR sensor is powered by 5V/GND and connected to A0.

License Information
-------------------
The **code** is beerware; if you see me (or any other SparkFun employee) at the local, and you've found our code helpful, please buy us a round!

Please use, reuse, and modify these files as you see fit. Please maintain attribution to SparkFun Electronics and release anything derivative under the same license.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.
