/****************************************************************************
**
*W  exprs.c                     GAP source                   Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
*Y  (C) 1998 School Math and Comp. Sci., University of St.  Andrews, Scotland
**
**  This file contains the functions of the expressions package.
**
**  The expressions  package is the  part  of the interpreter  that evaluates
**  expressions to their values and prints expressions.
*/
#include        "system.h"              /* Ints, UInts                     */

const char * Revision_exprs_c =
   "@(#)$Id$";

#include        "gasman.h"              /* garbage collector               */
#include        "objects.h"             /* objects                         */
#include        "scanner.h"             /* scanner                         */

#include        "gap.h"                 /* error handling, initialisation  */

#include        "gvars.h"               /* global variables                */

#include        "ariths.h"              /* basic arithmetic                */
#include        "records.h"             /* generic records                 */
#include        "lists.h"               /* generic lists                   */

#include        "bool.h"                /* booleans                        */

#include        "permutat.h"            /* permutations                    */

#include        "precord.h"             /* plain records                   */

#include        "plist.h"               /* plain lists                     */
#include        "range.h"               /* ranges                          */
#include        "string.h"              /* strings                         */

#include        "code.h"                /* coder                           */
#include        "vars.h"                /* variables                       */

#define INCLUDE_DECLARATION_PART
#include        "exprs.h"               /* expressions                     */
#undef  INCLUDE_DECLARATION_PART


/****************************************************************************
**

*F  OBJ_REFLVAR(<expr>) . . . . . . . . . . . value of a reference to a local
**
**  'OBJ_REFLVAR'  returns  the value of  the reference  to a  local variable
**  <expr>.
**
**  'OBJ_REFLVAR'  is defined  in the  declaration  part of  this  package as
**  follows
**
#ifdef  NO_LVAR_CHECKS
#define OBJ_REFLVAR(expr)       \
                        OBJ_LVAR( LVAR_REFLVAR( (expr) ) )
#endif
#ifndef NO_LVAR_CHECKS
#define OBJ_REFLVAR(expr)       \
                        (*(Obj*)(((char*)PtrLVars)+(expr)+5) != 0 ? \
                         *(Obj*)(((char*)PtrLVars)+(expr)+5) : \
                         ObjLVar( LVAR_REFLVAR( expr ) ) )
#endif
*/


/****************************************************************************
**
*F  OBJ_INTEXPR(<expr>) . . . . . . . . . . .  value of an integer expression
**
**  'OBJ_INTEXPR' returns the (immediate)  integer  value of the  (immediate)
**  integer expression <expr>.
**
**  'OBJ_INTEXPR(<expr>)'  should  be 'OBJ_INT(INT_INTEXPR(<expr>))', but for
**  performance  reasons we implement  it   as '(Obj)(<expr>)'.  This is   of
**  course    highly  dependent  on    (immediate)  integer   expressions and
**  (immediate) integer values having the same representation.
**
**  'OBJ_INTEXPR' is  defined in  the declaration  part  of  this package  as
**  follow
**
#define OBJ_INTEXPR(expr)       \
                        ((Obj)(Int)(Int4)(expr))
*/


/****************************************************************************
**
*F  EVAL_EXPR(<expr>) . . . . . . . . . . . . . . . .  evaluate an expression
**
**  'EVAL_EXPR' evaluates the expression <expr>.
**
**  'EVAL_EXPR' returns the value of <expr>.
**
**  'EVAL_EXPR'  causes  the   evaluation of   <expr> by  dispatching  to the
**  evaluator, i.e., to  the function that evaluates  expressions of the type
**  of <expr>.
**
**  Note that 'EVAL_EXPR' does not use 'TNUM_EXPR', since it also handles the
**  two special cases that 'TNUM_EXPR' handles.
**
**  'EVAL_EXPR' is defined in the declaration part of this package as follows:
**
#define EVAL_EXPR(expr) \
                        (IS_REFLVAR(expr) ? OBJ_REFLVAR(expr) : \
                         (IS_INTEXPR(expr) ? OBJ_INTEXPR(expr) : \
                          (*EvalExprFuncs[ TNUM_STAT(expr) ])( expr ) ))
*/


/****************************************************************************
**
*V  EvalExprFuncs[<type>]  . . . . . evaluator for expressions of type <type>
**
**  'EvalExprFuncs'  is the dispatch table   that contains for  every type of
**  expressions a pointer  to the  evaluator  for expressions of this   type,
**  i.e., the function that should be  called to evaluate expressions of this
**  type.
*/
Obj             (* EvalExprFuncs [256]) ( Expr expr );


/****************************************************************************
**
*F  EVAL_BOOL_EXPR(<expr>)  . . . . evaluate an expression to a boolean value
**
**  'EVAL_BOOL_EXPR' evaluates   the expression  <expr> and  checks  that the
**  value is either  'true' or 'false'.  If the  expression does not evaluate
**  to 'true' or 'false', then an error is signalled.
**
**  'EVAL_BOOL_EXPR' returns the  value of <expr> (which  is either 'true' or
**  'false').
**
**  'EVAL_BOOL_EXPR' is defined  in the declaration part  of this package  as
**  follows
**
#define EVAL_BOOL_EXPR(expr) \
                        ( (*EvalBoolFuncs[ TNUM_EXPR( expr ) ])( expr ) )
*/


/****************************************************************************
**
*V  EvalBoolFuncs[<type>] . . boolean evaluator for expression of type <type>
**
**  'EvalBoolFuncs'  is  the dispatch table that  contains  for every type of
**  expression a pointer to a boolean evaluator for expressions of this type,
**  i.e., a pointer to  a function which  is  guaranteed to return a  boolean
**  value that should be called to evaluate expressions of this type.
*/
Obj             (* EvalBoolFuncs [256]) ( Expr expr );


/****************************************************************************
**
*F  EvalUnknownExpr(<expr>) . . . . . . . evaluate expression of unknown type
**
**  'EvalUnknownExpr' is the evaluator that  is called if  an attempt is made
**  to  evaluate an  expression  <expr> of  an  unknown type.   It signals an
**  error.  If this is ever called, then  GAP is in  serious trouble, such as
**  an overwritten type field of an expression.
*/
Obj             EvalUnknownExpr (
    Expr                expr )
{
    Pr( "Panic: tried to evaluate an expression of unknown type '%d'\n",
        (Int)TNUM_EXPR(expr), 0L );
    return 0;
}


