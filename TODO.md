# TODO log

Todo will be updated periodically, checked items will be removed and report will be added to the bottom.
## TODO
- [x] Better sorting algorithm in move sort, currently it's O(n^2)
- [ ] Consider changing history move heuristic.
- [ ] Optimize PV move sorting and move scoring in general, probably should score moves while generating. 
- [ ] Improve late move reduction requirements
- [x] Add endgame detection (piece value? Piece count?)
- [ ] Add restriction on null move pruning. Stop null move pruning when entering end game (more complicated than previously thought)
- [ ] Null move pruning allows us to prune a lot of nodes, even though they are unsafe in end game due to zugzwang, removing it in endgame completely, proves to not be ideal as it reduces the engine's ability to calculate deep depth drastically. Add more restrictions on Null move pruning and see which one is better balanced. 
- [ ] Experiment with allowing consequtive null-moves (2 moves).
- [ ] Instead of resetting aspiration window, test gradually expanding it. 
- [ ] Change the time control UCI code to more modern c++, possibility make it less os dependent.
- [ ] Implement more search variant too see if they offer improvement
- [ ] Look into more hash options than Zorist
- [ ] Refactor the code and organize
- [ ] Help change the probe nnue library to use the newest stock fish NNUE
- [ ] Scale the NNUE score to improve margin based pruning
- [ ] Fail soft PVS causes some search instablility problems related to pruning and and margine based reduction. We might want to reviews these optimizations.  
- [ ] Implemented SEE, now we need to apply it to move ordering and quiescence search.
- [ ] Improbe SEE to see pins, check, etc

## 10/19/2022
- Changed C-stlye arrays to std::arrays, tested speed change. All std::array seems to not effect the speed by much as expected. Moving forward all c-style array will be defined as std::arrays. 
