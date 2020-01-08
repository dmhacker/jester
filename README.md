# jester

jester is an attempt at creating a strong but 
potentially unconventional AI for the card game [Durak](https://en.wikipedia.org/wiki/Durak).

## Game Mechanics

The complete rules for Durak will not be listed here, since
they are readily available elsewhere. However, jester implements
a reduced form of traditional Durak. This has the effect of both
simplfying the game's implementation and search space, leading
to improvements in strength and less potential for bugs. The
differences are as follows:

* A deck of 36 playing cards is used instead of 52. Cards with
ranks 2 through 5 are not used. This is usually normal.
* Only one attacker and defender are allowed at a time. Other
players are not allowed to join in an attack.
* Players cannot divert an attack by playing a card of the same 
rank (e.g. they cannot pass the buck).
* Only one card can be played at a time. 

All gameplay is currently conducted through the console. 
There are no plans to add a GUI.

## Strategic Choices

jester's game-playing algorithm of choice is 
[Monte Carlo tree search](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search), 
specifically variants such as determinized and 
information-set MCTS that have been adapted
for games with imperfect information. 

In short, jester implements the following options for players:

* Minimal — The minimal player will only play a card on the
on the first attack. On defense, they will always yield. 
A minimal player represents the worst possible
strategy and will always lose, even against random play.
* Random — If it is a random player's turn, they will select
a valid, random action with no bias.
* Greedy — A greedy player is similar to a random player.
However, a greedy player will always play a random card if it can. 
* Determinized MCTS — A player implementing Monte Carlo tree
search with explicit determinzations. That is, the player will
fill in incomplete game information with several potential 
determinizations and then run MCTS on each state.
* ISMCTS — A player implementing 
[information-set Monte Carlo tree search](http://eprints.whiterose.ac.uk/75048/1/CowlingPowleyWhitehouse2012.pdf). 
Rather than associating a single determinization with an entire MCTS tree,
every simulation determinizes and then follows an appropriate path
down only one tree.

Work-in-progress options are:

* Human — The human player will interface with the console and
will be controlled through stdin.
* Smart — The smart player will follow a pre-defined strategy. The
smart player will try to perform prudent actions, such as saving
trump and high cards for later and attacking with low cards 
early and midgame.
* Deep Q-learning — Reinforcement learning agent only available for 
2-player games.
