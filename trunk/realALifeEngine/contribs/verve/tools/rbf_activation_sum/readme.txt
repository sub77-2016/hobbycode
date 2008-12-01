This directory contains tools for experimentally computing the maximum sum of RBF activation given an RBF standard deviation (distance from the mean to 1 std. dev.) and a "resolution" parameter (the number of RBFs within a 2 unit range).  This assumes all RBFs will be positioned within [-1, 1].

The goal here is to find a relations between resolution and total activation sum for various RBF std. dev. widths.  These relations can be used to adjust learning rates based on the total amount of RBF activation.


Results for 1 dimension (tested up through res = 300):
-----------------------------------------------------
Std. Dev. Width		Approximate Relation
---------------		--------------------

0.4				Sum = 0.6166 * res^1
0.5				Sum = 0.7477 * res^1
1.2				Sum = 1.172 * res^1
4.0				Sum = 1.3168 * res^1
5.0				Sum = 1.3226 * res^1


Results for 2 dimensions (tested up through res = 100):
------------------------------------------------------
Std. Dev. Width		Approximate Relation
---------------		--------------------

0.4				Sum = 0.1441 * res^2
0.5				Sum = 0.2949 * res^2
1.2				Sum = 1.2746 * res^2
4.0				Sum = 1.829 * res^2
5.0				Sum = 1.8536 * res^2


Results for 3 dimensions (tested up through res = 50):
-----------------------------------------------------
Std. Dev. Width		Approximate Relation
---------------		--------------------

0.4				Sum = 0.0359 * res^3
0.5				Sum = 0.1332 * res^3
1.2				Sum = 1.7642 * res^3
4.0				Sum = 3.2945 * res^3
5.0				Sum = 3.3715 * res^3


Results for 4 dimensions (tested up through res = 30):
-----------------------------------------------------
Std. Dev. Width		Approximate Relation
---------------		--------------------

0.4				Sum = 0.0067 * res^4
0.5				Sum = 0.0524 * res^4
1.2				Sum = 2.6066 * res^4
4.0				Sum = 6.5627 * res^4
5.0				Sum = 6.7897 * res^4


Std. Dev. Width		Additional Multiplicative Constant
---------------		----------------------------------
0.4				0.24
0.5				0.42
1.2				1.3
4.0				1.75
5.0				1.8


For the case std. dev. width = 0.5, we can use the following relation:
Sum = 0.7477 * 0.42^(n-1) * res^n


Now, instead of using a constant std. dev. width, we'll try having it depend on resolution...

Results for 1 dimension (tested up through res = 300):
# of Dimensions	Max Res. Tested	Std. Dev. Width Relation	Approximate Sum Relation
---------------	---------------	------------------------	------------------------

1			300			width = 2 / res			Sum approaches 2.507
2			200			width = 2 / res			Sum approaches 6.3
3			70			width = 2 / res			Sum approaches 16.0
4			30			width = 2 / res			Sum approaches 40.0

The relation here is: Sum = 2.5^n


Worst-case Relation
-------------------
(Actually use resolution = # of RBFs close enough to be activated at all.  For the worst case we're just assuming that all close RBFs are maximally active.)
for 1 dimension: sum = resolution
for n dimensions: sum = resolution^n
