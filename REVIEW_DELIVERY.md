# Prepared change publication

All listed branches were source-reviewed and committed. Publication is to the fork, not upstream main. No PR has been created, merged or deployed. Upstream PRs are disabled.

Baseline: `129a67aadedc92368ca1db9b6c01bc2e5c6c6f41`. The complete 148-ID status inventory is in [REVIEW_BACKLOG.md](REVIEW_BACKLOG.md).

## Commands and actual outcomes

- `gh auth status`: MGRAFF2006 authenticated; no credential values recorded.
- `git fetch origin main`: successful; target unchanged.
- `gh pr list --repo Satisfactory-KMods/public-source --state open`: empty GraphQL list; REST listing returns 404 because PRs are disabled.
- GitHub repository GraphQL query: `hasPullRequestsEnabled: false`, `viewerPermission: READ`.
- `gh repo fork Satisfactory-KMods/public-source --clone=false --remote=false`: created fork; viewer has ADMIN on fork.
- `git merge-base`, dependency ancestor checks, and `git diff --check origin/main...HEAD`: passed for every listed branch.
- Fork pushes: successful. `git ls-remote --heads` confirms all 90 remote heads exactly match the prepared local commits; zero mismatches. No force pushes.
- `gh pr create`: rejected missing CreatePullRequest permission; REST create returned 404. No PR created.
- T3 thread PR inventory: empty; there is no PR to register.
- Unreal/FactoryGame/SML compile, automation, multiplayer, rendering and real HTTP execution: UNRUN; SDK and 4,038 LFS asset payloads unavailable. Added fixtures are not passing test results.

90 focused change branches cover 96 review IDs, including partial fixes; 52 IDs have no prepared implementation. Of the branches, 87 require draft native verification and 3 contain only checked documentation corrections. 22 native regression fixture files were added across branch diffs (dependency fixtures can appear in more than one diff); none were executed here.

`git merge-tree --write-tree --name-only` integration checks (without changing branches or worktrees):

- PL02 logging + PL10 HTTP lifetime: content conflict in `KPrivateCodeLib/Source/KPrivateCodeLib/Private/HttpLib/KPCLJsonObject.cpp`. Preserve redaction while adopting weak-object/once/game-thread completion.
- RS16 correlation + RS18 list validation: content conflicts in `RSS/Source/RSS/Private/Api/RssApiClient.cpp` and `RSS/Source/RSS/Public/Api/RssApiClient.h`. Keep transactional parsing and request-specific completion together when resolving.
- RS02 catalog save order + RS03 catalog load preservation: merge-tree succeeds.
- RS06 image action activation + RS09 bounded receive: merge-tree succeeds.

A clean textual integration check does not prove compilation or runtime compatibility. No integration result was committed or merged into a target branch.

## Review and eventual merge sequence

1. TLS and credential-log fixes, then RPC authorization and stock/item conservation. Resolve the PL02/PL10 shared HTTP changes deliberately.
2. Correctness/reliability changes by module; overlapping source files can require merge resolution even when each branch starts from main. Rerun the focused native checks after resolution.
3. RS17 lifetime and RS20 routing before RS16 correlation. Correlation includes both dependency commits via an ordinary merge; RS18 parsing remains separate and requires integration review.
4. DF22 deferred reporting before O01 provenance.
5. Remaining math, UI and documentation corrections; optional O06 swatch exchange and O01 provenance after correctness work.

No implementation branch is the integration branch for the whole batch. Native branches should be draft PRs until the stated engine checks pass.

## Complete branch inventory

### PL01: Restore HTTPS certificate verification

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/restore-http-certificate-verification) · `fix/restore-http-certificate-verification` · `701d59f74c5b`

Remove the global module and config overrides that disabled certificate verification for HTTP consumers.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### PL02: Remove credential-bearing HTTP logs

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/redact-http-credential-logs) · `fix/redact-http-credential-logs` · `e5dad9d2f5a3`

Stop logging request URLs and response payloads containing benefit-service credentials.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Partial: credential-bearing URL protocol still needs the service contract.

