/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include <assert.h>

#include "vp10/encoder/cost.h"
#include "vp10/common/entropy.h"

const unsigned int vp10_prob_cost[256] = {
  2047, 2047, 1791, 1641, 1535, 1452, 1385, 1328, 1279, 1235, 1196, 1161,
  1129, 1099, 1072, 1046, 1023, 1000, 979,  959,  940,  922,  905,  889,
  873,  858,  843,  829,  816,  803,  790,  778,  767,  755,  744,  733,
  723,  713,  703,  693,  684,  675,  666,  657,  649,  641,  633,  625,
  617,  609,  602,  594,  587,  580,  573,  567,  560,  553,  547,  541,
  534,  528,  522,  516,  511,  505,  499,  494,  488,  483,  477,  472,
  467,  462,  457,  452,  447,  442,  437,  433,  428,  424,  419,  415,
  410,  406,  401,  397,  393,  389,  385,  381,  377,  373,  369,  365,
  361,  357,  353,  349,  346,  342,  338,  335,  331,  328,  324,  321,
  317,  314,  311,  307,  304,  301,  297,  294,  291,  288,  285,  281,
  278,  275,  272,  269,  266,  263,  260,  257,  255,  252,  249,  246,
  243,  240,  238,  235,  232,  229,  227,  224,  221,  219,  216,  214,
  211,  208,  206,  203,  201,  198,  196,  194,  191,  189,  186,  184,
  181,  179,  177,  174,  172,  170,  168,  165,  163,  161,  159,  156,
  154,  152,  150,  148,  145,  143,  141,  139,  137,  135,  133,  131,
  129,  127,  125,  123,  121,  119,  117,  115,  113,  111,  109,  107,
  105,  103,  101,  99,   97,   95,   93,   92,   90,   88,   86,   84,
  82,   81,   79,   77,   75,   73,   72,   70,   68,   66,   65,   63,
  61,   60,   58,   56,   55,   53,   51,   50,   48,   46,   45,   43,
  41,   40,   38,   37,   35,   33,   32,   30,   29,   27,   25,   24,
  22,   21,   19,   18,   16,   15,   13,   12,   10,   9,    7,    6,
  4,    3,    1,    1};

static void cost(int *costs, vpx_tree tree, const vpx_prob *probs,
                 int i, int c) {
  const vpx_prob prob = probs[i / 2];
  int b;

  for (b = 0; b <= 1; ++b) {
    const int cc = c + vp10_cost_bit(prob, b);
    const vpx_tree_index ii = tree[i + b];

    if (ii <= 0)
      costs[-ii] = cc;
    else
      cost(costs, tree, probs, ii, cc);
  }
}

#if CONFIG_ANS
void vp10_cost_tokens_ans(int *costs, const vpx_prob *tree_probs,
                          const vpx_prob *token_probs, int skip_eob) {
  int c_tree = 0;  // Cost of the "tree" nodes EOB and ZERO.
  int i;
  costs[EOB_TOKEN] = vp10_cost_bit(tree_probs[0], 0);
  if (!skip_eob)
    c_tree = vp10_cost_bit(tree_probs[0], 1);
  costs[ZERO_TOKEN] = c_tree + vp10_cost_bit(tree_probs[1], 0);
  c_tree += vp10_cost_bit(tree_probs[1], 1);
  for (i = ONE_TOKEN; i <= CATEGORY6_TOKEN; ++i) {
    costs[i] = c_tree + vp10_cost_bit(token_probs[i - ONE_TOKEN], 0);
  }
}
#endif  // CONFIG_ANS

void vp10_cost_tokens(int *costs, const vpx_prob *probs, vpx_tree tree) {
  cost(costs, tree, probs, 0, 0);
}

void vp10_cost_tokens_skip(int *costs, const vpx_prob *probs, vpx_tree tree) {
  assert(tree[0] <= 0 && tree[1] > 0);

  costs[-tree[0]] = vp10_cost_bit(probs[0], 0);
  cost(costs, tree, probs, 2, 0);
}
