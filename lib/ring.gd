#############################################################################
##
#W  ring.gd                     GAP library                     Thomas Breuer
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1997,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
#Y  (C) 1998 School Math and Comp. Sci., University of St.  Andrews, Scotland
##
##  This file declares the operations for rings.
##
Revision.ring_gd :=
    "@(#)$Id$";


#############################################################################
##
#P  IsNearRing( <R> )
##
##  A *near-ring* in {\GAP} is a near-additive group
##  (see~"IsNearAdditiveGroup") that is also a semigroup (see~"IsSemigroup"),
##  such that addition `+' and multiplication `\*' are right distributive
##  (see~"IsRDistributive").
##  Any associative ring (see~"IsRing") is also a near-ring.
##
DeclareSynonymAttr( "IsNearRing",
    IsNearAdditiveGroup and IsMagma and IsRDistributive and IsAssociative );


#############################################################################
##
#P  IsNearRingWithOne( <R> )
##
##  A *near-ring-with-one* in {\GAP} is a near-ring (see~"IsNearRing")
##  that is also a magma-with-one (see~"IsMagmaWithOne").
##
##  Note that the identity and the zero of a near-ring-with-one need *not* be
##  distinct.
##  This means that a near-ring that consists only of its zero element can be
##  regarded as a near-ring-with-one.
##
DeclareSynonymAttr( "IsNearRingWithOne", IsNearRing and IsMagmaWithOne );


#############################################################################
##
#A  AsNearRing( <C> )
##
##  If the elements in the collection <C> form a near-ring then `AsNearRing'
##  returns this near-ring, otherwise `fail' is returned.
##
DeclareAttribute( "AsNearRing", IsNearRingElementCollection );


#############################################################################
##
#P  IsRing( <R> )
##
##  A *ring* in {\GAP} is an additive group (see~"IsAdditiveGroup")
##  that is also a magma (see~"IsMagma"),
##  such that addition `+' and multiplication `\*' are distributive.
##
##  The multiplication need *not* be associative (see~"IsAssociative").
##  For example, a Lie algebra (see~"Lie Algebras") is regarded as a
##  ring in {\GAP}.
##
DeclareSynonymAttr( "IsRing",
    IsAdditiveGroup and IsMagma and IsDistributive );


#############################################################################
##
#P  IsRingWithOne( <R> )
##
##  A *ring-with-one* in {\GAP} is a ring (see~"IsRing")
##  that is also a magma-with-one (see~"IsMagmaWithOne").
##
##  Note that the identity and the zero of a ring-with-one need *not* be
##  distinct.
##  This means that a ring that consists only of its zero element can be
##  regarded as a ring-with-one.
#T shall we force *every* trivial ring to be a ring-with-one
#T by installing an implication?
##
##  This is especially useful in the case of finitely presented rings,
##  in the sense that each factor of a ring-with-one is again a
##  ring-with-one.
##
DeclareSynonymAttr( "IsRingWithOne", IsRing and IsMagmaWithOne );


#############################################################################
##
#A  AsRing( <C> )
##
##  If the elements in the collection <C> form a ring then `AsRing' returns
##  this ring, otherwise `fail' is returned.
##
DeclareAttribute( "AsRing", IsRingElementCollection );


#############################################################################
##
#A  GeneratorsOfRing( <R> )
##
##  `GeneratorsOfRing' returns a list of elements such that the ring <R> is
##  the closure of these elements under addition, multiplication,
##  and taking additive inverses.
##
DeclareAttribute( "GeneratorsOfRing", IsRing );


#############################################################################
##
#A  GeneratorsOfRingWithOne( <R> )
##
##  `GeneratorsOfRingWithOne' returns a list of elements
##  such that the ring <R> is the closure of these elements
##  under addition, multiplication, taking additive inverses, and taking
##  the identity element `One( <R> )'.
##
##  <R> itself need *not* be known to be a ring-with-one.
##
DeclareAttribute( "GeneratorsOfRingWithOne", IsRingWithOne );