### PL03: Preserve source items during inventory transfers

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/inventory-transfer-conservation) · `fix/inventory-transfer-conservation` · `cb2dae09a7e6`

Validate the actual source class and preserve stack state when moving items; reject self-transfers.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Faxit count-only storage does not gain item-state persistence.

### PL04: Authorize RPC callers and interaction targets

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rco-authoritative-access) · `fix/rco-authoritative-access` · `47ba23f39ad5`

Resolve the server-owned controller and character; require own inventories and normal server interaction targets, with host/admin-only debug actions.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Policy A selected. Vehicle-controlled characters and UI master redirects require multiplayer verification.

### PL05, PL06: Preserve stored stock when capacity shrinks

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/faxit-capacity-stock) · `fix/faxit-capacity-stock` · `86dca2e9a3f5`

Clamp available capacity without subtracting existing stock when an over-capacity store rejects additions.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KPrivateCodeLib/Source/KPrivateCodeLib/Private/Tests/KPCLFaxitCapacityTests.cpp`.

### PL07, PL08: Reject invalid stock operations without empty slots

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/faxit-invalid-operations) · `fix/faxit-invalid-operations` · `a6198f7bd3d3`

Reject negative quantities and avoid creating empty stock entries for rejected operations.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KPrivateCodeLib/Source/KPrivateCodeLib/Private/Tests/KPCLFaxitOperationTests.cpp`.

### PL09: Use an exclusive lock for storage snapshots

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/faxit-snapshot-lock) · `fix/faxit-snapshot-lock` · `81e24936308f`

Use an exclusive lock where replication snapshot state is mutated.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Partial: factory-thread notifications and broader thread affinity require instrumentation.

### PL10: Guard HTTP completion against destroyed objects

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/http-request-uobject-lifetime) · `fix/http-request-uobject-lifetime` · `77e5599ecfc6`

Capture a weak UObject, complete once, and apply results on the game thread, including request startup failure.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Partial: other async paths and concurrent shared request state remain outside scope. Combine carefully with PL02 logging changes.

### PL15: Return the item name from descriptor helpers

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/item-descriptor-name) · `fix/item-descriptor-name` · `0f2eab5b1915`

Use the item-name accessor where the helper previously returned its description.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### PL17: Use independent color and extra-data bounds

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/fg-color-overrides) · `fix/fg-color-overrides` · `c60a52205b8a`

Check color channels and extra-data indices against their respective arrays.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### PL18: Allocate the indicator pulse value

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/indicator-pulse-data) · `fix/indicator-pulse-data` · `1c93d58a211f`

Allocate the additional custom-data float used by the pulse indicator.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### PL19: Remove the selected swatch by index

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/swatch-removal) · `fix/swatch-removal` · `eb108aefb316`

Delete only the selected index instead of every equal swatch.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: The original inequality claim is unsupported: baseline operator!= is already correct. This fixes the distinct indexed-deletion defect.

### PL20: Refresh delivery bindings when the network core changes

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/delivery-core-events) · `fix/delivery-core-events` · `facd018e0d40`

Detach old core settings and bind the current core during delivery network refresh.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Partial: addresses delivery bindings, not every generic network lifecycle path.

### PL21: Exclude base production from dynamic capacity

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/dynamic-generator-capacity) · `fix/dynamic-generator-capacity` · `a7ce56a43e75`

Keep dynamic generators out of the fixed base-production capacity total.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### PL22: Calculate progress using effective production duration

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/production-progress) · `fix/production-progress` · `57672eab9b1f`

Use the effective duration in the production-progress calculation.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### PL24: Accept valid attachment points near the origin

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/modular-origin-snap) · `fix/modular-origin-snap` · `2ef11cdba49f`

Use validity rather than a coordinate-based origin rejection for snap points.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### PL25: Preserve the complete replacement mesh transform

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/mesh-replacement-transform) · `fix/mesh-replacement-transform` · `464f5e339c7d`

Apply the complete configured transform when replacing a mesh.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### PL26: Validate and wrap variable power periods

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/variable-power-phase) · `fix/variable-power-phase` · `05166d88299f`

