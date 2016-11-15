// Game of Othello -- Example of main
// Universidad Simon Bolivar, 2012.
// Author: Blai Bonet
// Last Revision: 1/11/16
// Modified by: 

#include <iostream>
#include <climits>
#include "othello_cut.h" // won't work correctly until .h is fixed!
#include "utils.h"

#include <unordered_map>

using namespace std;

unsigned expanded = 0;
unsigned generated = 0;
int tt_threshold = 32; // threshold to save entries in TT

// Transposition table
struct stored_info_t {
    int value_;
    int type_;
    enum { EXACT, LOWER, UPPER };
    stored_info_t(int value = -100, int type = LOWER) : value_(value), type_(type) { }
};

struct hash_function_t {
    size_t operator()(const state_t &state) const {
        return state.hash();
    }
};

class hash_table_t : public unordered_map<state_t, stored_info_t, hash_function_t> {
};

hash_table_t TTable[2];

int maxmin(state_t state, int depth, bool use_tt);
int minmax(state_t state, int depth, bool use_tt = false);
int maxmin(state_t state, int depth, bool use_tt = false);
int negamax(state_t state, int depth, int color, bool use_tt = false);
int negamax(state_t state, int depth, int alpha, int beta, int color, bool use_tt = false);
int scout(state_t state, int depth, int color, bool use_tt = false);
int negascout(state_t state, int depth, int alpha, int beta, int color, bool use_tt = false);

int main(int argc, const char **argv) {
    state_t pv[128];
    int npv = 0;
    for( int i = 0; PV[i] != -1; ++i ) ++npv;

    int algorithm = 0;
    if( argc > 1 ) algorithm = atoi(argv[1]);
    bool use_tt = argc > 2;

    // Extract principal variation of the game
    state_t state;
    cout << "Extracting principal variation (PV) with " << npv << " plays ... " << flush;
    for( int i = 0; PV[i] != -1; ++i ) {
        bool player = i % 2 == 0; // black moves first!
        int pos = PV[i];
        pv[npv - i] = state;
        state = state.move(player, pos);
    }
    pv[0] = state;
    cout << "done!" << endl;

#if 0
    // print principal variation
    for( int i = 0; i <= npv; ++i )
        cout << pv[npv - i];
#endif

    // Print name of algorithm
    cout << "Algorithm: ";
    if( algorithm == 0 ) {
        cout << "Minmax-Maxmin";
    } else if( algorithm == 1 ) {
        cout << "Negamax (minmax version)";
    } else if( algorithm == 2 ) {
        cout << "Negamax (alpha-beta version)";
    } else if( algorithm == 3 ) {
        cout << "Scout";
    } else if( algorithm == 4 ) {
        cout << "Negascout";
    }
    cout << (use_tt ? " w/ transposition table" : "") << endl;

    // Run algorithm along PV (bacwards)
    cout << "Moving along PV:" << endl;
    for( int i = 0; i <= npv; ++i ) {
        //cout << pv[i];
        int value = 0;
        TTable[0].clear();
        TTable[1].clear();
        float start_time = Utils::read_time_in_seconds();
        expanded = 0;
        generated = 0;
        int color = i % 2 == 1 ? 1 : -1;

        try {
            if( algorithm == 0 ) {
                value = color * (color == 1 ? maxmin(pv[i], 0, use_tt) : minmax(pv[i], 0, use_tt));
            } else if( algorithm == 1 ) {
                value = negamax(pv[i], i, color, use_tt);
            } else if( algorithm == 2 ) {
                value = negamax(pv[i], 0, -200, 200, color, use_tt);
            } else if( algorithm == 3 ) {
                value = color*scout(pv[i], 0, color, use_tt);
            } else if( algorithm == 4 ) {
                value = negascout(pv[i], i, -200, 200, color, use_tt);
            }
        } catch( const bad_alloc &e ) {
            cout << "size TT[0]: size=" << TTable[0].size() << ", #buckets=" << TTable[0].bucket_count() << endl;
            cout << "size TT[1]: size=" << TTable[1].size() << ", #buckets=" << TTable[1].bucket_count() << endl;
            use_tt = false;
        }
        float elapsed_time = Utils::read_time_in_seconds() - start_time;

        cout << npv + 1 - i << ". " << (color == 1 ? "Black" : "White") << " moves: "
             << "value=" << color * value
             << ", #expanded=" << expanded
             << ", #generated=" << generated
             << ", seconds=" << elapsed_time
             << ", #generated/second=" << generated/elapsed_time
             << endl;
    }

    return 0;
}

int maxmin(state_t state, int depth, bool use_tt){
    state_t child;
    bool pass = true;
    if (state.terminal() ) return state.value();
    int score = INT_MIN;

    //iterate over all possible valid moves
    for (int i = 0; i < DIM; ++i)
    {
        if (state.is_black_move(i)){
            child = state.black_move(i);
            generated++;
            score = max(score,minmax(child,depth-1));
            pass = false;
        }
    }
    //Player has no possible move, passes the turn to adversary
    if (pass) score = max(score,minmax(state,depth-1));
    
    expanded++;
    return score;

}

