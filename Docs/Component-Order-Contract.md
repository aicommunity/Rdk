# Component and property order contract

## Calc order (components)

1. Sibling calculation order is the order of non-static children in `UContainer::PComponents` after the latest `UpdateComputationOrder()`.
2. `Calculate` / `Reset` / active-child `Build` loops use `ActiveComponents`, which must match that `PComponents` order.
3. `ChangeComponentPosition` / `SetComponentPosition` invalidate `ActiveComponents` and rebuild `ComponentsIdIndex`.
   Positions outside the vector clamp to its first/last element. The relative
   destination is evaluated without signed integer overflow; absolute positions
   clamp before subtracting the current index. Forward movement copies only the
   intervening elements and never reads the element after the destination.
   Invalid source indices return false without changing order. Native regression
   cases live in `UContainerComponentOrder`: forward-to-last order/IDs/cache,
   maximum positive step, and minimum absolute position. The pre-fix implementation
   has been reproduced with a failing order check and an ASan heap-buffer-overflow;
   runtime acceptance of this correction is recorded by the parent project.
4. Pulse neuron cable order (`NPulseNeuron::UpdateComputationOrder`) places distal segments first (`Dendrite1_N` … `Dendrite1_1`), then soma / LT — intentional tip→soma calc.
   This automatic order applies to neuron structure modes 1/2. Mode 0 keeps the explicit graph's sibling order, including after XML Load and native Build; moving only soma/LT would change its dynamics.
5. `NNeuronTrainer` / `NNeuronLearner` place pulse sources before the neuron in their own `UpdateComputationOrder`.

## Property enumeration order

1. Runtime lookup stays `unordered_map` (`GetPropertiesList`) for O(1) `FindProperty`.
2. Stable listing uses `GetPropertiesOrder()` — the sequence of successful `AddLookupProperty` calls (typically C++ member-init / ctor order).
3. C-API (`Model_GetComponentPropertiesList` / `LookupList`), Qt port pickers, componentsList IO tabs, and property XML export iterate `GetPropertiesOrder`.

## GUI

1. `UGuiSnapshot::ComponentOrder` preserves DFS `GetComponentsList` order for componentsList trees (do not use `QHash::keys()` for sibling order).
2. Do not enable `QTreeWidget` alphabetical sorting on these trees — it would diverge from calc order.
3. Favorites / ClDescr maps may still appear alphabetical (`std::map`); that is separate from instance property registration order.

## See also

- [Property-System.md](Diagrams/Property-System.md)
- [Component-Lifecycle.md](Diagrams/Component-Lifecycle.md)