Reject invalid periods and wrap phase across multiple elapsed cycles.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KPrivateCodeLib/Source/KPrivateCodeLib/Private/Tests/KPCLPowerPhaseTests.cpp`.

### PL27: Use child-specific research completion on edges

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/research-prerequisite-edges) · `fix/research-prerequisite-edges` · `a1f4c5cd784d`

Evaluate prerequisite display against the corresponding child task.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### O06: Add portable custom swatch text exchange

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...feat/portable-swatch-text) · `feat/portable-swatch-text` · `d81ca236a9e3`

Add bounded versioned JSON import/export using existing native APIs, transactional validation and authority checks.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KPrivateCodeLib/Source/KPrivateCodeLib/Private/Tests/KPCLSwatchTextTests.cpp`.

Limitations: No new UI, RPC, cloud service or account integration. Reject conflicting names and malformed/nonfinite colors.

### FG02: Restrict debug RPCs to host or admin

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-debug-rpc-authorization) · `fix/kbfl-debug-rpc-authorization` · `2756404c9a32`

Resolve the actual controlled character and require host/admin authorization for debug item creation/deletion.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Policy A selected; verify local host, dedicated admin and unauthorized clients.

### FG26: Require normal interaction for machine RPC changes

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-machine-rpc-interaction) · `fix/klib-machine-rpc-interaction` · `334b294cde92`

Require the caller's canonical character to be interacting with the target, respecting UI master redirects, and reject nonfinite environmental inputs.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Partial: buffered-item recovery during destructive recipe changes is not implemented.

### FG03: Apply overwrite startup once

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-overwrite-start-once) · `fix/kbfl-overwrite-start-once` · `2c76b92fb4be`

Remove duplicate startup overwrite execution.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG04: Merge every valid sparse collection entry

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-sparse-collection-merge) · `fix/kbfl-sparse-collection-merge` · `4579cf261f14`

Continue past invalid sparse entries instead of stopping the entire collection traversal.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG05: Honor recipe exclusions during initial application

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-recipe-ignore-initial-pass) · `fix/kbfl-recipe-ignore-initial-pass` · `3620fe02d176`

Apply the configured ignored-recipe list on the initial pass.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG06: Use research-tree paths for removal

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-research-removal-paths) · `fix/kbfl-research-removal-paths` · `d3f52b396af6`

Load research-tree removal entries from the intended configured path collection.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG07, FG34, FG35: Initialize and release live spawn requirements

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-spawn-requirement-lifecycle) · `fix/kbfl-spawn-requirement-lifecycle` · `ff6e518fcc39`

Instantiate configured requirements with world context, initialize before use, and clear live instances without destroying configuration.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Lifecycle cleanup occurs once; engine object/PIE behavior remains unrun.

### FG08: Skip disabled spawn descriptors

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-disabled-spawn-descriptors) · `fix/kbfl-disabled-spawn-descriptors` · `46ef0ec40bff`

Honor the descriptor-enabled flag before spawning.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG09: Match PIE maps against configured worlds

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-pie-world-matching) · `fix/kbfl-pie-world-matching` · `0b86db2d0e0a`

Normalize editor world names before matching configured spawn worlds.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG11: Preserve inventory locks in capacity queries

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-capacity-preserve-lock) · `fix/kbfl-capacity-preserve-lock` · `ac999ce8832b`

Avoid changing caller-owned inventory locking while checking capacity.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG12: Check quantity capacity in empty inventory slots

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-empty-slot-capacity) · `fix/kbfl-empty-slot-capacity` · `923ca6e6c3ff`

Account for stack quantity and per-slot capacity in an empty-slot query.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG13: Reserve capacity for complete item batches

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-batch-inventory-capacity) · `fix/kbfl-batch-inventory-capacity` · `ad3685d1fd9d`

Evaluate combined quantities against the engine's whole-batch capacity contract, with overflow guards.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG15: Check the complete hatchery egg input count

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-hatchery-egg-input-count) · `fix/klib-hatchery-egg-input-count` · `af422b9c916d`

