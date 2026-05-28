# Agentic Risk-To-Test Matrix

This matrix maps high-risk agentic dialog/autonomy failure modes to automated checks.

## Scope

- Dialog understanding and clarification loops
- Autonomous tool execution and guardrails
- Trust-boundary handling for untrusted tool/retrieval content
- Side-effect safety: idempotency and rollback outcomes

## Matrix

| Risk | OWASP / Category | Control | Automated coverage |
| --- | --- | --- | --- |
| Indirect prompt injection via tool output | LLM01 / Prompt Injection | Sanitize untrusted tool payloads before re-prompting model | `Test_LLM_Orchestrator` (tool round-trip paths), `Test_LLM_OrchestratorLifecycleArgs` |
| Unsafe repeated write action | LLM06 / Excessive Agency | Idempotency key propagation + gateway idempotency cache | `Test_LLM_ToolGateway.IdempotencyReturnsCachedResult` |
| Lost clarification state after restart | Reliability / Dialog Continuity | Persist `pending_tool_arguments` in session store | `Test_LLM_SessionPersist.RoundTripPendingToolArguments` |
| Sensitive text persisted to disk | LLM02 / Sensitive Disclosure | Redact common secret tokens before session persistence | `Test_LLM_SessionPersist.RedactsSensitiveMessageContentOnPersist` |
| Rollback reported as success when compensation fails | Reliability / Recovery integrity | Strict rollback status and explicit failure surfaces | `Test_LLM_Orchestrator.RollbackReportsFailureWhenCompensationFails` |
| Invalid schema argument accepted | LLM05 / Output Handling | Strict type/enum/range checks in validator | `Test_LLM_ArgumentValidator.EnforcesTypeEnumAndNumericRange` |
| Cancellation of one dialog affects another | Reliability / Isolation | Session-scoped cancel entrypoint from GUI to orchestrator | Unit coverage pending (see TECH-DEBT TD-071) |
| Transport retries amplify instability | LLM10 / Unbounded Consumption | Bounded retries + exponential backoff | `Test_LLM_OllamaNative`, `Test_LLM_HttpSse` smoke coverage |

## Gaps and deferred items

- Add dedicated multi-session cancellation isolation test (requires deterministic concurrent provider fixture).
- Add adversarial fixtures for retrieval/tool payload prompt-injection mutations.
- Add CI gate that fails release on matrix regressions for P0/P1 rows.
