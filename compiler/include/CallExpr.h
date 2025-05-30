/*
 * Copyright 2020-2025 Hewlett Packard Enterprise Development LP
 * Copyright 2004-2019 Cray Inc.
 * Other additional copyright holders may be indicated within.
 *
 * The entirety of this work is licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CALL_EXPR_H_
#define _CALL_EXPR_H_

#include "expr.h"

enum TryTag {
  TRY_TAG_NONE,
  TRY_TAG_IN_TRY,
  TRY_TAG_IN_TRYBANG
};

class CallExpr final : public Expr {
public:
  PrimitiveOp* primitive;        // primitive expression (baseExpr == NULL)
  Expr*        baseExpr;         // function expression

  AList        argList;          // function actuals

  bool         partialTag;
  bool         methodTag;        // Set to true if the call is a method call.
  bool         square;           // true if call made with square brackets
  TryTag       tryTag;

  CallExpr(BaseAST*     base,
           BaseAST*     arg1 = NULL,
           BaseAST*     arg2 = NULL,
           BaseAST*     arg3 = NULL,
           BaseAST*     arg4 = NULL,
           BaseAST*     arg5 = NULL);

  CallExpr(PrimitiveOp* prim,
           BaseAST*     arg1 = NULL,
           BaseAST*     arg2 = NULL,
           BaseAST*     arg3 = NULL,
           BaseAST*     arg4 = NULL,
           BaseAST*     arg5 = NULL);

  CallExpr(PrimitiveTag prim,
           BaseAST*     arg1 = NULL,
           BaseAST*     arg2 = NULL,
           BaseAST*     arg3 = NULL,
           BaseAST*     arg4 = NULL,
           BaseAST*     arg5 = NULL);

  CallExpr(const char*  name,
           BaseAST*     arg1 = NULL,
           BaseAST*     arg2 = NULL,
           BaseAST*     arg3 = NULL,
           BaseAST*     arg4 = NULL,
           BaseAST*     arg5 = NULL);

  ~CallExpr() override = default;

  void    verify() override;

  DECLARE_COPY(CallExpr);
  CallExpr* copyInner(SymbolMap* map) override;


  void    accept(AstVisitor* visitor) override;

  GenRet  codegen() override;
  void    prettyPrint(std::ostream* o) override;
  QualifiedType qualType() override;

  void    replaceChild(Expr* old_ast, Expr* new_ast) override;
  Expr*   getFirstExpr() override;
  Expr*   getNextExpr(Expr* expr) override;

  void            insertAtHead(BaseAST* ast);
  void            insertAtTail(BaseAST* ast);

  // True if the callExpr has been emptied (aka dead)
  bool            isEmpty()                                              const;

  bool            isCast();
  Expr*           castFrom();
  Expr*           castTo();

  bool            isPrimitive()                                          const;
  bool            isPrimitive(PrimitiveTag primitiveTag)                 const;
  bool            isPrimitive(const char*  primitiveName)                const;

  void            setUnresolvedFunction(const char* name);

  bool            isResolved()                                           const;
  FnSymbol*       resolvedFunction()                                     const;
  void            setResolvedFunction(FnSymbol* fn);
  FnSymbol*       resolvedOrVirtualFunction()                            const;

  // An indirect call is _only_ one in which the base expression of the call
  // is a value with a function type, but is not a use of a 'FnSymbol'. That
  // is, the type of the function is known, but not exactly which function
  // the call refers to.
  bool isIndirectCall()                                                  const;

  // Return the function type representing the function used to make the call.
  FunctionType* functionType()                                           const;

  FnSymbol*       theFnSymbol()                                          const;

  bool            isNamed(const char*)                                   const;
  bool            isNamedAstr(const char*)                               const;

  int             numActuals()                                           const;
  Expr*           get(int index)                                         const;
  FnSymbol*       findFnSymbol();

  void            convertToNoop();

  static void     registerPrimitivesForCodegen();

private:
  GenRet          codegenPrimitive();
  GenRet          codegenPrimMove();


  // Declare CallExpr::codegenPRIM_UNKNOWN() etc
#define PRIMITIVE_G(NAME, str) static void codegen ## NAME (CallExpr*, GenRet&);
#define PRIMITIVE_R(NAME, str)
#include "chpl/uast/prim-ops-list.h"
#undef PRIMITIVE_G
#undef PRIMITIVE_R

  void            codegenInvokeOnFun();
  void            codegenInvokeTaskFun(const char* name);

  GenRet          codegenBasicPrimitiveExpr()                            const;

public:
  bool            isRefExternStarTuple(Symbol* formal, Expr* actual)     const;
};

CallExpr* callChplHereAlloc(Type* type, VarSymbol* md = NULL);
CallExpr* callChplHereAllocWithAllocator(Type* type, Expr* allocator, VarSymbol* md = NULL);

void      insertChplHereAlloc(Expr*      call,
                              bool       insertAfter,
                              Symbol*    sym,
                              Type*      t,
                              VarSymbol* md = NULL);

CallExpr* callChplHereFree(BaseAST* p);

CallExpr* createCast(BaseAST* src, BaseAST* toType);

FnSymbol* resolvedToTaskFun(CallExpr* call);

static inline bool isEndOfStatementMarker(Expr* e) {
  if (CallExpr* call = toCallExpr(e))
    if (call->isPrimitive(PRIM_END_OF_STATEMENT))
      return true;

  return false;
}

inline FnSymbol* CallExpr::resolvedFunction() const {
  if (SymExpr* base = toSymExpr(baseExpr))
    return toFnSymbol(base->symbol());
  else
    return NULL;
}

inline FnSymbol* CallExpr::theFnSymbol() const {
  return resolvedFunction();
}

inline bool CallExpr::isResolved() const {
  return resolvedFunction() != NULL;
}

inline bool CallExpr::isEmpty() const {
  return primitive == NULL && baseExpr == NULL;
}

inline bool CallExpr::isPrimitive() const {
  return primitive != NULL;
}

inline bool CallExpr::isPrimitive(PrimitiveTag primitiveTag) const {
  return primitive && primitive->tag == primitiveTag;
}

inline bool CallExpr::isPrimitive(const char* primitiveName) const {
  return primitive && !strcmp(primitive->name, primitiveName);
}

inline bool CallExpr::isIndirectCall() const {
  // Eliminate edge cases first (e.g., primitives, direct call).
  if (!baseExpr || resolvedFunction() || isPrimitive()) return false;

  // Otherwise, if the base expression has a function type...
  if (isFunctionType(baseExpr->qualType().type()->getValType())) {
    return true;
  }

  return false;
}

inline FunctionType* CallExpr::functionType() const {
  if (isIndirectCall()) {
    // For indirect calls, grab the type of the base expression.
    auto ret = toFunctionType(baseExpr->qualType().type()->getValType());
    INT_ASSERT(ret);
    return ret;

  } else if (auto fn = resolvedFunction()) {
    // The call is a direct call, so compute the function's type...
    auto ret = fn->computeAndSetType();
    return ret;
  }

  return nullptr;
}

inline int CallExpr::numActuals() const {
  return argList.length;
}

inline Expr* CallExpr::get(int index) const {
  return argList.get(index);
}

// TODO: rename these
bool isInitOrReturn(CallExpr* call, SymExpr*& lhsSe, CallExpr*& initOrCtor);
bool isRecordInitOrReturn(CallExpr* call, SymExpr*& lhsSe, CallExpr*& initOrCtor);

#endif
