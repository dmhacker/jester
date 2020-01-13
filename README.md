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

Players in jester behave according to several pre-defined options.
When running the game engine, you can choose which players to 
instantiate in your games.

jester implements the following player types:

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
every simulation determinizes and then follows an appropriate path. 
Multiple trees can be compressed into one.
* Tabulated CFRM — Counterfactual regret minimization with Monte Carlo 
external sampling. The algorithm is implemented as listed in page 16
of [Richard Gibson's PhD dissertation](http://poker.cs.ualberta.ca/publications/gibson.phd.pdf). 

Work-in-progress options are:

* Human — The human player will interface with the console and
will be controlled through stdin.
* Smart — The smart player will follow a pre-defined strategy. The
smart player will try to perform prudent actions, such as saving
trump and high cards for later and attacking with low cards 
early and midgame.

## About the CFRM Algorithm

We choose to focus only on the two-player version of Durak for CFRM, although 
it can be extended to multiple players.

There is an extreme amount of training required to get CFRM working,
which jester implements using the `-t` flag. Additionally, we use
the tabulated form of CFRM, with minimal abstraction on top of existing
information sets. This means that, in addition to the large amount of
CPU time required to get the bot working, the CFRM algorithm also
requires a large amount of disk space & memory to store all possible 
information sets in the game, which limits its applicability. 

Despite the resource limitations, CFRM can quickly be trained to play an 
extremely simple version of Durak involving only 8-12 cards and 2 players 
(requiring only a few minutes of multi-threaded training on my 8-core 4.2GHz 
laptop processor). You can use this reduced form of Durak by providing the 
`-r` flag.

At the moment, the results of CFRM training are saved to a local binary file 
`cfrm.bin` in the working directory. Eventually, the training will be able to
be parallelized across multiple CPUs, and tabulated information sets will be
stored in a Redis database instance provided by the user. 
