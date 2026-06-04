# Card Game

Recreation of the **Blade** mini-game from *The Legend of Heroes: Trails of Cold Steel*.

Built in **C++20** using **SDL3** as a learning project.

## Features

* Turn-based card gameplay inspired by **Blade**
* Multiple special card mechanics
* AI opponent
* Sound effects and responsive UI
* Event-driven architecture

## Game Rules

Players take turns placing cards on the board. The objective is to finish a turn with a higher score than the opponent.

Losing conditions:

* your score drops below the opponent's
* you run out of cards

### Card Types

**Number Cards**
Regular cards with values from 1 to 7.

**Flip**
Swaps both players' fields.

**Strike**
Removes the opponent's last played card.

**Double**
Doubles your current score.

**Snatch**
Removes a random card from the opponent's hand.

### Additional Rules

**Card "1"**
Can be played normally or used to cancel an opponent's Strike card.

**Tie Rule**
If both players end with the same score, the board is cleared and a new round starts.

**Foul Rule**
Ending the game with a special card results in an immediate loss.

## Dependencies

The project requires:

* **C++20**
* **SDL3**
* `SDL3_image`
* `SDL3_ttf`
* `SDL3_mixer`

## License

This is a non-commercial fan project created for educational purposes.

All rights to the original **Blade** mini-game belong to **Nihon Falcom**.