Require the configured number of eggs before production.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Full-output transaction recovery (FG14) remains separate.

### FG17: Handle missing incubator hatching data

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-incubator-null-hatching-data) · `fix/klib-incubator-null-hatching-data` · `6654a6eb2ad5`

Guard visual updates when the selected hatching data is absent.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG18: Guard deferred hatchery discovery lifetime

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-hatchery-deferred-discovery-lifetime) · `fix/klib-hatchery-deferred-discovery-lifetime` · `eabd67a1a613`

Use weak actor ownership for deferred discovery callbacks.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG19: Correct the ore information unlock predicate

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-ore-unlock-check) · `fix/klib-ore-unlock-check` · `d8343992c67f`

Check the intended unlock prerequisite when exposing ore information.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG20: Honor resource allowlist priority

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kapi-allowlist-priority) · `fix/kapi-allowlist-priority` · `89bf74f7534f`

Apply priority ordering so the intended rule wins.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG21: Keep explicit resource denials after miner discovery

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kapi-allowlist-after-miner-defaults) · `fix/kapi-allowlist-after-miner-defaults` · `63766cc06799`

Apply explicit allowlist decisions after collecting miner defaults.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG22: Skip null resource allowlist entries safely

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kapi-null-allowlist-entries) · `fix/kapi-null-allowlist-entries` · `50c1fdddf4c5`

Validate entries before dereferencing them for logging or application.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG25: Validate debug hologram build mode indices

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-hologram-mode-bounds) · `fix/kbfl-hologram-mode-bounds` · `2e52a2b543a4`

Reject invalid mode indices before accessing build-mode data.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG27: Return skip-unlock enumeration results

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-skip-unlock-enumeration) · `fix/klib-skip-unlock-enumeration` · `80ee93c38da8`

Populate the promised enumeration result instead of returning an empty array.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG28: Write the correct instanced lamp RGB channels

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-instanced-lamp-rgb) · `fix/klib-instanced-lamp-rgb` · `983a5966b57c`

Write each RGB component to its corresponding instance-data channel.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG29: Return the selected class configuration

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-config-class-value) · `fix/kbfl-config-class-value` · `1e99f4b1b76d`

Return the configured class value from the class-setting helper.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG31: Remove the location export comparison allocation

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-location-export-allocation) · `fix/kbfl-location-export-allocation` · `44c40707b63f`

Compare against a stack-allocated default value rather than leaking a heap allocation.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG32: Return all assets when no class filter is requested

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kapi-unfiltered-data-assets) · `fix/kapi-unfiltered-data-assets` · `3fedca7b4011`

Implement the existing unfiltered data-asset discovery contract.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG33: Load pending soft swatch class references

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-load-pending-swatch-class) · `fix/kbfl-load-pending-swatch-class` · `6f5b70b78b47`

Resolve pending soft class references before testing their class.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG36: Align fluid widget rates with per-minute units

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-widget-fluid-rate-units) · `fix/kbfl-widget-fluid-rate-units` · `63f83e8b0892`

Convert the displayed fluid rate to the units promised by the helper.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Blueprint consumers and native compatibility require verification.

### DF01: Address schematic fields inside research structs

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-research-field-address) · `fix/kdf-research-field-address` · `b00daa8e84f4`

Use the containing research-node struct address when accessing its reflected schematic field.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF02: Reject abstract classes before construction

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-abstract-construction) · `fix/kdf-abstract-construction` · `bef9349b8a28`

Fail validation before constructing objects of an abstract class.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF05: Round-trip scalar instanced object properties

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-instanced-roundtrip) · `fix/kdf-instanced-roundtrip` · `d22d0ed12c87`

Serialize and restore the scalar instanced-object shape supported by the property contract.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF06: Finalize property writes during undo and redo

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-undo-fixups) · `fix/kdf-undo-fixups` · `0366d1ea892c`

Run the required property fixups after undo/redo restoration.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF07: Restore pack scope for deferred and lazy patches

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-late-pack-scope) · `fix/kdf-late-pack-scope` · `afbae8fbc6cf`

