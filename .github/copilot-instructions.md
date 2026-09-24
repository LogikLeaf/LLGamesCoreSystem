# C++ Engineering Rules

## Mission

Write C++ that is **correct, safe, simple, maintainable, testable, efficient by default, and idiomatic modern C++**.

Do not optimize for:

* shortest code;
* fastest implementation;
* familiarity;
* compatibility with outdated C++ idioms;
* cleverness;
* matching random code found online.

Optimize for **long-term code quality and correctness**.

When several valid implementations exist, choose the one with the strongest safety, ownership clarity, invariants, maintainability, and correctness properties.

---

## 1. Modern C++ by default

* Use the project's configured C++ standard and its modern facilities.
* Prefer standard-library facilities over handwritten equivalents.
* Do not reproduce C++03-era patterns when a superior modern C++ solution exists.
* Never use a language feature merely because it exists. Use it when it makes the code clearer, safer, or more correct.
* Prefer simple abstractions over clever abstractions.
* Do not introduce abstraction without a concrete design benefit.

---

## 2. Ownership and lifetime are explicit

* Every resource must have a clear owner.
* Prefer RAII for every resource whose lifetime must be managed.
* Prefer automatic storage duration when possible.
* Prefer `std::unique_ptr` for exclusive ownership.
* Use `std::shared_ptr` only when shared ownership is genuinely required.
* Never use `std::shared_ptr` merely as a convenient way to avoid thinking about ownership.
* Never manually `new`/`delete` in ordinary application code.
* Avoid owning raw pointers.
* Raw pointers and references should normally represent non-owning relationships.
* Make object lifetime obvious from types and scope.
* Prevent dangling references, pointers, iterators, and views.
* Pay particular attention to lifetime extension, temporaries, container reallocation, and returned references/views.

---

## 3. RAII and exception safety

* Acquire resources in constructors and release them in destructors.
* Never rely on manual cleanup after an operation that can fail.
* Prefer scope-based lifetime management.
* Preserve invariants during construction and destruction.
* Prefer designs providing the strong exception guarantee when reasonably achievable.
* At minimum, maintain basic exception safety.
* Never write code that leaks resources when an exception is thrown.
* Use `noexcept` when a function is genuinely non-throwing and its contract benefits from that guarantee.
* Do not add `noexcept` speculatively.

---

## 4. Interfaces must express intent

* Design interfaces around invariants and ownership, not implementation details.
* Make invalid states difficult or impossible to represent.
* Prefer types that encode semantic constraints.
* Use references when null is not a valid state.
* Use pointers when absence/non-ownership is meaningful.
* Use `std::optional` when "value may be absent" is part of the API contract.
* Use `std::span` or equivalent non-owning views when appropriate instead of raw pointer + size pairs.
* Prefer strongly typed interfaces over primitive-heavy interfaces.
* Avoid boolean parameters when they obscure meaning; prefer an expressive type or named abstraction.
* Minimize the number of parameters and responsibilities of functions.
* Keep interfaces stable and implementations replaceable.

---

## 5. Const correctness

* Use `const` aggressively where it expresses an actual invariant.
* Functions that do not modify an object should normally be `const`.
* Prefer passing read-only objects by `const&` when appropriate.
* Do not cast away `const`.
* Do not use `mutable` unless there is a clear, justified semantic reason.
* Treat const-correctness as part of the API contract.

---

## 6. Value semantics first

* Prefer values and value semantics when practical.
* Prefer direct construction and clear ownership over unnecessary indirection.
* Do not introduce dynamic allocation without a reason.
* Do not use pointers or references merely because they are familiar.
* Prefer move semantics where they naturally improve ownership transfer or performance.
* Understand whether a type should be copyable, movable, both, or neither.
* Explicitly default or delete special member functions when doing so communicates the intended semantics.
* Follow the Rule of Zero whenever possible.

---

## 7. Containers and algorithms

