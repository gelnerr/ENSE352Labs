# MasterMind Final Project (ENSE-352)

**Name** : Glen Issac
**SID**: 200499313
**Instructor**: Cameron Oehler

# How To Play - MasterMind
This version of Mastermind is a game about trying to guess a set or a random 16 bit number with the help of 4 dip switches, storing 4 bits per guess making up a whole hexadecimal by the end of 4 guesses.  
The game gives you feedback based on your Hexadecimal guess, and lights up LEDs from one of 3 states:
* Solid LED: Correct Guess in the Correct Place
* Blinking LED: Correct Guess in the Wrong Place
* Unlit LED: Incorrect Guess

The game also uses a Black Button (PB5) to traverse from state to state and store guesses iteratively.