Capture and restore the original pack context during late execution.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF08: Preserve distinct runtime patch rules

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-rule-identity) · `fix/kdf-rule-identity` · `8e8d2c8d09cc`

Use complete rule identity instead of collapsing separately configured rules.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF09: Apply pack includes in dependency order

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-include-order) · `fix/kdf-include-order` · `959c6e812e66`

Order includes topologically and report invalid dependency graphs.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KDataForge/Source/KDataForge/Private/Tests/KDFIncludeOrderTests.cpp`.

### DF10: Validate sink integer ranges

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-sink-range) · `fix/kdf-sink-range` · `8dc78132af96`

Reject sink values outside the supported integer range before conversion.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KDataForge/Source/KDataForge/Private/Tests/KDFSinkPointsTests.cpp`.

### DF11: Use unique identities for generated sink rows

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-sink-row-identity) · `fix/kdf-sink-row-identity` · `c278de126b46`

Avoid row-name collisions between independently generated sink entries.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF12: Detect case-equivalent Unreal identities

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-identity-case) · `fix/kdf-identity-case` · `a2693e41c7b0`

Compare registration identities consistently with Unreal's case-insensitive name semantics.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF18: Keep scalar strings separate from localized text

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-editor-strings) · `fix/kdf-editor-strings` · `9984b0bf83dd`

Edit FString/FName values using their scalar representation instead of localized-text conversion.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF19: Create editor output directories before writing

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-editor-directories) · `fix/kdf-editor-directories` · `b175afafa804`

Ensure the configured output directory exists before writing editor exports.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF20: Contain draft output paths and prevent overwrites

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-draft-paths) · `fix/kdf-draft-paths` · `7385c242b449`

Validate draft output paths and reject accidental overwrites outside the intended contract.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF22: Retain deferred diagnostics and operation records

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-deferred-report) · `fix/kdf-deferred-report` · `56aca1c25922`

Keep late-execution diagnostics and operation records available in the report.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF23: Release abandoned startup delegates

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-deferred-cleanup) · `fix/kdf-deferred-cleanup` · `438eb78bf0ed`

Detach startup callbacks when their owner is abandoned.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF25: Canonicalize property identities in diff snapshots

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-canonical-snapshots) · `fix/kdf-canonical-snapshots` · `beeb5ab0be29`

Use resolved canonical property identities so equivalent aliases compare against the same snapshot.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KDataForge/Source/KDataForge/Private/Tests/KDFSnapshotTests.cpp`.

### DF26: Retain undo history across editor reopening

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-editor-history) · `fix/kdf-editor-history` · `7f80cb021482`

Keep session undo history when the editor panel is closed and reopened.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF28: Parse bounded integer operation indices strictly

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-operation-indices) · `fix/kdf-operation-indices` · `2bad2f795cd5`

Reject malformed, fractional and out-of-range operation indices.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KDataForge/Source/KDataForge/Private/Tests/KDFIndexTests.cpp`.

### DF29: Use resolved fields for stack-size fixups

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-stack-fixup-case) · `fix/kdf-stack-fixup-case` · `90a9352511ac`

Detect the resolved stack-size property regardless of input alias casing.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### DF32: Document the native-only patch handler interface

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-handler-docs) · `fix/kdf-handler-docs` · `d87084602319`

Correct documentation to describe the C++ handler interface actually implemented.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Documentation checked against the implementation.

Limitations: Documentation only; native build is not required.

### O01: Show recorded property patch provenance

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...feat/kdf-property-provenance) · `feat/kdf-property-provenance` · `b4276f0c99e5`

