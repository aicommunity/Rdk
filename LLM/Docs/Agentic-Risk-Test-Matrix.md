# Agentic Risk-To-Test Matrix

## RU

This matrix maps high-risk agentic dialog/autonomy failure modes to automated checks.

## Scope

- Dialog understanding and clarification loops
- Autonomous tool execution and guardrails
- Trust-boundary handling for untrusted tool/retrieval content
- Side-effect safety: idempotency and rollback outcomes

## Matrix

| Risk | OWASP / Category | Control | Automated coverage |
| --- | --- | --- | --- |
| Indirect prompt injection via tool output | LLM01 / Prompt Injection | Sanitize untrusted tool payloads before re-prompting model | `Test_LLM_AdversarialFixtures`, `Test_LLM_Orchestrator` (tool round-trip paths), `Test_LLM_OrchestratorLifecycleArgs` |
| Unsafe repeated write action | LLM06 / Excessive Agency | Idempotency key propagation + gateway idempotency cache | `Test_LLM_ToolGateway.IdempotencyReturnsCachedResult` |
| Lost clarification state after restart | Reliability / Dialog Continuity | Persist `pending_tool_arguments` in session store | `Test_LLM_SessionPersist.RoundTripPendingToolArguments` |
| Sensitive text persisted to disk | LLM02 / Sensitive Disclosure | Redact common secret tokens before session persistence | `Test_LLM_SessionPersist.RedactsSensitiveMessageContentOnPersist` |
| Rollback reported as success when compensation fails | Reliability / Recovery integrity | Strict rollback status and explicit failure surfaces | `Test_LLM_Orchestrator.RollbackReportsFailureWhenCompensationFails` |
| Invalid schema argument accepted | LLM05 / Output Handling | Strict type/enum/range checks in validator | `Test_LLM_ArgumentValidator.EnforcesTypeEnumAndNumericRange` |
| Cancellation of one dialog affects another | Reliability / Isolation | Session-scoped cancel entrypoint from GUI to orchestrator | `Test_LLM_Orchestrator.CancelSessionDoesNotAffectOtherSessions` (TD-071 done) |
| Transport retries amplify instability | LLM10 / Unbounded Consumption | Bounded retries + exponential backoff | `Test_LLM_OllamaNative`, `Test_LLM_HttpSse` smoke coverage |
| P0/P1 matrix regression on release | Reliability / CI | `Scripts/ci-llm-agentic-risk.sh` + `agentic-risk/manifest.json` | Wired into `ci-llm-linux.sh` (TD-073) |

## Gaps and deferred items

Tracked in [Post-Audit-Phase-2-Implementation-Plan.md](Post-Audit-Phase-2-Implementation-Plan.md): remaining stretch items only (if any).

---

## EN

This matrix maps high-risk agentic dialog/autonomy failure modes to automated checks.

## Scope

- Dialog understanding and clarification loops
- Autonomous tool execution and guardrails
- Trust-boundary handling for untrusted tool/retrieval content
- Side-effect safety: idempotency and rollback outcomes

## Matrix

| Risk | OWASP / Category | Control | Automated coverage |
| --- | --- | --- | --- |
| Indirect prompt injection via tool output | LLM01 / Prompt Injection | Sanitize untrusted tool payloads before re-prompting model | `Test_LLM_AdversarialFixtures`, `Test_LLM_Orchestrator` (tool round-trip paths), `Test_LLM_OrchestratorLifecycleArgs` |
| Unsafe repeated write action | LLM06 / Excessive Agency | Idempotency key propagation + gateway idempotency cache | `Test_LLM_ToolGateway.IdempotencyReturnsCachedResult` |
| Lost clarification state after restart | Reliability / Dialog Continuity | Persist `pending_tool_arguments` in session store | `Test_LLM_SessionPersist.RoundTripPendingToolArguments` |
| Sensitive text persisted to disk | LLM02 / Sensitive Disclosure | Redact common secret tokens before session persistence | `Test_LLM_SessionPersist.RedactsSensitiveMessageContentOnPersist` |
| Rollback reported as success when compensation fails | Reliability / Recovery integrity | Strict rollback status and explicit failure surfaces | `Test_LLM_Orchestrator.RollbackReportsFailureWhenCompensationFails` |
| Invalid schema argument accepted | LLM05 / Output Handling | Strict type/enum/range checks in validator | `Test_LLM_ArgumentValidator.EnforcesTypeEnumAndNumericRange` |
| Cancellation of one dialog affects another | Reliability / Isolation | Session-scoped cancel entrypoint from GUI to orchestrator | `Test_LLM_Orchestrator.CancelSessionDoesNotAffectOtherSessions` (TD-071 done) |
| Transport retries amplify instability | LLM10 / Unbounded Consumption | Bounded retries + exponential backoff | `Test_LLM_OllamaNative`, `Test_LLM_HttpSse` smoke coverage |
| P0/P1 matrix regression on release | Reliability / CI | `Scripts/ci-llm-agentic-risk.sh` + `agentic-risk/manifest.json` | Wired into `ci-llm-linux.sh` (TD-073) |

## Gaps and deferred items

Tracked in [Post-Audit-Phase-2-Implementation-Plan.md](Post-Audit-Phase-2-Implementation-Plan.md): remaining stretch items only (if any).
