#ifndef nnue_eval_hpp
#define nnue_eval_hpp

/* NNUE wrapper function headers */
void init_nnue(char *filename);
int evaluate_nnue(int player, int *pieces, int *squares);
int evaluate_fen_nnue(char *fen);

#endif /* nnue_eval_hpp */
