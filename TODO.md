# Change log

Todo will be updated periodically, checked items will be removed and report will be added to the bottom.
## TODO
- [ ] Better sorting algorithm in move sort, currently it's O(n^2)
- [ ] Consider changing history move heuristic.
- [ ] Optimize PV move sorting and move scoring in general, probably should score moves while generating. 

## 10/19/2022
- Changed C-stlye arrays to std::arrays, tested speed change. All std::array seems to not effect the speed by much as expected. Moving forward all c-style array will be defined as std::arrays. 
