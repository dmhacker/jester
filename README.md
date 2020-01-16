# jester

[![Build Status](https://travis-ci.com/dmhacker/jester.svg?branch=master)](https://travis-ci.com/dmhacker/jester)
[![Coverage Status](https://coveralls.io/repos/github/dmhacker/jester/badge.svg?branch=master)](https://coveralls.io/github/dmhacker/jester?branch=master)

jester is an attempt at creating a strong but 
potentially unconventional AI for the card game [Durak](https://en.wikipedia.org/wiki/Durak).

## Game Mechanics

jester implements a simpler variation of traditional Durak. 
This has the effect of simplfying the game's implementation and 
search space, with improvements in bot strength and less potential for bugs. 
The differences are as follows:

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
every simulation determinizes and then follows an appropriate path down
only one tree.

Work-in-progress options are:

* Tabulated MCCFR — Monte Carlo counterfactual regret minimization 
with external sampling. The algorithm is implemented as listed on page 16
of [Richard Gibson's PhD dissertation](http://poker.cs.ualberta.ca/publications/gibson.phd.pdf). 
* Human — The human player will interface with the console and
will be controlled through stdin.

## About MCCFR

MCCFR is a reinforcement learning algorithm that learns from playing 
repeated games against itself. However, the complexity of Durak,
even with 36 cards and a simplified rule set, means that MCCFR needs 
a large amount of computational resources in order to raise its
playing strength to an acceptable level. For reference, NLTH poker bots
using MCCFR are usually trained on supercomputers, where millions of CPU
hours are invested into training. For this project, that may not be feasible.

That being said, MCCFR can quickly be trained to play optimally on an extremely 
simple version of Durak involving only 8 cards and 2 players, requiring only 
a few minutes of multi-threaded training. You can use this reduced form 
of Durak by providing the `-r` flag when running the executable. Training
mode can be enabled via the `-t` flag.

At the moment, the results of MCCFR training are saved to a local binary file 
`mccfr.bin` in the working directory. Eventually, the training will be able to
be parallelized across multiple CPUs, and tabulated information sets will be
stored in a Redis database instance provided by the user. 