#############################################################################
##
#O  RingByGenerators( <C> ) . . . . . . .  ring gener. by elements in a coll.
##
##  `RingByGenerators' returns the ring generated by the elements in the
##  collection <C>,
##  i.~e., the closure of <C> under addition, multiplication,
##  and taking additive inverses.
##
DeclareOperation( "RingByGenerators", [ IsCollection ] );


#############################################################################
##
#O  DefaultRingByGenerators( <coll> ) . . . . default ring containing a coll.
##
DeclareOperation( "DefaultRingByGenerators", [ IsCollection ] );


#############################################################################
##
#F  Ring( <r> ,<s>, ... )  . . . . . . . . . . ring generated by a collection
#F  Ring( <coll> ) . . . . . . . . . . . . . . ring generated by a collection
##
##  In the first form `Ring' returns the smallest ring that
##  contains all the elements <r>, <s>... etc.
##  In the second form `Ring' returns the smallest ring that
##  contains all the elements in the collection <coll>.
##  If any element is not an element of a ring or if the elements lie in no
##  common ring an error is raised.
##
##  `Ring' differs from `DefaultRing' (see~"DefaultRing") in that it returns
##  the smallest ring in which the elements lie, while `DefaultRing' may
##  return a larger ring if that makes sense.
##
DeclareGlobalFunction( "Ring" );


#############################################################################
##
#O  RingWithOneByGenerators( <coll> )
##
##  `RingWithOneByGenerators' returns the ring-with-one generated by the
##  elements in the collection <coll>, i.~e., the closure of <coll> under
##  addition, multiplication, taking additive inverses,
##  and taking the identity of an element.
##
DeclareOperation( "RingWithOneByGenerators", [ IsCollection ] );


#############################################################################
##
#F  RingWithOne( <r>, <s>, ... )  . . ring-with-one generated by a collection
#F  RingWithOne( <C> )  . . . . . . . ring-with-one generated by a collection
##
##  In the first form `RingWithOne' returns the smallest ring with one that
##  contains all the elements <r>, <s>... etc.
##  In the second form `RingWithOne' returns the smallest ring with one that
##  contains all the elements in the collection <C>.
##  If any element is not an element of a ring or if the elements lie in no
##  common ring an error is raised.
##
DeclareGlobalFunction( "RingWithOne" );


#############################################################################
##
#F  DefaultRing( <r> ,<s>, ... )  . . .  default ring containing a collection
#F  DefaultRing( <coll> ) . . . . . . .  default ring containing a collection
##
##  In the first form `DefaultRing' returns a ring that contains
##  all the elements <r>, <s>, ... etc.
##  In the second form `DefaultRing' returns a ring that contains
##  all the elements in the collection <coll>.
##  If any element is not an element of a ring or if the elements lie in no
##  common ring an error is raised.
##
##  The ring returned by `DefaultRing' need not be the smallest ring in which
##  the elements lie.
##  For example for elements from cyclotomic fields,
##  `DefaultRing' may return the ring of integers of the smallest cyclotomic
##  field in which the elements lie, which need not be the smallest ring
##  overall, because the elements may in fact lie in a smaller number field
##  which is itself not a cyclotomic field.
##
##  (For the exact definition of the default ring of a certain type of
##  elements, look at the corresponding method installation.)
##
##  `DefaultRing' is used by the ring functions like `Quotient', `IsPrime',
##  `Factors', or `Gcd' if no explicit ring is given.
##
##  `Ring' (see~"Ring") differs from `DefaultRing' in that it returns the
##  smallest ring in which the elements lie, while `DefaultRing' may return
##  a larger ring if that makes sense.
##
DeclareGlobalFunction( "DefaultRing" );


