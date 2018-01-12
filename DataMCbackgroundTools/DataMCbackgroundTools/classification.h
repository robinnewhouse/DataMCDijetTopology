#ifndef classification_H_
#define classification_H_

#include <map>
#include <string>


enum class topo : int {t = 1, W = 2, bq = 3, b = 4, other = 5, notruth = 6, unknown = -1};
const topo topo_types [] = {topo::t, topo::W, topo::bq, topo::b, topo::other, topo::notruth, topo::unknown}; 

const float dRoverRmax_topology = 0.75; // DEFAULT dR cut for jet containment classification ( dR divided by radius of jet collection)

enum class sele : int {t = 2, W = 1, unknown = -1};
const sele sele_types [] = {sele::t, sele::W};

enum class jetType : int { akt = 0, ca = 1, unknown = -1 };

// enum for classifying result of simple smoothed cut-based taggers taggers
// 0 - didn't pass any cut
// 1 - pass variable 1 cut
// 2 - pass variable 2 cut
// 3 - pass both cuts
enum class simpleTaggerPass : int { invalid = -1, none = 0, var1 = 1, var2 = 2, both = 3};

enum class smoothedTopTaggerConfig : int {Tau32Split23 = 0, MassTau32 = 1, QwTau32 = 2};

#endif