/****************************************************************************
**
*F  EvalUnknownBool(<expr>) . . . . boolean evaluator for general expressions
**
**  'EvalUnknownBool' evaluates   the expression <expr>  (using 'EVAL_EXPR'),
**  and checks that the value is either 'true' or 'false'.  If the expression
**  does not evaluate to 'true' or 'false', then an error is signalled.
**
**  This is the default function in 'EvalBoolFuncs' used for expressions that
**  are   not a priori    known  to evaluate  to a    boolean value  (such as
**  function calls).
*/
Obj             EvalUnknownBool (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */

    /* evaluate the expression                                             */
    val = EVAL_EXPR( expr );

    /* check that the value is either 'true' or 'false'                    */
    while ( val != True && val != False ) {
        val = ErrorReturnObj(
            "<expr> must be 'true' or 'false' (not a %s)",
            (Int)TNAM_OBJ(val), 0L,
            "you can replace <expr> via 'return <expr>;'" );
    }

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalOr(<expr>)  . . . . . . . . . . . . . evaluate a boolean or operation
**
**  'EvalOr' evaluates the or-expression <expr> and  returns its value, i.e.,
**  'true'  if  either of  the operands  is  'true',  and 'false'  otherwise.
**  'EvalOr'  is   called from  'EVAL_EXPR' to  evaluate  expressions of type
**  'T_OR'.
**
**  If '<expr>.left'  is   already  'true' 'EvalOr'  returns  'true'  without
**  evaluating '<expr>.right'.  This allows constructs like
**
**      if (index > max) or (list[index] = 0)  then ... fi;
*/
Obj             EvalOr (
    Expr                expr )
{
    Obj                 opL;            /* evaluated left operand          */
    Expr                tmp;            /* temporary expression            */

    /* evaluate and test the left operand                                  */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_BOOL_EXPR( tmp );
    if ( opL != False ) {
        return True;
    }

    /* evaluate and test the right operand                                 */
    tmp = ADDR_EXPR(expr)[1];
    return EVAL_BOOL_EXPR( tmp );
}


/****************************************************************************
**
*F  EvalAnd(<expr>) . . . . . . . . . . . .  evaluate a boolean and operation
**
**  'EvalAnd'  evaluates  the and-expression <expr>   and  returns its value,
**  i.e.,   'true'  if both  operands  are   'true',  and  'false' otherwise.
**  'EvalAnd' is called from   'EVAL_EXPR' to  evaluate expressions  of  type
**  'T_AND'.
**
**  If '<expr>.left' is  already  'false' 'EvalAnd' returns 'false'   without
**  evaluating '<expr>.right'.  This allows constructs like
**
**      if (index <= max) and (list[index] = 0)  then ... fi;
*/
extern  Obj             NewAndFilter (
            Obj                     oper1,
            Obj                     oper2 );

Obj             EvalAnd (
    Expr                expr )
{
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* if the left operand is 'false', this is the result                  */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    if      ( opL == False ) {
        return opL;
    }

    /* if the left operand is 'true', the result is the right operand      */
    else if ( opL == True  ) {
        tmp = ADDR_EXPR(expr)[1];
        return EVAL_BOOL_EXPR( tmp );
    }

    /* handle the 'and' of two filters                                    */
    else if ( TNUM_OBJ(opL) == T_FUNCTION ) {
        tmp = ADDR_EXPR(expr)[1];
        opR = EVAL_EXPR( tmp );
        if ( TNUM_OBJ(opR) == T_FUNCTION ) {
            return NewAndFilter( opL, opR );
        }
        else {
            ErrorQuit(
                "<expr> must be 'true' or 'false' (not a %s)",
                (Int)TNAM_OBJ(opL), 0L );
        }
    }
    
    /* signal an error                                                     */
    else {
        ErrorQuit(
            "<expr> must be 'true' or 'false' (not a %s)",
            (Int)TNAM_OBJ(opL), 0L );
    }
    
    /* please 'lint'                                                       */
    return 0;
}


/****************************************************************************
**
*F  EvalNot(<expr>) . . . . . . . . . . . . . . . . .  negate a boolean value
**
**  'EvalNot'  evaluates the  not-expression  <expr>  and returns its  value,
**  i.e., 'true' if the operand is 'false', and 'false' otherwise.  'EvalNot'
**  is called from 'EVAL_EXPR' to evaluate expressions of type 'T_NOT'.
*/
Obj             EvalNot (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 op;             /* evaluated operand               */
    Expr                tmp;            /* temporary expression            */

    /* evaluate the operand to a boolean                                   */
    tmp = ADDR_EXPR(expr)[0];
    op = EVAL_BOOL_EXPR( tmp );

    /* compute the negation                                                */
    val = (op == False ? True : False);

    /* return the negated value                                            */
    return val;
}


/****************************************************************************
**
*F  EvalEq(<expr>)  . . . . . . . . . . . . . . . . . . evaluate a comparison
**
**  'EvalEq' evaluates the  equality-expression <expr> and returns its value,
**  i.e.,  'true' if  the  operand '<expr>.left'   is equal  to  the  operand
**  '<expr>.right'   and   'false'  otherwise.   'EvalEq'  is   called   from
**  'EVAL_EXPR' to evaluate expressions of type 'T_EQ'.
**
**  'EvalEq' evaluates the operands and then calls the 'EQ' macro.
*/
Obj             EvalEq (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* compare the operands                                                */
    val = (EQ( opL, opR ) ? True : False);

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalNe(<expr>)  . . . . . . . . . . . . . . . . . . evaluate a comparison
**
**  'EvalNe'   evaluates the  comparison-expression  <expr>  and  returns its
**  value, i.e.,  'true'  if the operand   '<expr>.left' is not equal  to the
**  operand  '<expr>.right' and  'false' otherwise.  'EvalNe'  is called from
**  'EVAL_EXPR' to evaluate expressions of type 'T_LT'.
**
**  'EvalNe' is simply implemented as 'not <objL> = <objR>'.
*/
Obj             EvalNe (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* compare the operands                                                */
    val = (EQ( opL, opR ) ? False : True);

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalLt(<expr>)  . . . . . . . . . . . . . . . . . . evaluate a comparison
**
**  'EvalLt' evaluates  the  comparison-expression   <expr> and  returns  its
**  value, i.e., 'true' if the operand '<expr>.left' is less than the operand
**  '<expr>.right'  and  'false'   otherwise.    'EvalLt'  is   called   from
**  'EVAL_EXPR' to evaluate expressions of type 'T_LT'.
**
**  'EvalLt' evaluates the operands and then calls the 'LT' macro.
*/
Obj             EvalLt (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* compare the operands                                                */
    val = (LT( opL, opR ) ? True : False);

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalGe(<expr>)  . . . . . . . . . . . . . . . . . . evaluate a comparison
**
**  'EvalGe'  evaluates  the comparison-expression   <expr>  and returns  its
**  value, i.e., 'true' if the operand '<expr>.left' is greater than or equal
**  to the operand '<expr>.right' and 'false'  otherwise.  'EvalGe' is called
**  from 'EVAL_EXPR' to evaluate expressions of type 'T_GE'.
**
**  'EvalGe' is simply implemented as 'not <objL> < <objR>'.
*/
Obj             EvalGe (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* compare the operands                                                */
    val = (LT( opL, opR ) ? False : True);

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalGt(<expr>)  . . . . . . . . . . . . . . . . . . evaluate a comparison
**
**  'EvalGt'  evaluates  the  comparison-expression <expr>   and  returns its
**  value, i.e.,  'true' if the  operand  '<expr>.left' is  greater than  the
**  operand '<expr>.right' and 'false' otherwise.    'EvalGt' is called  from
**  'EVAL_EXPR' to evaluate expressions of type 'T_GT'.
**
**  'EvalGt' is simply implemented as '<objR> < <objL>'.
*/
Obj             EvalGt (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* compare the operands                                                */
    val = (LT( opR, opL ) ? True : False);

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalLe(<expr>)  . . . . . . . . . . . . . . . . . . evaluate a comparison
**
**  'EvalLe' evaluates   the comparison-expression   <expr> and  returns  its
**  value, i.e., 'true' if the operand '<expr>.left' is  less or equal to the
**  operand '<expr>.right' and 'false'   otherwise.  'EvalLe' is  called from
**  'EVAL_EXPR' to evaluate expressions of type 'T_LE'.
**
**  'EvalLe' is simply implemented as 'not <objR> < <objR>'.
*/
Obj             EvalLe (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* compare the operands                                                */
    val = (LT( opR, opL ) ? False : True);

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalIn(<in>)  . . . . . . . . . . . . . . . test for membership in a list
**
**  'EvalIn' evaluates the in-expression <expr>  and returns its value, i.e.,
**  'true' if  the  operand '<expr>.left'  is a  member of '<expr>.right' and
**  'false' otherwise.    'EvalIn' is  called  from  'EVAL_EXPR'  to evaluate
**  expressions of type 'T_IN'.
*/
Obj             EvalIn (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* evaluate <opL>                                                      */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );

    /* evaluate <opR>                                                      */
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* perform the test                                                    */
    val = (IN( opL, opR ) ? True : False);

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalSum(<expr>) . . . . . . . . . . . . . . . . . . . . .  evaluate a sum
**
**  'EvalSum'  evaluates the  sum-expression  <expr> and  returns its  value,
**  i.e., the sum of   the  two operands '<expr>.left'   and  '<expr>.right'.
**  'EvalSum'   is called from 'EVAL_EXPR'   to  evaluate expressions of type
**  'T_SUM'.
**
**  'EvalSum' evaluates the operands and then calls the 'SUM' macro.
*/
Obj             EvalSum (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* first try to treat the operands as small integers with small result */
    if ( ! ARE_INTOBJS( opL, opR ) || ! SUM_INTOBJS( val, opL, opR ) ) {

        /* if that doesn't work, dispatch to the addition function         */
        val = SUM( opL, opR );

    }

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalAInv(<expr>)  . . . . . . . . . . . . . . evaluate a additive inverse
**
**  'EvalAInv' evaluates  the additive  inverse-expression  and  returns  its
**  value, i.e., the  additive inverse of  the operand.  'EvalAInv' is called
**  from 'EVAL_EXPR' to evaluate expressions of type 'T_AINV'.
**
**  'EvalAInv' evaluates the operand and then calls the 'AINV' macro.
*/
Obj             EvalAInv (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );

    /* compute the additive inverse                                        */
    val = AINV( opL );

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalDiff(<expr>)  . . . . . . . . . . . . . . . . . evaluate a difference
**
**  'EvalDiff'  evaluates  the difference-expression <expr>   and returns its
**  value, i.e.,   the   difference of  the two  operands   '<expr>.left' and
**  '<expr>.right'.  'EvalDiff'    is  called from   'EVAL_EXPR'  to evaluate
**  expressions of type 'T_DIFF'.
**
**  'EvalDiff' evaluates the operands and then calls the 'DIFF' macro.
*/
Obj             EvalDiff (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* first try to treat the operands as small integers with small result */
    if ( ! ARE_INTOBJS( opL, opR ) || ! DIFF_INTOBJS( val, opL, opR ) ) {

        /* if that doesn't work, dispatch to the subtraction function      */
        val = DIFF( opL, opR );

    }

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalProd(<expr>)  . . . . . . . . . . . . . . . . . .  evaluate a product
**
**  'EvalProd' evaluates the product-expression <expr>  and returns it value,
**  i.e., the product of  the two operands '<expr>.left'  and '<expr>.right'.
**  'EvalProd'  is called from   'EVAL_EXPR' to evaluate  expressions of type
**  'T_PROD'.
**
**  'EvalProd' evaluates the operands and then calls the 'PROD' macro.
*/
Obj             EvalProd (
    Expr                expr )
{
    Obj                 val;            /* result                          */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* first try to treat the operands as small integers with small result */
    if ( ! ARE_INTOBJS( opL, opR ) || ! PROD_INTOBJS( val, opL, opR ) ) {

        /* if that doesn't work, dispatch to the multiplication function   */
        val = PROD( opL, opR );

    }

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalInv(<expr>) . . . . . . . . . . . . evaluate a multiplicative inverse
**
**  'EvalInv' evaluates the multiplicative inverse-expression and returns its
**  value,  i.e., the multiplicative inverse  of  the operand.  'EvalInv' is
**  called from 'EVAL_EXPR' to evaluate expressions of type 'T_INV'.
**
**  'EvalInv' evaluates the operand and then calls the 'INV' macro.
*/
Obj             EvalInv (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );

    /* compute the multiplicative inverse                                  */
    val = INV_MUT( opL );

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalQuo(<expr>) . . . . . . . . . . . . . . . . . . . evaluate a quotient
**
**  'EvalQuo' evaluates the quotient-expression <expr> and returns its value,
**  i.e., the quotient of the  two operands '<expr>.left' and '<expr>.right'.
**  'EvalQuo' is  called  from 'EVAL_EXPR' to   evaluate expressions  of type
**  'T_QUO'.
**
**  'EvalQuo' evaluates the operands and then calls the 'QUO' macro.
*/
Obj             EvalQuo (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* dispatch to the division function                                   */
    val = QUO( opL, opR );

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalMod(<expr>) . . . . . . . . . . . . . . . . . .  evaluate a remainder
**
**  'EvalMod' evaluates the  remainder-expression   <expr> and returns    its
**  value, i.e.,  the  remainder  of   the two  operands   '<expr>.left'  and
**  '<expr>.right'.  'EvalMod'  is   called   from  'EVAL_EXPR'  to  evaluate
**  expressions of type 'T_MOD'.
**
**  'EvalMod' evaluates the operands and then calls the 'MOD' macro.
*/
Obj             EvalMod (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* dispatch to the remainder function                                  */
    val = MOD( opL, opR );

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalPow(<expr>) . . . . . . . . . . . . . . . . . . . .  evaluate a power
**
**  'EvalPow'  evaluates the power-expression  <expr>  and returns its value,
**  i.e.,   the power of the  two  operands '<expr>.left' and '<expr>.right'.
**  'EvalPow' is called  from  'EVAL_EXPR'  to evaluate expressions  of  type
**  'T_POW'.
**
**  'EvalPow' evaluates the operands and then calls the 'POW' macro.
*/
Obj             EvalPow (
    Expr                expr )
{
    Obj                 val;            /* value, result                   */
    Obj                 opL;            /* evaluated left  operand         */
    Obj                 opR;            /* evaluated right operand         */
    Expr                tmp;            /* temporary expression            */

    /* get the operands                                                    */
    tmp = ADDR_EXPR(expr)[0];
    opL = EVAL_EXPR( tmp );
    tmp = ADDR_EXPR(expr)[1];
    opR = EVAL_EXPR( tmp );

    /* dispatch to the powering function                                   */
    val = POW( opL, opR );

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalIntExpr(<expr>) . . . . . . . . . evaluate literal integer expression
**
**  'EvalIntExpr' evaluates the literal integer expression <expr> and returns
**  its value.
*/
#define IDDR_EXPR(expr)         ((UInt2*)ADDR_EXPR(expr))

Obj             EvalIntExpr (
    Expr                expr )
{
    Obj                 val;            /* integer, result                 */
    UInt                i;              /* loop variable                   */

    /* allocate the integer                                                */
    if ( ((UInt2*)ADDR_EXPR(expr))[0] == 1 ) {
        val = NewBag( T_INTPOS, SIZE_EXPR(expr) - sizeof(UInt2) );
    }
    else {
        val = NewBag( T_INTNEG, SIZE_EXPR(expr) - sizeof(UInt2) );
    }

    /* copy over                                                           */
    for ( i = 1; i < SIZE_EXPR(expr)/sizeof(UInt2); i++ ) {
        ((UInt2*)ADDR_OBJ(val))[i-1] = ((UInt2*)ADDR_EXPR(expr))[i];
    }

    /* return the value                                                    */
    return val;
}


/****************************************************************************
**
*F  EvalTrueExpr(<expr>)  . . . . . . . . .  evaluate literal true expression
**
**  'EvalTrueExpr' evaluates the  literal true expression <expr> and  returns
**  its value (True).
*/
Obj             EvalTrueExpr (
    Expr                expr )
{
    return True;
}


/****************************************************************************
**
*F  EvalFalseExpr(<expr>) . . . . . . . . . evaluate literal false expression
**
**  'EvalFalseExpr' evaluates the literal false expression <expr> and returns
**  its value (False).
*/
Obj             EvalFalseExpr (
    Expr                expr )
{
    return False;
}


/****************************************************************************
**
*F  EvalCharExpr(<expr>)  . . . . . . evaluate a literal character expression
**
**  'EvalCharExpr' evaluates  the   literal character expression <expr>   and
**  returns its value.
*/
Obj             EvalCharExpr (
    Expr                expr )
{
    return ObjsChar[ ((UChar*)ADDR_EXPR(expr))[0] ];
}


/****************************************************************************
**
*F  EvalPermExpr(<expr>)  . . . . . . . . . evaluate a permutation expression
**
**  'EvalPermExpr' evaluates the permutation expression <expr>.
*/
Obj             EvalPermExpr (
    Expr                expr )
{
    Obj                 perm;           /* permutation, result             */
    UInt4 *             ptr4;           /* pointer into perm               */
    UInt2 *             ptr2;           /* pointer into perm               */
    Obj                 val;            /* one entry as value              */
    UInt                c, p, l;        /* entries in permutation          */
    UInt                m;              /* maximal entry in permutation    */
    Expr                cycle;          /* one cycle of permutation        */
    UInt                i, j, k;        /* loop variable                   */

    /* special case for identity permutation                               */
    if ( SIZE_EXPR(expr) == 0 ) {
        return IdentityPerm;
    }

    /* allocate the new permutation                                        */
    m = 0;
    perm = NEW_PERM4( 0 );

    /* loop over the cycles                                                */
    for ( i = 1; i <= SIZE_EXPR(expr)/sizeof(Expr); i++ ) {
        cycle = ADDR_EXPR(expr)[i-1];

        /* loop over the entries of the cycle                              */
        c = p = l = 0;
        for ( j = SIZE_EXPR(cycle)/sizeof(Expr); 1 <= j; j-- ) {

            /* get and check current entry for the cycle                   */
            val = EVAL_EXPR( ADDR_EXPR( cycle )[j-1] );
            while ( ! IS_INTOBJ(val) || INT_INTOBJ(val) <= 0 ) {
                val = ErrorReturnObj(
              "Permutation: <expr> must be a positive integer (not a %s)",
                    (Int)TNAM_OBJ(val), 0L,
                    "you can replace <expr> via 'return <expr>;'" );
            }
            c = INT_INTOBJ(val);

            /* if necessary resize the permutation                         */
            if ( SIZE_OBJ(perm)/sizeof(UInt4) < c ) {
                ResizeBag( perm, (c + 1023) / 1024 * 1024 * sizeof(UInt4) );
                ptr4 = ADDR_PERM4( perm );
                for ( k = m+1; k <= SIZE_OBJ(perm)/sizeof(UInt4); k++ ) {
                    ptr4[k-1] = k-1;
                }
            }
            if ( m < c ) {
                m = c;
            }

            /* check that the cycles are disjoint                          */
            ptr4 = ADDR_PERM4( perm );
            if ( (p != 0 && p == c) || (ptr4[c-1] != c-1) ) {
                return ErrorReturnObj(
                    "Permutation: cycles must be disjoint",
                    0L, 0L,
                    "you can replace permutation <perm> via 'return <perm>;'" );
            }

            /* enter the previous entry at current location                */
            ptr4 = ADDR_PERM4( perm );
            if ( p != 0 ) { ptr4[c-1] = p-1; }
            else          { l = c;          }

            /* remember current entry for next round                       */
            p = c;
        }

        /* enter first (last popped) entry at last (first popped) location */
        ptr4 = ADDR_PERM4( perm );
        ptr4[l-1] = p-1;

    }

    /* if possible represent the permutation with short entries            */
    if ( m <= 65536UL ) {
        ptr2 = ADDR_PERM2( perm );
        ptr4 = ADDR_PERM4( perm );
        for ( k = 1; k <= m; k++ ) {
            ptr2[k-1] = ptr4[k-1];
        };
        RetypeBag( perm, T_PERM2 );
        ResizeBag( perm, m * sizeof(UInt2) );
    }

    /* otherwise just shorten the permutation                              */
    else {
        ResizeBag( perm, m * sizeof(UInt4) );
    }

    /* return the permutation                                              */
    return perm;
}


/****************************************************************************
**
*F  EvalListExpr(<expr>)  . . . . .  evaluate list expression to a list value
**
**  'EvalListExpr'  evaluates the list   expression, i.e., not  yet evaluated
**  list, <expr> to a list value.
**
**  'EvalListExpr'  just  calls 'ListExpr1'  and  'ListExpr2' to evaluate the
**  list expression.
*/
Obj             ListExpr1 ( Expr expr );
void            ListExpr2 ( Obj list, Expr expr );
Obj             RecExpr1 ( Expr expr );
void            RecExpr2 ( Obj rec, Expr expr );

Obj             EvalListExpr (
    Expr                expr )
{
    Obj                 list;         /* list value, result                */

    /* evalute the list expression                                         */
    list = ListExpr1( expr );
    ListExpr2( list, expr );

    /* return the result                                                   */
    return list;
}


/****************************************************************************
**
*F  EvalListTildeExpr(<expr>) . . . . evaluate a list expression with a tilde
**
**  'EvalListTildeExpr' evaluates the     list  expression, i.e., not     yet
**  evaluated list, <expr> to a list value.  The difference to 'EvalListExpr'
**  is that  in <expr> there are   occurences of '~'  referring to  this list
**  value.
**
**  'EvalListTildeExpr' just  calls 'ListExpr1' to  create  the list, assigns
**  the list to  the variable '~', and  finally calls 'ListExpr2' to evaluate
**  the   subexpressions into  the  list.   Thus subexpressions  in  the list
**  expression can  refer to   this  variable and  its subobjects  to  create
**  objects that are not trees.
*/
Obj             EvalListTildeExpr (
    Expr                expr )
{
    Obj                 list;           /* list value, result              */
    Obj                 tilde;          /* old value of tilde              */

    /* remember the old value of '~'                                       */
    tilde = VAL_GVAR( Tilde );

    /* create the list value                                               */
    list = ListExpr1( expr );

    /* assign the list to '~'                                              */
    AssGVar( Tilde, list );

    /* evaluate the subexpressions into the list value                     */
    ListExpr2( list, expr );

    /* restore old value of '~'                                            */
    AssGVar( Tilde, tilde );

    /* return the list value                                               */
    return list;
}


/****************************************************************************
**
*F  ListExpr1(<expr>) . . . . . . . . . . . make a list for a list expression
*F  ListExpr2(<list>,<expr>)  . . .  enter the sublists for a list expression
**
**  'ListExpr1' and 'ListExpr2'  together evaluate the list expression <expr>
**  into the list <list>.
**
**  'ListExpr1'  allocates a new  plain  list of the  same  size as  the list
**  expression <expr> and returns this list.
**
**  'ListExpr2' evaluates  the  subexpression of <expr>   and puts the values
**  into the list  <list> (which should be a  plain list of  the same size as
**  the list expression <expr>, e.g., the one allocated by 'ListExpr1').
**
**  This two step allocation  is necessary, because  list expressions such as
**  '[ [1], ~[1] ]'  requires that the value of  one subexpression is entered
**  into the list value before the next subexpression is evaluated.
*/
Obj ListExpr1 (
    Expr                expr )
{
    Obj                 list;           /* list value, result              */
    Int                 len;            /* logical length of the list      */

    /* get the length of the list                                          */
    len = SIZE_EXPR(expr) / sizeof(Expr);

    /* allocate the list value                                             */
    if ( 0 == len ) {
        list = NEW_PLIST( T_PLIST_EMPTY, len );
    }
    else {
        list = NEW_PLIST( T_PLIST, len );
    }
    SET_LEN_PLIST( list, len );

    /* return the list                                                     */
    return list;
}

void ListExpr2 (
    Obj                 list,
    Expr                expr )
{
    Obj                 sub;            /* value of a subexpression        */
    Int                 len;            /* logical length of the list      */
    Int                 i;              /* loop variable                   */
    Int                 posshole;       /* initially 0, set to 1 at
					   first empty position, then
					   next full position causes
					   the list to be made
					   non-dense */

    /* get the length of the list                                          */
    len = SIZE_EXPR(expr) / sizeof(Expr);

    /* initially we have not seen a hole                                   */
    posshole = 0;

    /* handle the subexpressions                                           */
    for ( i = 1; i <= len; i++ ) {

        /* if the subexpression is empty                                   */
        if ( ADDR_EXPR(expr)[i-1] == 0 ) {
	  if (!posshole)
	    posshole = 1;
	  continue;
        }
	else 
	  {
	    if (posshole == 1)
	      {
		SET_FILT_LIST(list, FN_IS_NDENSE);
		posshole = 2;
	      }

	    /* special case if subexpression is a list expression              */
	    if ( TNUM_EXPR( ADDR_EXPR(expr)[i-1] ) == T_LIST_EXPR ) {
	      sub = ListExpr1( ADDR_EXPR(expr)[i-1] );
	      SET_ELM_PLIST( list, i, sub );
	      CHANGED_BAG( list );
	      ListExpr2( sub, ADDR_EXPR(expr)[i-1] );
	    }
	    
	    /* special case if subexpression is a record expression            */
	    else if ( TNUM_EXPR( ADDR_EXPR(expr)[i-1] ) == T_REC_EXPR ) {
	      sub = RecExpr1( ADDR_EXPR(expr)[i-1] );
	      SET_ELM_PLIST( list, i, sub );
	      CHANGED_BAG( list );
	      RecExpr2( sub, ADDR_EXPR(expr)[i-1] );
	    }
	    
	    /* general case                                                    */
	    else {
	      sub = EVAL_EXPR( ADDR_EXPR(expr)[i-1] );
	      SET_ELM_PLIST( list, i, sub );
	      CHANGED_BAG( list );
	    }
	  }

    }
    if (!posshole)
      SET_FILT_LIST(list, FN_IS_DENSE);

}


/****************************************************************************
**
*F  EvalRangeExpr(<expr>) . . . . .  eval a range expression to a range value
**
**  'EvalRangeExpr' evaluates the range expression <expr> to a range value.
*/
Obj             EvalRangeExpr (
    Expr                expr )
{
    Obj                 range;          /* range, result                   */
    Obj                 val;            /* subvalue of range               */
    Int                 low;            /* low (as C integer)              */
    Int                 inc;            /* increment (as C integer)        */
    Int                 high;           /* high (as C integer)             */

    /* evaluate the low value                                              */
    val = EVAL_EXPR( ADDR_EXPR(expr)[0] );
    while ( ! IS_INTOBJ(val) ) {
        val = ErrorReturnObj(
            "Range: <first> must be an integer less than 2^28 (not a %s)",
            (Int)TNAM_OBJ(val), 0L,
            "you can replace <first> via 'return <first>;'" );
    }
    low = INT_INTOBJ( val );

    /* evaluate the second value (if present)                              */
    if ( SIZE_EXPR(expr) == 3*sizeof(Expr) ) {
        val = EVAL_EXPR( ADDR_EXPR(expr)[1] );
        while ( ! IS_INTOBJ(val) || INT_INTOBJ(val) == low ) {
            if ( ! IS_INTOBJ(val) ) {
                val = ErrorReturnObj(
                    "Range: <second> must be an integer less than 2^28 (not a %s)",
                    (Int)TNAM_OBJ(val), 0L,
                    "you can replace <second> via 'return <second>;'" );
            }
            else {
                val = ErrorReturnObj(
                    "Range: <second> must not be equal to <first> (%d)",
                    (Int)low, 0L,
                    "you can replace the integer <second> via 'return <second>;'" );
            }
        }
        inc = INT_INTOBJ(val) - low;
    }
    else {
        inc = 1;
    }

    /* evaluate and check the high value                                   */
    val = EVAL_EXPR( ADDR_EXPR(expr)[ SIZE_EXPR(expr)/sizeof(Expr)-1 ] );
    while ( ! IS_INTOBJ(val) || (INT_INTOBJ(val) - low) % inc != 0 ) {
        if ( ! IS_INTOBJ(val) ) {
            val = ErrorReturnObj(
                "Range: <last> must be an integer less than 2^28 (not a %s)",
                (Int)TNAM_OBJ(val), 0L,
                "you can replace <last> via 'return <last>;'" );
        }
        else {
            val = ErrorReturnObj(
                "Range: <last>-<first> (%d) must be divisible by <inc> (%d)",
                (Int)(INT_INTOBJ(val)-low), (Int)inc,
                "you can replace the integer <last> via 'return <last>;'" );
        }
    }
    high = INT_INTOBJ(val);

    /* if <low> is larger than <high> the range is empty                   */
    if ( (0 < inc && high < low) || (inc < 0 && low < high) ) {
        range = NEW_PLIST( T_PLIST, 0 );
        SET_LEN_PLIST( range, 0 );
    }

    /* if <low> is equal to <high> the range is a singleton list           */
    else if ( low == high ) {
        range = NEW_PLIST( T_PLIST, 1 );
        SET_LEN_PLIST( range, 1 );
        SET_ELM_PLIST( range, 1, INTOBJ_INT(low) );
    }

    /* else make the range                                                 */
    else {
        if ( 0 < inc )
            range = NEW_RANGE_SSORT();
        else
            range = NEW_RANGE_NSORT();
        SET_LEN_RANGE( range, (high-low) / inc + 1 );
        SET_LOW_RANGE( range, low );
        SET_INC_RANGE( range, inc );
    }

    /* return the range                                                    */
    return range;
}


/****************************************************************************
**
*F  EvalStringExpr(<expr>)  . . . . eval string expressions to a string value
**
**  'EvalStringExpr'   evaluates the  string  expression  <expr>  to a string
**  value.
*/
Obj             EvalStringExpr (
    Expr                expr )
{
    Obj                 string;         /* string value, result            */
    UInt                 len;           /* size of expression              */
    
    len = *((UInt *)ADDR_EXPR(expr));
    string = NEW_STRING(len);
    memcpy((void *)ADDR_OBJ(string), (void *)ADDR_EXPR(expr), 
                      SIZEBAG_STRINGLEN(len) );

    /* return the string                                                   */
    return string;
}


/****************************************************************************
**
*F  EvalRecExpr(<expr>) . . . . . .  eval record expression to a record value
**
**  'EvalRecExpr' evaluates the record expression,   i.e., not yet  evaluated
**  record, <expr> to a record value.
**
**  'EvalRecExpr' just calls 'RecExpr1' and 'RecExpr2' to evaluate the record
**  expression.
*/
Obj             EvalRecExpr (
    Expr                expr )
{
    Obj                 rec;            /* record value, result            */

    /* evaluate the record expression                                      */
    rec = RecExpr1( expr );
    RecExpr2( rec, expr );

    /* return the result                                                   */
    return rec;
}


/****************************************************************************
**
*F  EvalRecTildeExpr(<expr>)  . . . evaluate a record expression with a tilde
**
**  'EvalRecTildeExpr'  evaluates  the    record expression,  i.e.,   not yet
**  evaluated   record, <expr>  to  a   record   value.  The   difference  to
**  'EvalRecExpr' is that in <expr> there are  occurences of '~' referring to
**  this record value.
**
**  'EvalRecTildeExpr' just  calls 'RecExpr1'  to create teh  record, assigns
**  the record to the variable '~',  and finally calls 'RecExpr2' to evaluate
**  the subexpressions  into the record.  Thus  subexpressions  in the record
**  expression    can refer to this variable    and its  subobjects to create
**  objects that are not trees.
*/
Obj             EvalRecTildeExpr (
    Expr                expr )
{
    Obj                 rec;            /* record value, result            */
    Obj                 tilde;          /* old value of tilde              */

    /* remember the old value of '~'                                       */
    tilde = VAL_GVAR( Tilde );

    /* create the record value                                             */
    rec = RecExpr1( expr );

    /* assign the record value to the variable '~'                         */
    AssGVar( Tilde, rec );

    /* evaluate the subexpressions into the record value                   */
    RecExpr2( rec, expr );

    /* restore the old value of '~'                                        */
    AssGVar( Tilde, tilde );

    /* return the record value                                             */
    return rec;
}


/****************************************************************************
**
*F  RecExpr1(<expr>)  . . . . . . . . . make a record for a record expression
*F  RecExpr2(<rec>,<expr>)  . .  enter the subobjects for a record expression
**
**  'RecExpr1' and 'RecExpr2' together  evaluate the record expression <expr>
**  into the record <rec>.
**
**  'RecExpr1' allocates   a new record  of the    same size as   the  record
**  expression <expr> and returns this record.
**
**  'RecExpr2' evaluates the subexpressions   of <expr> and puts the   values
**  into the record <rec>  (which should be a record  of the same size as the
**  record expression <expr>, e.g., the one allocated by 'RecExpr1').
**
**  This two step allocation is necessary, because record expressions such as
**  'rec(  a := 1,  ~.a  )' requires that the   value of one subexpression is
**  entered into the record value before the next subexpression is evaluated.
*/
Obj             RecExpr1 (
    Expr                expr )
{
    Obj                 rec;            /* record value, result            */
    Int                 len;            /* number of components            */

    /* get the number of components                                        */
    len = SIZE_EXPR( expr ) / (2*sizeof(Expr));

    /* allocate the record value                                           */
    rec = NEW_PREC( len );

    /* return the record                                                   */
    return rec;
}

void            RecExpr2 (
    Obj                 rec,
    Expr                expr )
{
    UInt                rnam;           /* name of component               */
    Obj                 sub;            /* value of subexpression          */
    Int                 len;            /* number of components            */
    Expr                tmp;            /* temporary variable              */
    Int                 i;              /* loop variable                   */

    /* get the number of components                                        */
    len = SIZE_EXPR( expr ) / (2*sizeof(Expr));

    /* handle the subexpressions                                           */
    for ( i = 1; i <= len; i++ ) {

        /* handle the name                                                 */
        tmp = ADDR_EXPR(expr)[2*i-2];
        if ( IS_INTEXPR(tmp) ) {
            rnam = (UInt)INT_INTEXPR(tmp);
        }
        else {
            rnam = RNamObj( EVAL_EXPR(tmp) );
        }
        SET_RNAM_PREC( rec, i, rnam );

        /* if the subexpression is empty (cannot happen for records)       */
        tmp = ADDR_EXPR(expr)[2*i-1];
        if ( tmp == 0 ) {
            continue;
        }

        /* special case if subexpression is a list expression             */
        else if ( TNUM_EXPR( tmp ) == T_LIST_EXPR ) {
            sub = ListExpr1( tmp );
            SET_ELM_PREC( rec, i, sub );
            CHANGED_BAG( rec );
            ListExpr2( sub, tmp );
        }

        /* special case if subexpression is a record expression            */
        else if ( TNUM_EXPR( tmp ) == T_REC_EXPR ) {
            sub = RecExpr1( tmp );
            SET_ELM_PREC( rec, i, sub );
            CHANGED_BAG( rec );
            RecExpr2( sub, tmp );
        }

        /* general case                                                    */
        else {
            sub = EVAL_EXPR( tmp );
            SET_ELM_PREC( rec, i, sub );
            CHANGED_BAG( rec );
        }

    }

}


/****************************************************************************
**
*F  PrintExpr(<expr>) . . . . . . . . . . . . . . . . . . print an expression
**
**  'PrintExpr' prints the expression <expr>.
**
**  'PrintExpr' simply dispatches  through  the table 'PrintExprFuncs' to the
**  appropriate printer.
*/
void            PrintExpr (
    Expr                expr )
{
    (*PrintExprFuncs[ TNUM_EXPR(expr) ])( expr );
}


/****************************************************************************
**
*V  PrintExprFuncs[<type>]  . .  printing function for objects of type <type>
**
**  'PrintExprFuncs' is the dispatching table that contains for every type of
**  expressions a pointer to the printer for expressions  of this type, i.e.,
**  the function that should be called to print expressions of this type.
*/
void            (* PrintExprFuncs[256] ) ( Expr expr );


/****************************************************************************
**
*F  PrintUnknownExpr(<expr>)  . . . . . . .  print expression of unknown type
**
**  'PrintUnknownExpr' is the printer that is called if an attempt is made to
**  print an expression <expr> of an unknown type.  It signals  an error.  If
**  this  is ever called,   then  GAP is  in  serious   trouble, such as   an
**  overwritten type field of an expression.
*/
void            PrintUnknownExpr (
    Expr                expr )
{
    Pr( "Panic: tried to print an expression of unknown type '%d'\n",
        (Int)TNUM_EXPR(expr), 0L );
}


/****************************************************************************
**
*V  PrintPreceedence  . . . . . . . . . . . . . . . current preceedence level
**
**  'PrintPreceedence' contains  the  current  preceedence   level,  i.e.  an
**  integer  indicating the binding power  of the currently printed operator.
**  If one of the operands is an operation that has lower binding power it is
**  printed in parenthesis.  If the right  operand has the same binding power
**  it is put in parenthesis, since  all the operations are left associative.
**  Preceedence: 14: ^; 12: mod,/,*; 10: -,+; 8: in,=; 6: not; 4: and; 2: or.
**  This sometimes puts in superflous parenthesis: 2 * f( (3 + 4) ), since it
**  doesn't know that a function call adds automatically parenthesis.
*/
UInt            PrintPreceedence;


/****************************************************************************
**
*F  PrintNot(<expr>)  . . . . . . . . . . . . .  print a boolean not operator
**
**  'PrintNot' print a not operation in the following form: 'not <expr>'.
*/
void            PrintNot (
    Expr                expr )
{
    UInt                oldPrec;

    oldPrec = PrintPreceedence;
    PrintPreceedence = 6;
    Pr("not%> ",0L,0L);
    PrintExpr( ADDR_EXPR(expr)[0] );
    Pr("%<",0L,0L);
    PrintPreceedence = oldPrec;
}


/****************************************************************************
**
*F  PrintBinop(<expr>)  . . . . . . . . . . . . . .  prints a binary operator
**
**  'PrintBinop'  prints  the   binary operator    expression <expr>,   using
**  'PrintPreceedence' for parenthesising.
*/
void            PrintAInv (
    Expr                expr )
{
    UInt                oldPrec;

    oldPrec = PrintPreceedence;
    PrintPreceedence = 14;
    Pr("-%> ",0L,0L);
    PrintExpr( ADDR_EXPR(expr)[0] );
    Pr("%<",0L,0L);
    PrintPreceedence = oldPrec;
}

void            PrintInv (
    Expr                expr )
{
    UInt                oldPrec;

    oldPrec = PrintPreceedence;
    PrintPreceedence = 14;
    Pr("%> ",0L,0L);
    PrintExpr( ADDR_EXPR(expr)[0] );
    Pr("%<^-1",0L,0L);
    PrintPreceedence = oldPrec;
}

void            PrintBinop (
    Expr                expr )
{
    UInt                oldPrec;        /* old preceedence level           */
    Char *              op;             /* operand                         */

    /* remember the current preceedence level                              */
    oldPrec = PrintPreceedence;

    /* select the new preceedence level                                    */
    switch ( TNUM_EXPR(expr) ) {
    case T_OR:     op = "or";   PrintPreceedence =  2;  break;
    case T_AND:    op = "and";  PrintPreceedence =  4;  break;
    case T_EQ:     op = "=";    PrintPreceedence =  8;  break;
    case T_LT:     op = "<";    PrintPreceedence =  8;  break;
    case T_GT:     op = ">";    PrintPreceedence =  8;  break;
    case T_NE:     op = "<>";   PrintPreceedence =  8;  break;
    case T_LE:     op = "<=";   PrintPreceedence =  8;  break;
    case T_GE:     op = ">=";   PrintPreceedence =  8;  break;
    case T_IN:     op = "in";   PrintPreceedence =  8;  break;
    case T_SUM:    op = "+";    PrintPreceedence = 10;  break;
    case T_DIFF:   op = "-";    PrintPreceedence = 10;  break;
    case T_PROD:   op = "*";    PrintPreceedence = 12;  break;
    case T_QUO:    op = "/";    PrintPreceedence = 12;  break;
    case T_MOD:    op = "mod";  PrintPreceedence = 12;  break;
    case T_POW:    op = "^";    PrintPreceedence = 16;  break;
    default:       op = "<bogus-operator>";   break;
    }

    /* if necessary print the opening parenthesis                          */
    if ( oldPrec > PrintPreceedence ) Pr("%>(%>",0L,0L);
    else Pr("%2>",0L,0L);

    /* print the left operand                                              */
    if ( TNUM_EXPR(expr) == T_POW
	 && ((  (IS_INTEXPR(ADDR_EXPR(expr)[0])
		 && INT_INTEXPR(ADDR_EXPR(expr)[0]) < 0)
		|| TNUM_EXPR(ADDR_EXPR(expr)[0]) == T_INTNEG)
	     || TNUM_EXPR(ADDR_EXPR(expr)[0]) == T_POW) ) {
        Pr( "(", 0L, 0L );
        PrintExpr( ADDR_EXPR(expr)[0] );
        Pr( ")", 0L, 0L );
    }
    else {
        PrintExpr( ADDR_EXPR(expr)[0] );
    }

    /* print the operator                                                  */
    Pr("%2< %2>%s%> %<",(Int)op,0L);

    /* print the right operand                                             */
    PrintPreceedence++;
    PrintExpr( ADDR_EXPR(expr)[1] );
    PrintPreceedence--;

    /* if necessary print the closing parenthesis                          */
    if ( oldPrec > PrintPreceedence ) Pr("%2<)",0L,0L);
    else Pr("%2<",0L,0L);

    /* restore the old preceedence level                                   */
    PrintPreceedence = oldPrec;
}


/****************************************************************************
**
*F  PrintIntExpr(<expr>)  . . . . . . . . . . . . print an integer expression
**
**  'PrintIntExpr' prints the literal integer expression <expr>.
*/
void            PrintIntExpr (
    Expr                expr )
{
    if ( IS_INTEXPR(expr) ) {
        Pr( "%d", INT_INTEXPR(expr), 0L );
    }
    else {
        Pr( "<<not yet implemented>>", 0L, 0L );
    }
}


/****************************************************************************
**
*F  PrintTrueExpr(<expr>) . . . . . . . . . . . print literal true expression
*/
void            PrintTrueExpr (
    Expr                expr )
{
    Pr( "true", 0L, 0L );
}


/****************************************************************************
**
*F  PrintFalseExpr(<expr>)  . . . . . . . . .  print literal false expression
*/
void            PrintFalseExpr (
    Expr                expr )
{
    Pr( "false", 0L, 0L );
}


/****************************************************************************
**
*F  PrintCharExpr(<expr>) . . . . . . . .  print literal character expression
*/
void            PrintCharExpr (
    Expr                expr )
{
    UChar               chr;

    chr = *(UChar*)ADDR_EXPR(expr);
    if      ( chr == '\n'  )  Pr("'\\n'",0L,0L);
    else if ( chr == '\t'  )  Pr("'\\t'",0L,0L);
    else if ( chr == '\r'  )  Pr("'\\r'",0L,0L);
    else if ( chr == '\b'  )  Pr("'\\b'",0L,0L);
    else if ( chr == '\03' )  Pr("'\\c'",0L,0L);
    else if ( chr == '\''  )  Pr("'\\''",0L,0L);
    else if ( chr == '\\'  )  Pr("'\\\\'",0L,0L);
    else                      Pr("'%c'",(Int)chr,0L);
}


/****************************************************************************
**
*F  PrintPermExpr(<expr>) . . . . . . . . . .  print a permutation expression
**
**  'PrintPermExpr' prints the permutation expression <expr>.
*/
void            PrintPermExpr (
    Expr                expr )
{
    Expr                cycle;          /* one cycle of permutation expr.  */
    UInt                i, j;           /* loop variables                  */

    /* if there are no cycles, print the identity permutation              */
    if ( SIZE_EXPR(expr) == 0 ) {
        Pr("()",0L,0L);
    }
    
    /* print all cycles                                                    */
    for ( i = 1; i <= SIZE_EXPR(expr)/sizeof(Expr); i++ ) {
        cycle = ADDR_EXPR(expr)[i-1];
        Pr("%>(",0L,0L);

        /* print all entries of that cycle                                 */
        for ( j = 1; j <= SIZE_EXPR(cycle)/sizeof(Expr); j++ ) {
            Pr("%>",0L,0L);
            PrintExpr( ADDR_EXPR(cycle)[j-1] );
            Pr("%<",0L,0L);
            if ( j < SIZE_EXPR(cycle)/sizeof(Expr) )  Pr(",",0L,0L);
        }

        Pr("%<)",0L,0L);
    }
}


/****************************************************************************
**
*F  PrintListExpr(<expr>) . . . . . . . . . . . . . . print a list expression
**
**  'PrintListExpr' prints the list expression <expr>.
*/
void            PrintListExpr (
    Expr                expr )
{
    Int                 len;            /* logical length of <list>        */
    Expr                elm;            /* one element from <list>         */
    Int                 i;              /* loop variable                   */

    /* get the logical length of the list                                  */
    len = SIZE_EXPR( expr ) / sizeof(Expr);

    /* loop over the entries                                               */
    Pr("%2>[ %2>",0L,0L);
    for ( i = 1;  i <= len;  i++ ) {
        elm = ADDR_EXPR(expr)[i-1];
        if ( elm != 0 ) {
            if ( 1 < i )  Pr("%<,%< %2>",0L,0L);
            PrintExpr( elm );
        }
        else {
            if ( 1 < i )  Pr("%2<,%2>",0L,0L);
        }
    }
    Pr(" %4<]",0L,0L);
}


/****************************************************************************
**
*F  PrintRangeExpr(<expr>)  . . . . . . . . . . . . .  print range expression
**
**  'PrintRangeExpr' prints the record expression <expr>.
*/
void            PrintRangeExpr (
    Expr                expr )
{
    if ( SIZE_EXPR( expr ) == 2*sizeof(Expr) ) {
        Pr("%2>[ %2>",0L,0L);    PrintExpr( ADDR_EXPR(expr)[0] );
        Pr("%2< .. %2>",0L,0L);  PrintExpr( ADDR_EXPR(expr)[1] );
        Pr(" %4<]",0L,0L);
    }
    else {
        Pr("%2>[ %2>",0L,0L);    PrintExpr( ADDR_EXPR(expr)[0] );
        Pr("%<,%< %2>",0L,0L);   PrintExpr( ADDR_EXPR(expr)[1] );
        Pr("%2< .. %2>",0L,0L);  PrintExpr( ADDR_EXPR(expr)[2] );
        Pr(" %4<]",0L,0L);
    }
}


/****************************************************************************
**
*F  PrintStringExpr(<expr>) . . . . . . . . . . . . print a string expression
**
**  'PrintStringExpr' prints the string expression <expr>.
*/
void            PrintStringExpr (
    Expr                expr )
{
    PrintString(EvalStringExpr(expr));
    /*Pr( "\"%S\"", (Int)ADDR_EXPR(expr), 0L );*/
}


/****************************************************************************
**
*F  PrintRecExpr(<expr>)  . . . . . . . . . . . . . print a record expression
**
**  'PrintRecExpr' the record expression <expr>.
*/
void            PrintRecExpr1 (
    Expr                expr )
{
  Expr                tmp;            /* temporary variable              */
  UInt                i;              /* loop variable                   */
  
  for ( i = 1; i <= SIZE_EXPR(expr)/(2*sizeof(Expr)); i++ ) {
        /* print an ordinary record name                                   */
        tmp = ADDR_EXPR(expr)[2*i-2];
        if ( IS_INTEXPR(tmp) ) {
            Pr( "%I", (Int)NAME_RNAM( INT_INTEXPR(tmp) ), 0L );
        }

        /* print an evaluating record name                                 */
        else {
            Pr(" (",0L,0L);
            PrintExpr( tmp );
            Pr(")",0L,0L);
        }

        /* print the component                                             */
        tmp = ADDR_EXPR(expr)[2*i-1];
        Pr("%< := %>",0L,0L);
        PrintExpr( tmp );
        if ( i < SIZE_EXPR(expr)/(2*sizeof(Expr)) )
            Pr("%2<,\n%2>",0L,0L);

    }
}

void            PrintRecExpr (
    Expr                expr )
{
    Pr("%2>rec(\n%2>",0L,0L);
    PrintRecExpr1(expr);
    Pr(" %4<)",0L,0L);

}


/****************************************************************************
**

*F * * * * * * * * * * * * * initialize package * * * * * * * * * * * * * * *
*/


/****************************************************************************
**

*F  InitKernel( <module> )  . . . . . . . . initialise kernel data structures
*/
static Int InitKernel (
    StructInitInfo *    module )
{
    UInt                type;           /* loop variable                   */

    /* clear the evaluation dispatch table                                 */
    for ( type = 0; type < 256; type++ ) {
        EvalExprFuncs[ type ] = EvalUnknownExpr;
        EvalBoolFuncs[ type ] = EvalUnknownBool;
    }

    /* install the evaluators for logical operations                       */
    EvalExprFuncs [ T_OR             ] = EvalOr;   
    EvalExprFuncs [ T_AND            ] = EvalAnd;  
    EvalExprFuncs [ T_NOT            ] = EvalNot;  

    /* the logical operations are guaranteed to return booleans            */
    EvalBoolFuncs [ T_OR             ] = EvalOr;
    EvalBoolFuncs [ T_AND            ] = EvalAnd;
    EvalBoolFuncs [ T_NOT            ] = EvalNot;

    /* install the evaluators for comparison operations                    */
    EvalExprFuncs [ T_EQ             ] = EvalEq;   
    EvalExprFuncs [ T_NE             ] = EvalNe;   
    EvalExprFuncs [ T_LT             ] = EvalLt;   
    EvalExprFuncs [ T_GE             ] = EvalGe;   
    EvalExprFuncs [ T_GT             ] = EvalGt;   
    EvalExprFuncs [ T_LE             ] = EvalLe;   
    EvalExprFuncs [ T_IN             ] = EvalIn;     

    /* the comparison operations are guaranteed to return booleans         */
    EvalBoolFuncs [ T_EQ             ] = EvalEq;
    EvalBoolFuncs [ T_NE             ] = EvalNe;
    EvalBoolFuncs [ T_LT             ] = EvalLt;
    EvalBoolFuncs [ T_GE             ] = EvalGe;
    EvalBoolFuncs [ T_GT             ] = EvalGt;
    EvalBoolFuncs [ T_LE             ] = EvalLe;
    EvalBoolFuncs [ T_IN             ] = EvalIn;

    /* install the evaluators for binary operations                        */
    EvalExprFuncs [ T_SUM            ] = EvalSum;
    EvalExprFuncs [ T_AINV           ] = EvalAInv;
    EvalExprFuncs [ T_DIFF           ] = EvalDiff;
    EvalExprFuncs [ T_PROD           ] = EvalProd;
    EvalExprFuncs [ T_INV            ] = EvalInv;
    EvalExprFuncs [ T_QUO            ] = EvalQuo;
    EvalExprFuncs [ T_MOD            ] = EvalMod;
    EvalExprFuncs [ T_POW            ] = EvalPow;

    /* install the evaluators for literal expressions                      */
    EvalExprFuncs [ T_INT_EXPR       ] = EvalIntExpr;
    EvalExprFuncs [ T_TRUE_EXPR      ] = EvalTrueExpr;
    EvalExprFuncs [ T_FALSE_EXPR     ] = EvalFalseExpr;
    EvalExprFuncs [ T_CHAR_EXPR      ] = EvalCharExpr;
    EvalExprFuncs [ T_PERM_EXPR      ] = EvalPermExpr;

    /* install the evaluators for list and record expressions              */
    EvalExprFuncs [ T_LIST_EXPR      ] = EvalListExpr;
    EvalExprFuncs [ T_LIST_TILD_EXPR ] = EvalListTildeExpr;
    EvalExprFuncs [ T_RANGE_EXPR     ] = EvalRangeExpr;
    EvalExprFuncs [ T_STRING_EXPR    ] = EvalStringExpr;
    EvalExprFuncs [ T_REC_EXPR       ] = EvalRecExpr;
    EvalExprFuncs [ T_REC_TILD_EXPR  ] = EvalRecTildeExpr;

    /* clear the tables for the printing dispatching                       */
    for ( type = 0; type < 256; type++ ) {
        PrintExprFuncs[ type ] = PrintUnknownExpr;
    }

    /* install the printers for logical operations                         */
    PrintExprFuncs[ T_OR             ] = PrintBinop;
    PrintExprFuncs[ T_AND            ] = PrintBinop;
    PrintExprFuncs[ T_NOT            ] = PrintNot;

    /* install the printers for comparison operations                      */
    PrintExprFuncs[ T_EQ             ] = PrintBinop;
    PrintExprFuncs[ T_LT             ] = PrintBinop;
    PrintExprFuncs[ T_NE             ] = PrintBinop;
    PrintExprFuncs[ T_GE             ] = PrintBinop;
    PrintExprFuncs[ T_GT             ] = PrintBinop;
    PrintExprFuncs[ T_LE             ] = PrintBinop;
    PrintExprFuncs[ T_IN             ] = PrintBinop;

    /* install the printers for binary operations                          */
    PrintExprFuncs[ T_SUM            ] = PrintBinop;
    PrintExprFuncs[ T_AINV           ] = PrintAInv;
    PrintExprFuncs[ T_DIFF           ] = PrintBinop;
    PrintExprFuncs[ T_PROD           ] = PrintBinop;
    PrintExprFuncs[ T_INV            ] = PrintInv;
    PrintExprFuncs[ T_QUO            ] = PrintBinop;
    PrintExprFuncs[ T_MOD            ] = PrintBinop;
    PrintExprFuncs[ T_POW            ] = PrintBinop;

    /* install the printers for literal expressions                        */
    PrintExprFuncs[ T_INTEXPR        ] = PrintIntExpr;
    PrintExprFuncs[ T_INT_EXPR       ] = PrintIntExpr;
    PrintExprFuncs[ T_TRUE_EXPR      ] = PrintTrueExpr;
    PrintExprFuncs[ T_FALSE_EXPR     ] = PrintFalseExpr;
    PrintExprFuncs[ T_CHAR_EXPR      ] = PrintCharExpr;
    PrintExprFuncs[ T_PERM_EXPR      ] = PrintPermExpr;

    /* install the printers for list and record expressions                */
    PrintExprFuncs[ T_LIST_EXPR      ] = PrintListExpr;
    PrintExprFuncs[ T_LIST_TILD_EXPR ] = PrintListExpr;
    PrintExprFuncs[ T_RANGE_EXPR     ] = PrintRangeExpr;
    PrintExprFuncs[ T_STRING_EXPR    ] = PrintStringExpr;
    PrintExprFuncs[ T_REC_EXPR       ] = PrintRecExpr;
    PrintExprFuncs[ T_REC_TILD_EXPR  ] = PrintRecExpr;

    /* return success                                                      */
    return 0;
}


/****************************************************************************
**
*F  InitInfoExprs() . . . . . . . . . . . . . . . . . table of init functions
*/
static StructInitInfo module = {
    MODULE_BUILTIN,                     /* type                           */
    "exprs",                            /* name                           */
    0,                                  /* revision entry of c file       */
    0,                                  /* revision entry of h file       */
    0,                                  /* version                        */
    0,                                  /* crc                            */
    InitKernel,                         /* initKernel                     */
    0,                                  /* initLibrary                    */
    0,                                  /* checkInit                      */
    0,                                  /* preSave                        */
    0,                                  /* postSave                       */
    0                                   /* postRestore                    */
};

StructInitInfo * InitInfoExprs ( void )
{
    module.revision_c = Revision_exprs_c;
    module.revision_h = Revision_exprs_h;
    FillInVersion( &module );
    return &module;
}


/****************************************************************************
**

*E  exprs.c . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
*/