#############################################################################
##
#F  Subring( <R>, <gens> ) . . . . . . . . subring of <R> generated by <gens>
#F  SubringNC( <R>, <gens> ) . . . . . . . subring of <R> generated by <gens>
##
##  returns the ring with parent <R> generated by the elements in
##  <gens>. When the second form, `SubringNC' is used, it is *not* checked
##  whether all elements in <gens> lie in <R>. 
##
DeclareGlobalFunction( "Subring" );
DeclareGlobalFunction( "SubringNC" );


#############################################################################
##
#F  SubringWithOne( <R>, <gens> )   .  subring-with-one of <R> gen. by <gens>
#F  SubringWithOneNC( <R>, <gens> ) .  subring-with-one of <R> gen. by <gens>
##
##  returns the ring with one with parent <R> generated by the elements in
##  <gens>. When the second form, `SubringNC' is used, it is *not* checked
##  whether all elements in <gens> lie in <R>. 
##
DeclareGlobalFunction( "SubringWithOne" );
DeclareGlobalFunction( "SubringWithOneNC" );


#############################################################################
##
#O  ClosureRing( <R>, <r> )
#O  ClosureRing( <R>, <S> )
##
##  For a ring <R> and either an element <r> of its elements family or a ring
##  <S>, `ClosureRing' returns the ring generated by both arguments.
##
DeclareOperation( "ClosureRing", [ IsRing, IsObject ] );


#############################################################################
##
#C  IsUniqueFactorizationRing( <R> )
##
##  A ring <R> is called a *unique factorization ring* if it is an integral
##  ring (see~"IsIntegralRing"),
##  and every element has a unique factorization into irreducible elements,
##  i.e., a  unique representation as product  of irreducibles (see
##  "IsIrreducibleRingElement").
##  Unique in this context means unique up to permutations of the factors and
##  up to multiplication of the factors by units (see~"Units").
##
##  Mathematically, a field should therefore also be a  unique factorization
##  ring, since every element is a unit. In {\GAP}, however, at least at present
##  fields do not lie in the filter `IsUniqueFactorizationRing' 
##  (see~"IsUniqueFactorizationRing"), since 
##  Operations such as `Factors', `Gcd', `StandardAssociate' and so on do
##  not apply to fields (the results would be trivial, and not
##  especially useful) and Methods which require their arguments to
##  lie in `IsUniqueFactorizationRing' expect these Operations to work.
##
##  (Note that we cannot install a subset maintained method for this category
##  since the factorization of an element needs not exist in a subring.
##  As an example, consider the subring $4 \N + 1$ of the ring $4 \Z + 1$;
##  in the subring, the element $3 \cdot 3 \cdot 11 \cdot 7$ has the two
##  factorizations $33 \cdot 21 = 9 \cdot 77$, but in the large ring there
##  is the unique factorization $(-3) \cdot (-3) \cdot (-11) \cdot (-7)$,
##  and it is easy to see that every element in $4 \Z + 1$ has a unique
##  factorization.)
##
DeclareCategory( "IsUniqueFactorizationRing", IsRing );


#############################################################################
##
#C  IsEuclideanRing( <R> )
##
##  A ring $R$ is called a Euclidean ring if it is an integral ring and
##  there exists a function $\delta$, called the Euclidean degree, from
##  $R-\{0_R\}$ to the nonnegative integers, such that for every pair $r \in
##  R$ and $s \in  R-\{0_R\}$ there exists an element $q$ such that either
##  $r - q s = 0_R$ or $\delta(r - q s) \< \delta( s )$. In {\GAP} the
##  Euclidean degree $\delta$ is implicitly built into an ring and cannot be
##  changed.  The existence of this division with remainder implies that the
##  Euclidean algorithm can be applied to compute a greatest common divisor
##  of two elements, which in turn implies that $R$ is a unique
##  factorization ring.
##
#T more general: new category ``valuated domain''?
##
DeclareCategory( "IsEuclideanRing",
    IsRingWithOne and IsUniqueFactorizationRing );