Add an ordered read-only history view for recorded property operations using the existing operation records.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KDataForge/Source/KDataForge/Private/Tests/KDFOperationOrderTests.cpp`.

Dependencies: `fix/kdf-deferred-report`.

Limitations: Depends on DF22. Reports recorded operations only; external writes, undo and persistence are outside scope.

### RS06: Start image async actions on activation

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-image-action-activation) · `fix/rss-image-action-activation` · `954f9c94cf5e`

Defer factory-started requests until Activate so callers can bind completion delegates first.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `RSS/Source/RSS/Private/Tests/RssDownloadImageActivationTests.cpp`.

Limitations: Explicit native Start APIs keep their existing immediate behavior. Native direct ProcessRequest(false) handling remains outside scope.

### RS09: Bound image HTTP response buffering

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-bounded-image-http-receive) · `fix/rss-bounded-image-http-receive` · `333044475a2d`

Stream both HTTP paths into an archive capped at 16 MiB and decode retained bytes; reject unsupported streaming and receive errors.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `RSS/Source/RSS/Private/Tests/RssImageResponseBufferTests.cpp`.

Limitations: Bounds logical received body bytes per request, not total memory, decoding or concurrent requests. Widget startup false handling is inherited.

### RS17: Retain and clean up API async actions

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-api-action-lifetime) · `fix/rss-api-action-lifetime` · `9d89fc589036`

Register actions with the game instance, complete once and remove delegates when terminal.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `RSS/Source/RSS/Private/Tests/RssApiActionLifecycleTests.cpp`.

Limitations: GC and actual network callbacks need engine verification.

### RS18: Reject malformed API design lists transactionally

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-api-list-validation) · `fix/rss-api-list-validation` · `907016c381c0`

Reject invalid entries without publishing a partially parsed list as a successful response.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `RSS/Source/RSS/Private/Tests/RssApiListValidationTests.cpp`.

Limitations: Overlaps API implementation with RS16/RS20; resolve and recheck integration.

### RS20: Route API callbacks by operation and encode IDs

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-api-operation-routing) · `fix/rss-api-operation-routing` · `26c7599f75b3`

Select response handlers explicitly by operation and encode design IDs as path segments.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `RSS/Source/RSS/Private/Tests/RssApiEndpointTests.cpp`.

Limitations: Overlaps API implementation with RS16; correlation depends on this routing.

### RS16: Correlate API async actions with their requests

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-api-request-correlation) · `fix/rss-api-request-correlation` · `f5d04c649e41`

Track each action's request and ignore unrelated or duplicate completions, including startup failure.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `RSS/Source/RSS/Private/Tests/RssApiActionLifecycleTests.cpp`, `RSS/Source/RSS/Private/Tests/RssApiEndpointTests.cpp`, `RSS/Source/RSS/Private/Tests/RssApiRequestCorrelationTests.cpp`.

Dependencies: `fix/rss-api-action-lifetime`, `fix/rss-api-operation-routing`.

Limitations: Depends on RS17 and RS20. Legacy client-level Blueprint multicasts remain global; new correlation applies to async action nodes.

### RS21: Own decoded image buffers across render handoff

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-image-decode-buffer-ownership) · `fix/rss-image-decode-buffer-ownership` · `446c28df2f53`

Use scoped ownership for decoded bytes and transfer ownership into the render command.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Actual RHI/resource failure checks require the engine.

### RS33: Clean up rejected cached image actions

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-cache-action-rejection-cleanup) · `fix/rss-cache-action-rejection-cleanup` · `6bd4a0139a8b`

Use the guarded failure path when RequestImage rejects an action, releasing roots and bindings once.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### PL11: Preserve outputs on failed JSON numeric reads

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/json-numeric-read-contract) · `fix/json-numeric-read-contract` · `c007556397a3`

Validate numeric types, finiteness and destination ranges before assigning caller outputs.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KPrivateCodeLib/Source/KPrivateCodeLib/Private/Tests/KPCLJsonNumericTests.cpp`.

Limitations: Fractional integer truncation remains compatible.

### PL12, PL13: Honor JSON object and collection read contracts

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/json-object-read-contract) · `fix/json-object-read-contract` · `1d1f02ac4e2d`

Return success for valid nested objects, validate whole arrays transactionally and accept valid empty arrays.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `KPrivateCodeLib/Source/KPrivateCodeLib/Private/Tests/KPCLJsonCollectionTests.cpp`.

