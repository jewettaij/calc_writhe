This is a cautionary example.
Do not use this program to calculate the writhe of an open curve.

This curve is a long helix, whose diameter so so much narrower than it's length,
it looks like a straight line. I naively guessed it would have a writhe of ~= 0.
However the results of "calc_writhe" change drastically when deleting
a few points off of the end of the curve, with magnitudes of 10000 or larger.
(As a reference, whenever two of these helices are combined together in opposite
 directions to form a plectonemic loop, the resulting writhe should be only 10.
 See "gen_plecto.sh" in the "utils" directory.)