#############################################################################
##
#P  IsAnticommutative( <R> )
##
##  is `true' if the relation $a * b = - b * a$
##  holds for all elements $a$, $b$ in the ring <R>,
##  and `false' otherwise.
##
DeclareProperty( "IsAnticommutative", IsRing );

InstallSubsetMaintenance( IsAnticommutative,
    IsRing and IsAnticommutative, IsRing );

InstallFactorMaintenance( IsAnticommutative,
    IsRing and IsAnticommutative, IsObject, IsRing );


#############################################################################
##
#P  IsIntegralRing( <R> )
##
##  A ring-with-one <R> is integral if it is commutative, contains no
##  nontrivial zero divisors,
##  and if its identity is distinct from its zero.
##
DeclareProperty( "IsIntegralRing", IsRing );

InstallSubsetMaintenance( IsIntegralRing,
    IsRing and IsIntegralRing, IsRing and IsNonTrivial );

InstallTrueMethod( IsIntegralRing,
    IsRing and IsMagmaWithInversesIfNonzero and IsNonTrivial );
InstallTrueMethod( IsIntegralRing,
    IsUniqueFactorizationRing and IsNonTrivial );


#############################################################################
##
#P  IsJacobianRing( <R> )
##
##  is `true' if the Jacobi identity holds in <R>, and `false' otherwise.
##  The Jacobi identity means that $x \* (y \* z) + z \* (x \* y) + 
##  y \* (z \* x)$
##  is the zero element of <R>, for all elements $x$, $y$, $z$ in <R>.
##
DeclareProperty( "IsJacobianRing", IsRing );

InstallTrueMethod( IsJacobianRing,
    IsJacobianElementCollection and IsRing );

InstallSubsetMaintenance( IsJacobianRing,
    IsRing and IsJacobianRing, IsRing );

InstallFactorMaintenance( IsJacobianRing,
    IsRing and IsJacobianRing, IsObject, IsRing );


#############################################################################
##
#P  IsZeroSquaredRing( <R> )
##
##  is `true' if $a * a$ is the zero element of the ring <R>
##  for all $a$ in <R>, and `false' otherwise.
##
DeclareProperty( "IsZeroSquaredRing", IsRing );

InstallTrueMethod( IsAnticommutative, IsRing and IsZeroSquaredRing );

InstallTrueMethod( IsZeroSquaredRing,
    IsZeroSquaredElementCollection and IsRing );

InstallSubsetMaintenance( IsZeroSquaredRing,
    IsRing and IsZeroSquaredRing, IsRing );

InstallFactorMaintenance( IsZeroSquaredRing,
    IsRing and IsZeroSquaredRing, IsObject, IsRing );


#############################################################################
##
#A  Units( <R> )
##
##  `Units' returns the group of units of the ring <R>.
##  This may either be returned as a list or as a group.
##
##  An element $r$ is called a *unit* of a ring $R$, if $r$ has an inverse in
##  $R$.
##  It is easy to see that the set of units forms a multiplicative group.
##
DeclareAttribute( "Units", IsRing );


#############################################################################
##
#O  Factors( <R>, <r> )
#O  Factors( <r> )
##
##  In the first form `Factors' returns the factorization of the ring
##  element <r> in the ring <R>.
##  In the second form `Factors' returns the factorization of the ring
##  element <r> in its default ring (see "DefaultRing").
##  The factorization is returned as a list of primes (see "IsPrime").
##  Each element in the list is a standard associate (see
##  "StandardAssociate") except the first one, which is multiplied by a unit
##  as necessary to have `Product( Factors( <R>, <r> )  )  = <r>'.
##  This list is usually also sorted, thus smallest prime factors come first.
##  If <r> is a unit or zero, `Factors( <R>, <r> ) = [ <r> ]'.
##
#T Who does really need the additive structure?
#T We could define `Factors' for arbitrary commutative monoids.
##
DeclareOperation( "Factors", [ IsRing, IsRingElement ] );