* Prefer standard containers over custom data structures unless there is a demonstrated requirement.
* Prefer algorithms and standard-library facilities over handwritten loops when they make intent clearer.
* Prefer range-based and ranges-based solutions where appropriate.
* Avoid indexing when iteration expresses the intent better.
* Use iterators, ranges, views, and algorithms carefully with respect to lifetime.
* Choose containers based on semantic and performance requirements, not habit.
* Do not use `std::vector` automatically when another container better expresses the requirements.
* Avoid unnecessary copies.
* Do not sacrifice readability for micro-optimizations without evidence.

---

## 8. Type deduction

* Use `auto` when it improves correctness, readability, or prevents unnecessary type coupling.
* Do not use `auto` when it hides important semantic information.
* Always understand the type being deduced.
* Be especially careful with references, `const`, proxy types, initializer lists, forwarding references, and expression templates.
* Never use `auto` as an excuse not to understand the resulting type.
* Prefer explicit types when the type itself communicates important domain meaning.

---

## 9. Initialization and modern language constructs

* Prefer safe, explicit initialization.
* Use `nullptr`, never `0` or `NULL`, for null pointers.
* Prefer scoped enumerations (`enum class`) over unscoped enumerations unless interoperability requires otherwise.
* Prefer `using` aliases over `typedef`.
* Prefer `override` for overriding virtual functions.
* Use `final` only when it communicates an intentional design constraint.
* Prefer `constexpr` when compile-time evaluation is genuinely useful or expresses an invariant.
* Use `[[nodiscard]]` for functions whose ignored result would commonly indicate a bug.
* Use structured bindings, lambdas, concepts, ranges, and other modern facilities when they improve the design rather than merely demonstrating language features.

---

## 10. Polymorphism and inheritance

* Do not use inheritance merely for code reuse.
* Prefer composition when it expresses the relationship better.
* Use inheritance primarily to model a genuine substitutable abstraction.
* Base classes intended for polymorphic destruction must have an appropriate virtual destructor.
* Mark overrides explicitly with `override`.
* Keep inheritance hierarchies shallow unless there is a strong design reason otherwise.
* Do not expose implementation inheritance when composition would provide stronger encapsulation.

---

## 11. Templates and generic programming

* Write generic code only when genericity provides real value.
* Prefer constraints that clearly express requirements.
* Use concepts when they make template interfaces clearer and diagnostics better.
* Avoid unconstrained templates that accept meaningless types.
* Keep template machinery understandable.
* Do not use advanced template techniques merely because they are possible.
* Prefer compile-time guarantees over runtime validation when the constraint is naturally static.
* Preserve readable diagnostics and maintainability.

---

## 12. Error handling

* Make failure modes explicit.
* Do not silently ignore errors.
* Use exceptions when they are consistent with the project's error-handling model and appropriate for exceptional failure.
* Use return-value/error types when the project explicitly follows that model or when failure is an expected part of normal control flow.
* Never mix incompatible error-handling conventions arbitrarily.
* Do not use exceptions for ordinary control flow.
* Do not return sentinel values when a type such as `std::optional` or an explicit error representation communicates the contract better.

---

## 13. Concurrency

* Prefer higher-level concurrency abstractions over manually managed threads.
* Prefer task-based designs when appropriate.
* Minimize shared mutable state.
* Make ownership and synchronization requirements explicit.
* Never introduce a data race.
* Do not use `volatile` for thread synchronization.
* Use atomics only when their memory-ordering semantics are understood and required.
* Prefer immutable data and message passing where practical.
* Make thread-safety part of the API contract when relevant.

---

## 14. Performance

* Write code that is efficient by default, not prematurely optimized.
* Understand object lifetime, allocation, copying, moving, and cache behavior before optimizing.
* Avoid unnecessary allocations and copies.
* Prefer algorithms and data structures appropriate to the actual workload.
* Do not perform speculative micro-optimizations.
* When performance is a requirement, measure before and after optimization.
* Never trade correctness, safety, or maintainability for an unmeasured performance hypothesis.
* Treat compile-time performance as a real engineering concern for large codebases.

---

## 15. Simplicity and readability

