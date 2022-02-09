# wordle-bot

A simple [wordle](https://www.powerlanguage.co.uk/wordle/) AI which attempts to greedily minimize the amount of remaining possible words. It never loses and has an average score of 3.47.

### Commands
Available commands:
* `benchmark`         Run the bot on all possible answers and find the average score.
* `simulate [word]`   Simulate running the bot on a word. If a word is not provided it is picked randomly.
* `play`              Run the bot by providing feedback on the command line.
* `help`              Print this help sheet.