int minmax(state_t state, int depth, bool use_tt){
    state_t child;
    bool pass = true;
    if (state.terminal() ) return state.value();
    int score = INT_MAX;

    //iterate over all possible valid moves
    for (int i = 0; i < DIM; ++i)
    {
        if (state.is_white_move(i)){
            child = state.white_move(i);
            generated++;
            score = min(score,maxmin(child,depth-1));
            pass = false;
        }
    }
    //Player has no possible move, passes the turn to adversary
    if (pass) score = min(score,maxmin(state,depth-1));
    
    expanded++;
    return score;

}

int negamax(state_t state, int depth, int color, bool use_tt){
    /*
        color = 1 ---> Max node
        color = -1 ---> Min node
    */

    state_t child;
    bool is_max = false;
    bool pass = true;
    is_max = color > 0;
    if (state.terminal() ) return color*state.value();
    int alpha = INT_MIN;

    //iterate over all possible valid moves
    for (int i = 0; i < DIM; ++i)
    {
        if (state.outflank(is_max,i)){
            child = state.move(is_max,i);
            generated++;
            alpha = max(alpha,-negamax(child,depth-1,-color));
            pass = false;
        }
    }
    //Player has no possible move, passes the turn to adversary
    if (pass) alpha = max(alpha,-negamax(state,depth-1,-color));

    expanded++;
    return alpha;

}

int negamax(state_t state, int depth, int alpha, int beta, int color, bool use_tt){
    /*
        color = 1 ---> Max node
        color = -1 ---> Min node
    */

    state_t child;
    bool is_max = false;
    bool pass = true;
    is_max = color > 0;
    if (state.terminal() ) return color*state.value();
    int score = INT_MIN;

    //iterate over all possible valid moves
    for (int i = 0; i < DIM; ++i)
    {
        if (state.outflank(is_max,i)){
            child = state.move(is_max,i);
            generated++;
            int val = -negamax(child,depth-1,-beta,-alpha,-color);
            alpha = max(alpha,val);
            score = max(val,score);
            pass = false;
            if (alpha > beta) break;
        }
    }
    //Player has no possible move, passes the turn to adversary
    if (pass) score = -negamax(state,depth-1,alpha,beta,-color);

    expanded++;
    return score;

}
bool TEST(state_t state, int score , bool bigger, int color){
    
    state_t child;
    bool is_max = false;
    bool pass = true;
    is_max = color > 0;

    if (state.terminal() ){
        if(bigger) return (state.value() > score);
        return (state.value() >= score);
    } 

    for (int i = 0; i < DIM; ++i)
    {
        if (state.outflank(is_max,i)){
            child = state.move(is_max,i);
            generated++;
            pass = false;
            if (is_max && TEST(child,score,bigger,-color)) return true;
            if (!is_max && !TEST(child,score,bigger,-color)) return false;
        }
    }
    if (pass){
        expanded++; 
        return TEST(state,score,bigger,-color);
    }

    expanded++;
    return !is_max;
}

int scout(state_t state, int depth, int color, bool use_tt){
    /*
        color = 1 ---> Max node
        color = -1 ---> Min node
    */

    state_t child;
    bool is_max = false;
    bool pass = true;
    bool f_child = true;
    is_max = color > 0;
    if (state.terminal() ) return state.value();
    int score = 0;

    //iterate over all possible valid moves
    for (int i = 0; i < DIM; ++i)
    {
        if (state.outflank(is_max,i)){
            child = state.move(is_max,i);
            generated++;
            pass = false;
            if (f_child){ 
                f_child = false;
                score = scout(child,depth-1,-color);
            }
            else{
                if (is_max && TEST(child,score,true,-color)) score = scout(child,depth-1,-color);
                if (!is_max && !TEST(child,score,false,-color)) score = scout(child,depth-1,-color);
            }
            
        }
    }
    //Player has no possible move, passes the turn to adversary
    if (pass) score = scout(state,depth-1,-color);

    expanded++;
    return score;
}

int negascout(state_t state, int depth, int alpha, int beta, int color, bool use_tt){
    /*
        color = 1 ---> Max node
        color = -1 ---> Min node
    */
    state_t child;
    bool is_max = false;
    bool pass = true;
    bool f_child = true;
    is_max = color > 0;
    if (state.terminal() ){  return color*state.value(); }
    int score;
    //iterate over all possible valid moves
    for (int i = 0; i < DIM; ++i)
    {
        if (state.outflank(is_max,i)){
            child = state.move(is_max,i);
            generated++;
            pass = false;
            if (f_child){ 
                f_child = false;
                score = -negascout(child,depth-1,-beta,-alpha,-color);
            }
            else{
                score = -negascout(child,depth-1,-alpha - 1,-alpha,-color);
                
                if(alpha < score && score < beta ){
                    score = -negascout(child,depth-1,-beta,-score,-color);
                }
            }
            alpha = max(alpha,score);
            if (alpha >= beta) break;
        }

    }
    //Player has no possible move, passes the turn to adversary
    if (pass) alpha = -negascout(state,depth-1,-beta,-alpha,-color);
           
    expanded++;
    return alpha;

}
