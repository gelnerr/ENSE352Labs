# MasterMind Final Project (ENSE-352)

**Name** : Glen Issac <br>
**SID**: 200499313 <br>
**Instructor**: Cameron Oehler <br>

## How To Play - MasterMind
This version of Mastermind is a game about trying to guess a set or a random 16 bit number with the help of 4 dip switches, storing 4 bits per guess making up a whole hexadecimal by the end of 4 guesses.  
The game gives you feedback based on your Hexadecimal guess, and lights up LEDs from one of 3 states:
* **Solid LED**: Correct Guess in the Correct Place
* **Blinking LED**: Correct Guess in the Wrong Place
* **Unlit LED**: Incorrect Guess

### Components that the Circuit Uses

#### Inputs
  | Name | Port | Purpose |
  |-----|------|--------------------------------- |
  | sw0 | PB4 | Digit Input 4 |
  | sw1 | PB6 | Digit Input 3 |
  | sw2 | PB8 | Digit Input 2 |
  | sw3 | PB9 | Digit Input 1 |
  | black | PB5 | State Change |

  #### Outputs
  | Name | Port | Purpose |
  |-----|------|--------------------------------- |
  | D0 | PA0 | Led 1 bits |
  | D1 | PA1 | Led 2 bits |
  | D2 | PA4 | Led 3 bits |
  | D3 | PB0 | Led 4 bits |

### States
The game has  different states based on where you are in the game at that point in time:
* **Guess State** - Where input is read and stored into guess_real. At this point, the leds will show which digit you're on.
* **Feedback State** - This comes right after guess state, and will give you a feedback through leds based on *how* right or wrong your code is.
* **Win State** - If real_guess is equal to hc_guess (hardcoded, can implement random), you win the game, and all the leds should blink followed by a representation in hex of how many tries it took to get the guess right.
* **Lose State** - This state is entered when all guesses are exhausted and the code hasn't been guessed correctly. The lights blink in a pattern followed by a reveal of the answer.
