# Native operation diagnostics

`UContainer` catches component exceptions during Default, Build, Init, Reset,
Calculate and UnInit. The existing lifecycle, scheduler, logging and return-value
contract remain unchanged. In particular Build/Reset/Calculate return values
are not an error channel.

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
to the parent project's native integration gate. Server validation is required
before accepting this change into develop.
