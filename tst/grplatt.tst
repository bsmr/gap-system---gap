#############################################################################
##
#W  grplatt.tst                GAP tests                     Alexander Hulpke
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1997,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This  file  tests the subgroup lattice program
##

gap> START_TEST("$Id$");

gap> g:=PerfectGroup(IsPermGroup,95040);;
gap> l:=ConjugacyClassesSubgroups(g);;
gap> li:=List(l,i->[Size(Representative(i)),Size(i)]);;
gap> Sort(li);
gap> Length(li);
147
gap> Print(List(li,i->i),"\n");
[ [ 1, 1 ], [ 2, 396 ], [ 2, 495 ], [ 3, 880 ], [ 3, 1320 ], [ 4, 495 ], 
  [ 4, 1320 ], [ 4, 1485 ], [ 4, 1485 ], [ 4, 1980 ], [ 4, 2970 ], 
  [ 5, 2376 ], [ 6, 1320 ], [ 6, 2640 ], [ 6, 2640 ], [ 6, 3960 ], 
  [ 6, 3960 ], [ 6, 7920 ], [ 8, 495 ], [ 8, 495 ], [ 8, 495 ], [ 8, 495 ], 
  [ 8, 990 ], [ 8, 1485 ], [ 8, 1485 ], [ 8, 1485 ], [ 8, 2970 ], 
  [ 8, 2970 ], [ 8, 2970 ], [ 8, 2970 ], [ 8, 2970 ], [ 8, 2970 ], 
  [ 8, 2970 ], [ 9, 220 ], [ 9, 220 ], [ 9, 1760 ], [ 10, 2376 ], 
  [ 10, 2376 ], [ 10, 2376 ], [ 11, 1728 ], [ 12, 1320 ], [ 12, 1320 ], 
  [ 12, 1980 ], [ 12, 1980 ], [ 12, 2640 ], [ 12, 3960 ], [ 12, 3960 ], 
  [ 12, 7920 ], [ 16, 495 ], [ 16, 1485 ], [ 16, 1485 ], [ 16, 1485 ], 
  [ 16, 1485 ], [ 16, 1485 ], [ 16, 1485 ], [ 16, 2970 ], [ 16, 2970 ], 
  [ 16, 2970 ], [ 16, 2970 ], [ 16, 2970 ], [ 18, 220 ], [ 18, 220 ], 
  [ 18, 2640 ], [ 18, 2640 ], [ 18, 5280 ], [ 20, 2376 ], [ 20, 2376 ], 
  [ 20, 2376 ], [ 24, 1320 ], [ 24, 1980 ], [ 24, 1980 ], [ 24, 1980 ], 
  [ 24, 1980 ], [ 24, 1980 ], [ 24, 1980 ], [ 24, 1980 ], [ 24, 1980 ], 
  [ 24, 3960 ], [ 27, 880 ], [ 32, 495 ], [ 32, 495 ], [ 32, 1485 ], 
  [ 32, 1485 ], [ 32, 1485 ], [ 32, 1485 ], [ 32, 1485 ], [ 36, 660 ], 
  [ 36, 660 ], [ 36, 1320 ], [ 36, 1320 ], [ 36, 1320 ], [ 40, 2376 ], 
  [ 48, 495 ], [ 48, 1980 ], [ 48, 1980 ], [ 48, 1980 ], [ 48, 1980 ], 
  [ 54, 880 ], [ 54, 880 ], [ 54, 880 ], [ 55, 1728 ], [ 60, 396 ], 
  [ 60, 792 ], [ 60, 792 ], [ 60, 1584 ], [ 64, 1485 ], [ 72, 220 ], 
  [ 72, 220 ], [ 72, 660 ], [ 72, 660 ], [ 72, 660 ], [ 72, 660 ], 
  [ 72, 1320 ], [ 96, 495 ], [ 96, 495 ], [ 96, 495 ], [ 96, 495 ], 
  [ 108, 880 ], [ 120, 396 ], [ 120, 396 ], [ 120, 396 ], [ 120, 792 ], 
  [ 120, 792 ], [ 144, 660 ], [ 144, 660 ], [ 192, 495 ], [ 192, 495 ], 
  [ 216, 220 ], [ 216, 220 ], [ 240, 396 ], [ 360, 66 ], [ 360, 66 ], 
  [ 432, 220 ], [ 432, 220 ], [ 660, 144 ], [ 660, 144 ], [ 720, 66 ], 
  [ 720, 66 ], [ 720, 66 ], [ 720, 66 ], [ 720, 66 ], [ 720, 66 ], 
  [ 1440, 66 ], [ 1440, 66 ], [ 7920, 12 ], [ 7920, 12 ], [ 95040, 1 ] ]

gap> LatticeSubgroups(Group((1,2,3,4,5,6)));;
gap> g := Group( (1,2,3,4)(5,6,7,8), (1,5,3,7)(2,8,4,6) );;
gap> AsSortedList(List(ConjugacyClassesSubgroups(g),i->Size(Representative(i))));
[ 1, 2, 4, 4, 4, 8 ]
gap> AsSortedList(List(NormalSubgroups(g),Size));
[ 1, 2, 4, 4, 4, 8 ]

# thats all, folks
gap> STOP_TEST( "grplatt.tst", 5761430000 );

#############################################################################
##
#E  grplatt.tst  . . . . . . . . . . . . . . . . . . . . . . . . . ends here
##
