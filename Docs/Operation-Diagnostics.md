# Native operation diagnostics

`UContainer` catches component exceptions during Default, Build, Init, Reset,
Calculate and UnInit. The existing lifecycle, scheduler, logging and return-value
contract of Reset/Calculate remain unchanged. Build's legacy caught-exception
false return is corrected to the user-required true artifact as a separate
change. Ready remains false on a caught local Build exception. Lifecycle bools
are not an error channel.

Connection/structure APIs retain their own meaningful success/failure results.
Their previous checks of Build's bool use the operation marker instead, so a
failed Build still refuses linking or structure creation. This also preserves
the existing no-storage no-op behavior; no unconditional IsReady requirement
is added. Environment forwards Model/subcomponent Build failures when those
objects are separate ownership roots. The ABuild hook's existing return and
native invocation order are retained.

Each caught exception records its operation and component-qualified reason in a
process-local ledger on that container and its current owner chain. A caller
captures `OperationFailureSequence()` before a transaction, then calls
`HasOperationFailureSince(marker)` afterwards. `LastOperationFailure()` returns
a coherent snapshot of the latest recorded failure. Subsequent successful calls
do not erase history. An overflow permanently makes the comparison fail closed.

The ledger is independent of lifecycle revision and is not a property, model
parameter, serialized state or checkpoint. Copy construction creates independent
history; assignment preserves the target's history. Recording is synchronized;
this does not make concurrent graph mutation or calculation supported. Owner
traversal follows the existing synchronous component ownership contract.

This detects caught exceptions, not every possible numerical/configuration
error. Components must still expose their specialized validity diagnostics.
Direct UModule overrides that handle their own exceptions may use the protected
RecordOperationFailure method. No before-hooks, extra Build or Reset calls are
introduced by the ledger.

Unit coverage checks retention, copy/assignment, independence from lifecycle and
coherent concurrent recording. Real storage/neuron fault-injection tests belong
to the parent project's native integration gate.

Scoped server acceptance (2026-11-02): the corrected Core at
`c3850711b2cd73f33956aae18a8db6cd1c4ead41` passed 9/9 unit and 13/13 native
integration cases in normal Elder and M40 ASan builds. Parent Nmsdk evidence
commits are `094bced1b7431e726b052e315eceb2e04e2ad277` and
`12d5a24605bd464f0471a4b044f198c8d1b19763`. The latter preserves actual
libasan mappings for both processes; all 237 archived members were verified.
All 74 corresponding CSV/probe records match the normal run. Native cases
exercise caught Build/Reset/Calculate faults, preserved connection and
Environment failure results, joint physical edit rollback and real neurons.
Earlier evidence retaining Build's false return is a baseline, not acceptance
of the corrected contract. This acceptance covers the operation ledger and
listed consumers, not the entire Rdk lifecycle/profile matrix or completion of
the parent TimeLearner refactoring plan. Core has not changed since c3850711.
