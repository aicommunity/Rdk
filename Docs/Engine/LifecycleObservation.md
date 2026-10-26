# Process-local lifecycle observations

`UModule::ObserveLifecycle()` returns a non-owning observation of a component's
native lifecycle. `IsCurrent()` becomes false before Default, Build, Reset, Init
or UnInit hooks are entered, on C++ assignment, or when the component is destroyed.
`IsAlive()` distinguishes destruction from a lifecycle change on a live object.
The handle owns only observation metadata, not the component. It can safely be
examined after component deletion; it must not be used to dereference a deleted
`UEPtr`, which is a weak pointer.

Calculate does not invalidate observations. Native no-op calls that return before
the corresponding before-hook do not invalidate them either. A Reset attempt
on an uninitialized UContainer does enter BeforeReset and therefore invalidates
an observation even when the initialization guard subsequently skips AReset.
An invalid observation proves that a lifecycle boundary was entered; it does
not prove that construction, initialization or reset completed successfully.

The API adds no Build/Reset/Calculate calls, changes no native order, and does not
add error propagation through their return values. The invalidation counter is
not a number of successful resets: Default/Build may enter multiple hooks, and
the existing AfterBuild -> Reset path remains unchanged. Counter overflow makes
observations unusable conservatively instead of allowing an old handle to match.

Observation state is never serialized and is excluded from mathematical model
fingerprints. A C++ copy has its own observation identity; framework Copy still
has its existing graph and dynamics semantics. This API is neither a checkpoint
nor a record of parameter writes. A caller measuring a frozen physical model
must check both semantic parameters/bindings and lifecycle observations. A reset
can otherwise change physical history while leaving the semantic hash unchanged.

Use from the component's normal scheduling/synchronization context. Atomic
observation fields prevent torn metadata reads; they do not authorize concurrent
Reset/Calculate or protect the component's other mutable data.