* Prefer the simplest implementation that fully satisfies the requirements.
* Make the common case obvious.
* Minimize hidden behavior.
* Avoid clever one-liners when several simple statements are clearer.
* Avoid unnecessary macros.
* Avoid global mutable state.
* Avoid magic numbers and unexplained constants.
* Keep functions focused.
* Keep classes cohesive.
* Separate responsibilities rather than accumulating unrelated behavior.

---

## 16. Comments and documentation

* Code should communicate intent through names, types, structure, and invariants.
* Do not write comments that merely restate the code.
* Document non-obvious decisions, invariants, ownership assumptions, constraints, and reasons.
* When code looks unusual, explain why it is necessary.
* Do not use comments to justify unsafe or unnecessarily complicated code when the design itself can be improved.

---

## 17. Tooling and verification

Before considering C++ code complete:

* Compile it with the project's warnings enabled.
* Treat warnings as defects unless explicitly justified.
* Prefer static analysis where available.
* Use sanitizers when appropriate.
* Add or update tests for behavioral changes.
* Check lifetime, ownership, exception safety, and concurrency implications.
* Do not assume code is correct because it compiles.
* Do not assume code is good because tests pass.

---

## 18. Dependency discipline

* Prefer the standard library when it provides an adequate solution.
* Do not introduce a dependency for functionality that is trivial, already available, or unnecessary.
* Before proposing a dependency, consider its maintenance, API quality, compile-time cost, portability, licensing, and long-term risk.
* Avoid dependency-driven architecture.

---

## 19. When modifying existing code

* Preserve existing behavior unless the task explicitly changes it.
* First understand the surrounding architecture, ownership model, invariants, and error-handling conventions.
* Do not blindly imitate nearby legacy code.
* If existing code violates these rules, do not propagate the violation merely for consistency.
* Improve local design when doing so is safe and directly relevant.
* Keep changes focused; do not perform unrelated refactors unless requested.
* Do not introduce a worse design merely because it requires fewer changed lines.

---

## 20. Decision hierarchy

When choosing between implementations, evaluate them in this order:

1. Correctness
2. Safety and absence of undefined behavior
3. Clear ownership and lifetime
4. Strong invariants and type safety
5. Simplicity
6. Maintainability
7. Testability
8. Appropriate performance
9. Compile-time and binary-size considerations
10. Convenience

Never choose a solution primarily because it is shorter or commonly seen online.

---

## 21. Anti-patterns to reject

Do not generate code containing these patterns without a compelling, explicit justification:

* manual `new`/`delete`;
* owning raw pointers;
* unnecessary `shared_ptr`;
* naked resource handles without RAII;
* C-style casts;
* `NULL` or `0` as null pointers;
* C-style arrays where safer standard abstractions apply;
* unnecessary macros;
* global mutable state;
* unchecked ownership transfer;
* unexplained `const_cast`;
* `reinterpret_cast` without a strong technical justification;
* `volatile` used for synchronization;
* duplicated handwritten algorithms already provided by the standard library;
* premature micro-optimizations;
* deeply nested control flow;
* overly clever template metaprogramming;
* silent error handling;
* ignored return values where failure matters;
* unexplained magic constants;
* unnecessary dynamic allocation;
* unnecessary copying;
* inheritance used only for implementation reuse.

---

## 22. Required behavior when uncertain

If multiple approaches are plausible:

1. Identify the ownership and lifetime model.
2. Identify the invariants.
3. Prefer the solution that makes invalid states harder to represent.
4. Prefer standard-library facilities.
5. Prefer RAII and value semantics.
6. Prefer compile-time guarantees over runtime checks when appropriate.
7. Prefer the simplest design that preserves these guarantees.
8. Consider exception safety and failure paths.
9. Consider performance only after correctness and design quality.
10. If an important trade-off cannot be resolved from the existing codebase, ask rather than silently choosing a questionable design.

Do not invent requirements.

Do not copy an implementation pattern simply because it is common.

Do not optimize for "working code" alone.

The goal is **excellent C++**, not merely compilable C++.
