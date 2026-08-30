# Route A — Propagation-complete OBDD closure

## Theorem
Let `B` be a reduced ordered binary decision diagram for a Boolean function `f(X)`. For every nonterminal node

\[
v=\operatorname{ite}(x,h,l),
\]

use the six prime clauses of the four-variable ITE relation:

\[
\begin{aligned}
&\neg v\vee x\vee l, &&\neg v\vee\neg x\vee h,\\
&v\vee x\vee\neg l, &&v\vee\neg x\vee\neg h,\\
&\neg v\vee l\vee h,&&v\vee\neg l\vee\neg h.
\end{aligned}
\]

Fix the false terminal to 0, the true terminal to 1, and the root to 1.

For every partial assignment `alpha` of input variables:

* if `f|alpha` is identically false, unit propagation derives the root false and hence a conflict;
* if `f|alpha` is identically true, unit propagation derives the root true.

### Proof
Induct upward from the terminals. If a restricted node is identically false and its decision variable is assigned, the selected child is identically false and one branch clause propagates `not v`. If the decision variable is unset, both children are identically false and the first consensus clause propagates `not v`. The true case is dual and uses the second consensus clause. Applying this at the root proves the claim.

Consequently, if `C` is any implicate of `f`, falsifying all literals of `C` makes `f` identically false; unit propagation therefore conflicts. Thus **every implicate of every width is RUP** against this OBDD extension.

## Impact on ES(7)
The previously built seven-point reduced OBDD has 11,536 nonterminal nodes and a 52,993-clause six-ITE encoding. It compactly provides the entire local implicate closure, including all known width-6 and width-7 primes and any higher-width primes. Full width-by-width enumeration is useful for auditing but is no longer required for logical completeness.
