# OmegaStay - The unknown brother of AlphaGo

## Background

All chess engines work by searching the game tree for the best move. There are two main approaches:

1. Minimax search with alpha-beta pruning (e.g. Stockfish)
2. Monte Carlo tree search (e.g. Leela Chess Zero)

With minimax search, we can never search till the end of the game. Hence, an evaluation function must be used to estimate the value of a position. This can be a handcrafted function or a neural network (NNUE).

However, with pure Monte Carlo tree search, we can simulate the game till the end and get the exact value of the position. Hence, an evaluation function is not required (although it can be used to focus searching on better moves).

## Approach

- Use Monte Carlo tree search
- No limit on running time. Output every time a new best move is found.
- Evaluation strategy can be one of the following (in decreasing order of implementation priority):
  - Simulate till the end of the game and get the exact value of the position
  - Use a handcraft evaluation function
  - Efficiently updatable neural network (NNUE)

## References

### Monte Carlo tree search

- https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
- https://web.archive.org/web/20160308043415/http://mcts.ai:80/index.html
- https://int8.io/monte-carlo-tree-search-beginners-guide
- https://www.geeksforgeeks.org/ml-monte-carlo-tree-search-mcts
- http://tim.hibal.org/blog/alpha-zero-how-and-why-it-works
- https://github.com/AdamStelmaszczyk/gtsa
- https://github.com/Konijnendijk/cpp-mcts

### NNUE

- https://en.wikipedia.org/wiki/Efficiently_updatable_neural_network
- https://github.com/glinscott/nnue-pytorch/blob/master/docs/nnue.md

### Random number generators

Yes, we need to generate so many random numbers that `mt19937` is not fast enough.

- https://prng.di.unimi.it/
- https://www.pcg-random.org/posts/bounded-rands.html
