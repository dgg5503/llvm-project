#ifndef LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_INVALIDATION_CAUSE_H
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_INVALIDATION_CAUSE_H

#include "llvm/ADT/FoldingSet.h"

namespace clang {
namespace ento {

class InvalidationCause : public llvm::FoldingSetNode {
  virtual void anchor();
public:
  virtual ~InvalidationCause() = default;

  enum Kind {
#define INVALIDATION_CAUSE(Id, Parent) Id##Kind,
#define ABSTRACT_INVALIDATION_CAUSE(Id, Parent) Id##Kind,
#define INVALIDATION_CAUSE_RANGE(Id, First, Last) BEGIN_##Id = First, END_##Id = Last,
#include "clang/StaticAnalyzer/Core/PathSensitive/InvalidationCause.def"
  };

  Kind getKind() const { return K; }
  virtual void dump() const;
  virtual void dump(raw_ostream &OS) const;

  protected:
    explicit InvalidationCause(Kind K) : K(K) {}

  private:
    Kind K;
};

inline raw_ostream &operator<<(raw_ostream &OS,
                               const InvalidationCause *Invalidation) {
  Invalidation->dump(OS);
  return OS;
}

class UnmodeledCall : public InvalidationCause {
public:
  static bool classof(const InvalidationCause *C) {
    Kind k = C->getKind();
    return k >= BEGIN_UNMODELEDCALLS && k <= END_UNMODELEDCALLS;
  }

protected:
  explicit UnmodeledCall(Kind K) : InvalidationCause(K) {}
};

class UnmodeledStmt : public InvalidationCause {
public:
  static bool classof(const InvalidationCause *C) {
    Kind k = C->getKind();
    return k >= BEGIN_UNMODELEDSTMTS && k <= END_UNMODELEDSTMTS;
  }

protected:
  explicit UnmodeledStmt(Kind K) : InvalidationCause(K) {}
};

class ConservativeEvalCall : public UnmodeledCall {
public:
  ConservativeEvalCall() : UnmodeledCall(ConservativeEvalCallKind) {}

  static bool classof(const InvalidationCause *C) {
    return C->getKind() == ConservativeEvalCallKind;
  }
};

class PartiallyModeledCall : public UnmodeledCall {
public:
  PartiallyModeledCall() : UnmodeledCall(PartiallyModeledCallKind) {}

  static bool classof(const InvalidationCause *C) {
    return C->getKind() == PartiallyModeledCallKind;
  }
};

class UnmodeledExpr : public UnmodeledStmt {
public:
  UnmodeledExpr() : UnmodeledStmt(UnmodeledExprKind) {}

  static bool classof(const InvalidationCause *C) {
    return C->getKind() == UnmodeledExprKind;
  }
};

class LoopWidening : public UnmodeledStmt {
public:
  LoopWidening() : UnmodeledStmt(LoopWideningKind) {}

  static bool classof(const InvalidationCause *C) {
    return C->getKind() == LoopWideningKind;
  }
};

} // namespace ento
} // namespace clang

#endif // LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_INVALIDATION_CAUSE_H