### PL14: Read the NewsFeed setting when it changes

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/news-feed-config-key) · `fix/news-feed-config-key` · `70df91b50ebd`

Look up NewsFeed instead of PatreonList when refreshing the feed configuration.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### FG30: Space orbit points uniformly in KBFL and RSS

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-orbit-point-spacing) · `fix/kbfl-orbit-point-spacing` · `7680b2d04bd6`

Use floating-point 360/point-count steps so generated points cover a complete orbit evenly.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Both copies are included in this coherent math fix.

### RS07: Allow cached images without live DNS resolution

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-offline-cached-images) · `fix/rss-offline-cached-images` · `a6837e1475d0`

Use structural URL checks for cached reads and retain full public-address validation for network paths.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `RSS/Source/RSS/Private/Tests/RssOfflineCacheTests.cpp`.

Limitations: Offline disk decode and inherited shutdown/rebinding behavior remain native verification limits.

### RS27: Paste to compatible interface-only signs

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-interface-sign-paste) · `fix/rss-interface-sign-paste` · `467cebefd7ec`

Invoke the existing sign-paste interface for compatible signs without a factory clipboard path.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Third-party interface and multiplayer fixtures require native checks.

### RS28: Use the selected hologram build mode

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-hologram-selected-mode) · `fix/rss-hologram-selected-mode` · `05057fb1d2d8`

Honor the supplied selected build mode and forward the engine notification.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### RS29: Apply belt snap scroll rotation

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-belt-scroll-rotation) · `fix/rss-belt-scroll-rotation` · `0b0c5cce4749`

Apply the configured scroll rotation steps to belt snapping.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### RS31: Remove the template export comparison allocation

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-template-export-allocation) · `fix/rss-template-export-allocation` · `a88278f37f05`

Compare against a stack-allocated default template instead of leaking a heap allocation.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

### RS34: Document the sign gun copy and paste controls

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-sign-gun-controls-docs) · `fix/rss-sign-gun-controls-docs` · `e35beb42f187`

Align README copy/paste controls with the implemented primary and secondary actions.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Documentation checked against the implementation.

Limitations: Documentation only.

### RS35: Document the supported 128-layer sign limit

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-layer-limit-docs) · `fix/rss-layer-limit-docs` · `529b2685cffa`

Replace the unlimited-layer claim with the native 128-layer validation limit.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Documentation checked against the implementation.

Limitations: Partial: exposing the limit in unavailable Blueprint UI remains blocked.

### RS01, RS03, RS04: Preserve templates across load and migration failures

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-template-migration) · `fix/rss-template-migration` · `637d5546681b`

Load and validate existing templates before migration, stage one save, clear legacy state only after success, and parse/export transactionally.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `RSS/Source/RSS/Private/Tests/RSSTemplateSerializationTests.cpp`.

Limitations: Partial: cross-SaveGame/file crash idempotency is unverified. Failed file replacement keeps the staged file. CDO/world mismatch is rejected; editor world lookup remains a separate requirement. Platform replacement/disk-full fault checks unrun.

### RS02: Keep image catalog saves ordered

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-catalog-save-order) · `fix/rss-catalog-save-order` · `d1781c19df50`

Perform debounced saves synchronously so older queued writes cannot overwrite later or EndPlay state, and retain dirty state after failure.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Limitations: Synchronous game-thread disk I/O has a latency tradeoff; measure with representative catalogs before release.

### RS03: Preserve image catalogs when loading fails

[Review diff](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-catalog-load-preservation) · `fix/rss-catalog-load-preservation` · `8c1e25eb8f12`

Parse catalog data transactionally, require a typed entries array and nonempty entry URLs, and block saves after an unsuccessful existing-file load.

Verification: final source diff reviewed; branch/worktree checks and `git diff --check` pass. Native checks unrun.

Added regression fixtures (unrun): `RSS/Source/RSS/Private/Tests/RSSImageCatalogTests.cpp`.

Limitations: Recovery requires restarting/reloading after restoring the file. Shared standalone URL serialization contract remains compatible; filesystem failure tests unrun.