#############################################################################
##
#O  IsAssociated( <R>, <r>, <s> )
#O  IsAssociated( <r>, <s> )
##
##  In the first form `IsAssociated' returns `true' if the two ring elements
##  <r> and <s> are associated in the ring <R> and `false' otherwise.
##  In the second form `IsAssociated' returns `true' if the two ring elements
##  <r> and <s> are associated in their default ring (see "DefaultRing") and
##  `false' otherwise.
##
##  Two elements $r$ and $s$ of a ring $R$ are called *associated* if there
##  is a unit $u$ of $R$ such that $r u = s$.
##
DeclareOperation( "IsAssociated", [ IsRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  Associates( <R>, <r> )
#O  Associates( <r> )
##
##  In the first form `Associates' returns the set of associates of <r> in
##  the ring <R>.
##  In the second form `Associates' returns the set of associates of the
##  ring element <r> in its default ring (see "DefaultRing").
##
##  Two elements $r$ and $s$ of a ring $R$ are called *associated* if there
##  is a unit $u$ of $R$ such that $r u = s$.
##
DeclareOperation( "Associates",
    [ IsRing, IsRingElement ] );


#############################################################################
##
#O  IsUnit( <R>, <r> )  . . . . . . . . .  check whether <r> is a unit in <R>
#O  IsUnit( <r> ) . . . . . . check whether <r> is a unit in its default ring
##
##  In the first form `IsUnit' returns `true' if <r> is a unit in the ring
##  <R>.
##  In the second form `IsUnit' returns `true' if the ring element <r> is a
##  unit in its default ring (see "DefaultRing").
##
##  An element $r$ is called a *unit* in a ring $R$, if $r$ has an inverse in
##  $R$.
##
##  `IsUnit' may call `Quotient'.
#T really?
##
DeclareOperation( "IsUnit", [ IsRing, IsRingElement ] );


#############################################################################
##
#O  InterpolatedPolynomial( <R>, <x>, <y> ) . . . . . . . . . . interpolation
##
##  `InterpolatedPolynomial' returns, for given lists <x>, <y> of elements in
##  a ring <R> of the same length $n$, say, the unique  polynomial of  degree
##  less than $n$ which has value <y>[$i$] at <x>[$i$], 
##  for all $i\in\{1,\ldots,n\}$. 
##  Note that the elements in <x> must be distinct.
##
DeclareOperation( "InterpolatedPolynomial",
    [ IsRing, IsHomogeneousList, IsHomogeneousList ] );


#############################################################################
##
#O  Quotient( <R>, <r>, <s> )
#O  Quotient( <r>, <s> )
##
##  In the first form `Quotient' returns the quotient of the two ring
##  elements <r> and <s> in the ring <R>.
##  In the second form `Quotient' returns the quotient of the two ring
##  elements <r> and <s> in their default ring.
##  It returns `fail' if the quotient does not exist in the respective ring.
##
##  (To perform the division in the quotient field of a ring, use the
##  quotient operator `/'.)
##
DeclareOperation( "Quotient", [ IsRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  StandardAssociate( <R>, <r> )
#O  StandardAssociate( <r> )
##
##  In the first form `StandardAssociate' returns the standard associate of
##  the ring element <r> in the ring <R>.
##  In the second form `StandardAssociate' returns the standard associate of
##  the ring element <r> in its default ring (see "DefaultRing").
##
##  The *standard associate* of a ring element $r$ of $R$ is an associated
##  element of $r$ which is, in a ring dependent way, distinguished among the
##  set of associates of $r$.
##  For example, in the ring of integers the standard associate is the
##  absolute value.
##
DeclareOperation( "StandardAssociate", [ IsRing, IsRingElement ] );


#############################################################################
##
#O  IsPrime( <R>, <r> )
#O  IsPrime( <r> )
##
##  In the first form `IsPrime' returns `true' if the ring element <r> is a
##  prime in the ring <R> and `false' otherwise.
##  In the second form `IsPrime' returns `true' if the ring element <r> is a
##  prime in its default ring (see "DefaultRing") and `false' otherwise.
##
##  An element $r$ of a ring $R$ is called *prime* if for each pair $s$ and
##  $t$ such that $r$ divides $s t$ the element $r$ divides either $s$ or
##  $t$.
##  Note that there are rings where not every irreducible element
##  (see "IsIrreducibleRingElement") is a prime.
##
DeclareOperation( "IsPrime", [ IsRing, IsRingElement ] );


#############################################################################
##
#O  IsIrreducibleRingElement( <R>, <r> )
#O  IsIrreducibleRingElement( <r> )
##
##  In the first form `IsIrreducibleRingElement' returns `true' if the ring
##  element <r> is irreducible in the ring <R> and `false' otherwise.
##  In the second form `IsIrreducibleRingElement' returns `true' if the ring
##  element <r> is irreducible in its default ring (see "DefaultRing") and
##  `false' otherwise.
##
##  An element $r$ of a ring $R$ is called *irreducible* if $r$ is not a
##  unit in $R$ and if there is no nontrivial factorization of $r$ in $R$,
##  i.e., if there is no representation of $r$ as product $s t$ such that
##  neither $s$ nor $t$ is a unit (see "IsUnit").
##  Each prime element (see "IsPrime") is irreducible.
##
DeclareOperation( "IsIrreducibleRingElement", [ IsRing, IsRingElement ] );


#############################################################################
##
#O  EuclideanDegree( <R>, <r> )
#O  EuclideanDegree( <r> )
##
##  In the first form `EuclideanDegree' returns the Euclidean degree of the
##  ring element in the ring <R>.
##  In the second form `EuclideanDegree' returns the Euclidean degree of the
##  ring element <r> in its default ring.
##  <R> must of course be a Euclidean ring (see "IsEuclideanRing").
##
DeclareOperation( "EuclideanDegree", [ IsEuclideanRing, IsRingElement ] );


#############################################################################
##
#O  EuclideanRemainder( <R>, <r>, <m> )
#O  EuclideanRemainder( <r>, <m> )
##
##  In the first form `EuclideanRemainder' returns the remainder of the ring
##  element <r> modulo the ring element <m> in the ring <R>.
##  In the second form `EuclideanRemainder' returns the remainder of the ring
##  element <r> modulo the ring element <m> in their default ring.
##  The ring <R> must be a Euclidean ring (see "IsEuclideanRing") otherwise
##  an error is signalled.
##
DeclareOperation( "EuclideanRemainder",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  EuclideanQuotient( <R>, <r>, <m> )
#O  EuclideanQuotient( <r>, <m> )
##
##  In the first form `EuclideanQuotient' returns the Euclidean quotient of
##  the ring elements <r> and <m> in the ring <R>.
##  In the second form `EuclideanQuotient' returns the Euclidean quotient of
##  the ring elements <r> and <m> in their default ring.
##  The ring <R> must be a Euclidean ring (see "IsEuclideanRing") otherwise
##  an error is signalled.
##
DeclareOperation( "EuclideanQuotient",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  QuotientRemainder( <R>, <r>, <s> )
#O  QuotientRemainder( <r>, <s> )
##
##  In the first form `QuotientRemainder' returns the Euclidean quotient
##  and the Euclidean remainder of the ring elements <r> and <m> in the ring
##  <R>.
##  In the second form `QuotientRemainder' returns the Euclidean quotient and
##  the Euclidean remainder of the ring elements <r> and <m> in their default
##  ring as pair of ring elements.
##  The ring <R> must be a Euclidean ring (see "IsEuclideanRing") otherwise
##  an error is signalled.
##
DeclareOperation( "QuotientRemainder",
    [ IsRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  QuotientMod( <R>, <r>, <s>, <m> )
#O  QuotientMod( <r>, <s>, <m> )
##
##  In the first form `QuotientMod' returns the quotient of the ring
##  elements <r> and <s> modulo the ring element <m> in the ring <R>.
##  In the second form `QuotientMod' returns the quotient of the ring elements
##  <r> and  <s> modulo the ring element <m> in their default ring (see
##  "DefaultRing").
##  <R> must be a Euclidean ring (see "IsEuclideanRing") so that
##  `EuclideanRemainder' (see "EuclideanRemainder") can be applied.
##  If the modular quotient does not exist, `fail' is returned.
##
##  The quotient $q$ of $r$ and $s$ modulo $m$ is an element of $R$ such that
##  $q s = r$ modulo $m$, i.e., such that $q s - r$ is divisible by $m$ in
##  $R$ and that $q$ is either 0 (if $r$ is divisible by $m$) or the
##  Euclidean degree of $q$ is strictly smaller than the Euclidean degree of
##  $m$.
##
DeclareOperation( "QuotientMod",
    [ IsRing, IsRingElement, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  PowerMod( <R>, <r>, <e>, <m> )
#O  PowerMod( <r>, <e>, <m> )
##
##  In the first form `PowerMod' returns the <e>-th power of the ring
##  element <r> modulo the ring element <m> in the ring <R>.
##  In the second form `PowerMod' returns the <e>-th power of the ring
##  element <r> modulo the ring element <m> in their default ring (see
##  "DefaultRing").
##  <e> must be an integer.
##  <R> must be a Euclidean ring (see "IsEuclideanRing") so that
##  `EuclideanRemainder' (see "EuclideanRemainder") can be applied to its
##  elements.
##
##  If $e$ is positive the result is $r^e$ modulo $m$.
##  If $e$ is negative then `PowerMod' first tries to find the inverse of $r$
##  modulo $m$, i.e., $i$ such that $i r = 1$ modulo $m$.
##  If the inverse does not exist an error is signalled.
##  If the inverse does exist `PowerMod' returns
##  `PowerMod( <R>, <i>, -<e>, <m> )'.
##
##  `PowerMod' reduces the intermediate values modulo $m$, improving
##  performance drastically when <e> is large and <m> small.
##
DeclareOperation( "PowerMod",
    [ IsRing, IsRingElement, IsInt, IsRingElement ] );


#############################################################################
##
#F  Gcd( <R>, <r1>, <r2>, ... )
#F  Gcd( <R>, <list> )
#F  Gcd( <r1>, <r2>, ... )
#F  Gcd( <list> )
##
##  In the first two forms `Gcd' returns the greatest common divisor of the
##  ring elements `<r1>, <r2>, ...' resp. of the ring elements in the list
##  <list> in the ring <R>.
##  In the second two forms `Gcd' returns the greatest common divisor of the
##  ring elements `<r1>, <r2>, ...' resp. of the ring elements in the list
##  <list> in their default ring (see "DefaultRing").
##  <R> must be a Euclidean ring (see "IsEuclideanRing") so that
##  `QuotientRemainder' (see "QuotientRemainder") can be applied to its
##  elements.
##  `Gcd' returns the standard associate (see "StandardAssociate") of the
##  greatest common divisors.
##
##  A greatest common divisor of the elements $r_1, r_2, \ldots$ of the
##  ring $R$ is an element of largest Euclidean degree (see
##  "EuclideanDegree") that is a divisor of $r_1, r_2, \ldots$ .
##
##  We define 
##  `Gcd( <r>, $0_{<R>}$ ) = Gcd( $0_{<R>}$, <r> ) = StandardAssociate( <r> )'
##  and `Gcd( $0_{<R>}$, $0_{<R>}$ ) = $0_{<R>}$'.
##
DeclareGlobalFunction( "Gcd" );


#############################################################################
##
#O  GcdOp( <R>, <r>, <s> )
#O  GcdOp( <r>, <s> )
##
##  `GcdOp' is the operation to compute the greatest common divisor of
##  two ring elements <r>, <s> in the ring <R> or in their default ring.
##

DeclareOperation( "GcdOp",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#F  GcdRepresentation( <R>, <r1>, <r2>, ... )
#F  GcdRepresentation( <R>, <list> )
#F  GcdRepresentation( <r1>, <r2>, ... )
#F  GcdRepresentation( <list> )
##
##  In the first two forms `GcdRepresentation' returns the representation of
##  the greatest common divisor of the ring elements `<r1>, <r2>, ...' resp.
##  of the ring elements in the list <list> in the ring <R>.
##  In the second two forms `GcdRepresentation' returns the representation of
##  the greatest common divisor of the ring elements `<r1>, <r2>, ...' resp.
##  of the ring elements in the list <list> in their default ring
##  (see "DefaultRing").
##  <R> must be a Euclidean ring (see "IsEuclideanRing") so that
##  `Gcd' (see "Gcd") can be applied to its elements.
##
##  The representation of the gcd  $g$ of  the elements $r_1, r_2, \ldots$
##  of a ring $R$ is a list of ring elements $s_1, s_2, \ldots$ of $R$,
##  such that $g = s_1 r_1 + s_2  r_2 + \cdots$.
##  That this representation exists can be shown using the Euclidean
##  algorithm, which in fact can compute those coefficients.
##
DeclareGlobalFunction( "GcdRepresentation" );


#############################################################################
##
#O  GcdRepresentationOp( <R>, <r>, <s> )
#O  GcdRepresentationOp( <r>, <s> )
##
##  `GcdRepresentationOp' is the operation to compute the representation of
##  the greatest common divisor of two ring elements <r>, <s> in the ring
##  <R> or in their default ring, respectively.
##
DeclareOperation( "GcdRepresentationOp",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#F  Lcm( <R>, <r1>, <r2>, ... )
#F  Lcm( <R>, <list> )
#F  Lcm( <r1>, <r2>, ... )
#F  Lcm( <list> )
#T optional ``1'' in list version?
##
##  In the first two forms `Lcm' returns the least common multiple of the
##  ring elements `<r1>, <r2>, ...' resp. of the ring elements in the list
##  <list> in the ring <R>.
##  In the second two forms `Lcm' returns the least common multiple of the
##  ring elements `<r1>, <r2>, ...' resp. of the ring elements in the list
##  <list> in their default ring (see~"DefaultRing").
##
##  <R> must be a Euclidean ring (see~"IsEuclideanRing") so that `Gcd'
##  (see~"Gcd") can be applied to its elements.
##  `Lcm' returns the standard associate (see~"StandardAssociate") of the
##  least common multiples.
##
##  A least common multiple of the elements $r_1, r_2, \ldots$ of the
##  ring $R$ is an element of smallest Euclidean degree
##  (see~"EuclideanDegree") that is a multiple of $r_1, r_2, \ldots$ .
##
##  We define 
##  `Lcm( <r>, $0_{<R>}$ ) = Lcm( $0_{<R>}$, <r> ) = StandardAssociate( <r> )'
##  and `Lcm( $0_{<R>}$, $0_{<R>}$ ) = $0_{<R>}$'.
##
##  `Lcm' uses the equality $lcm(m,n) = m\*n / gcd(m,n)$ (see~"Gcd").
##
DeclareGlobalFunction( "Lcm" );


#############################################################################
##
#O  LcmOp( <R>, <r>, <s> )
#O  LcmOp( <r>, <s> )
##
##  `LcmOp' is the operation to compute the least common multiple of
##  two ring elements <r>, <s> in the ring <R> or in their default ring,
##  respectively.
##
DeclareOperation( "LcmOp",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#E

