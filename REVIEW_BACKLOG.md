# Verified-fix backlog

Baseline: `origin/main` at `129a67aadedc92368ca1db9b6c01bc2e5c6c6f41`.
Finding IDs refer to the full review in the originating T3 Code thread. The
review's evidence and qualifications remain applicable; this file tracks work,
not a second review. No PR being opened implies it has been merged or deployed.

## Delivery constraints

- Each independent change starts from the normal target branch, `main`.
- Check current code and existing PRs before publication; do not duplicate work.
- Authentication restored for MGRAFF2006. Upstream main is unchanged. Fork pushes work.
  Upstream has `hasPullRequestsEnabled: false`; GraphQL create is denied and REST
  pull-request routes return 404. Repository admin must enable PRs. No PR exists
  for these branches yet; publication means pushed fork branches only.
- Unreal/FactoryGame/SML SDK and asset payloads are unavailable here. All 4,038
  Unreal assets are LFS pointers. Engine changes remain drafts until the stated
  focused build/runtime checks pass; source review is not an engine test.
- No new SDK/dependency installation, production writes, merge or deployment.

## Status meanings

`actionable`: supported behavior and a clear fix; local preparation can proceed.
`already resolved`: current target/identified existing work removes the defect.
`unsupported`: further evidence rejects the original finding; retain the reason.
`blocked`: a named unavailable dependency or verification prevents completion.
`requiring a decision`: behavior/policy needs an explicit choice before dependent work.

Publication state is separate from classification: queued, prepared, reviewed,
published draft, published ready, or merged (only if independently observed).

## Inventory

| ID | Priority | Item | Classification | State / next check |
|---|---|---|---|---|
| PL01 | high | Disable global TLS suppression | actionable | Pushed; upstream PR blocked: `fix/restore-http-certificate-verification` |
| PL02 | high | Redact HTTP credentials and payload logs | actionable | Pushed; upstream PR blocked: `fix/redact-http-credential-logs`; Partial: credential-bearing service URL protocol remains blocked on the backend contract |
| PL03 | high | Conserve source item class and state in transfers | actionable | Pushed; upstream PR blocked: `fix/inventory-transfer-conservation` |
| PL04 | high | Authorize RPC requester and targets | actionable | Pushed; upstream PR blocked: `fix/rco-authoritative-access` |
| PL05 | high | Preserve stock when unique capacity shrinks | actionable | Pushed; upstream PR blocked: `fix/faxit-capacity-stock` |
| PL06 | high | Reject additions without subtracting overcapacity stock | actionable | Pushed; upstream PR blocked: `fix/faxit-capacity-stock` |
| PL07 | medium | Reject negative stock removals | actionable | Pushed; upstream PR blocked: `fix/faxit-invalid-operations` |
| PL08 | low | Avoid zero-stock slot creation on rejected operations | actionable | Pushed; upstream PR blocked: `fix/faxit-invalid-operations` |
| PL09 | high | Protect storage snapshots and game-thread notifications | actionable | Pushed; upstream PR blocked: `fix/faxit-snapshot-lock`; Partial: exclusive snapshot lock fixed; thread-affinity/notification analysis still blocked |
| PL10 | high | Protect UObject lifetimes in asynchronous work | actionable | Pushed; upstream PR blocked: `fix/http-request-uobject-lifetime`; Partial: HTTP callback path fixed; other asynchronous lifetimes and concurrent request state remain |
| PL11 | medium | Initialize outputs only after successful JSON numeric reads | actionable | Pushed; upstream PR blocked: `fix/json-numeric-read-contract` |
| PL12 | medium | Report nested JSON object success correctly | actionable | Pushed; upstream PR blocked: `fix/json-object-read-contract` |
| PL13 | low | Honor JSON collection types and empty results | actionable | Pushed; upstream PR blocked: `fix/json-object-read-contract` |
| PL14 | low | Read the correct NewsFeed setting | actionable | Pushed; upstream PR blocked: `fix/news-feed-config-key` |
| PL15 | medium | Use item name rather than description | actionable | Pushed; upstream PR blocked: `fix/item-descriptor-name` |
| PL16 | medium | Complete equipment targeting trace | blocked | Equipment trace/input contract and Blueprint fixtures unavailable |
| PL17 | low | Separate standard color and extra-data bounds | actionable | Pushed; upstream PR blocked: `fix/fg-color-overrides` |
| PL18 | low | Allocate the pulse indicator float | actionable | Pushed; upstream PR blocked: `fix/indicator-pulse-data` |
| PL19 | medium | Correct swatch equality and indexed deletion | actionable | Pushed; upstream PR blocked: `fix/swatch-removal`; Original inequality claim unsupported (baseline correct); distinct duplicate indexed-deletion bug fixed |
| PL20 | medium | Dispatch network lifecycle transitions | actionable | Pushed; upstream PR blocked: `fix/delivery-core-events`; Partial: delivery core event bindings fixed; broader generic lifecycle verification remains |
| PL21 | medium | Exclude base production from dynamic generators | actionable | Pushed; upstream PR blocked: `fix/dynamic-generator-capacity` |
| PL22 | low | Use effective duration for progress | actionable | Pushed; upstream PR blocked: `fix/production-progress` |
| PL23 | medium | Validate attachment limits at commit | blocked | Construction rejection/refund contract and attachment fixtures unavailable |
| PL24 | low | Accept valid snap points near origin | actionable | Pushed; upstream PR blocked: `fix/modular-origin-snap` |
| PL25 | low | Preserve full mesh replacement transform | actionable | Pushed; upstream PR blocked: `fix/mesh-replacement-transform` |
| PL26 | medium | Validate and wrap variable-power periods | actionable | Pushed; upstream PR blocked: `fix/variable-power-phase` |
| PL27 | low | Show child-specific research repeat completion | actionable | Pushed; upstream PR blocked: `fix/research-prerequisite-edges` |
| PL28 | high | Reacquire delivery state after reentrant callbacks | blocked | Reentrant delivery/reward fixture needed to establish callback-safe ownership |
| FG01 | high | Validate reflected property ownership | blocked | Sibling Blueprint reflected-property compatibility fixtures unavailable |
| FG02 | high | Restrict debug inventory RPCs | actionable | Pushed; upstream PR blocked: `fix/kbfl-debug-rpc-authorization` |
| FG03 | medium | Apply overwrite startup once | actionable | Pushed; upstream PR blocked: `fix/kbfl-overwrite-start-once` |
| FG04 | medium | Iterate all valid sparse collection entries | actionable | Pushed; upstream PR blocked: `fix/kbfl-sparse-collection-merge` |
| FG05 | medium | Honor initial recipe exclusions | actionable | Pushed; upstream PR blocked: `fix/kbfl-recipe-ignore-initial-pass` |
| FG06 | medium | Use research-tree removal paths | actionable | Pushed; upstream PR blocked: `fix/kbfl-research-removal-paths` |
| FG07 | medium | Initialize configured spawn requirements | actionable | Pushed; upstream PR blocked: `fix/kbfl-spawn-requirement-lifecycle` |
| FG08 | medium | Honor disabled spawn descriptors | actionable | Pushed; upstream PR blocked: `fix/kbfl-disabled-spawn-descriptors` |
| FG09 | medium | Match editor maps correctly | actionable | Pushed; upstream PR blocked: `fix/kbfl-pie-world-matching` |
| FG10 | medium | Reach intended resource-well collision channels | blocked | Actual physics collision presets and query fixtures unavailable |
| FG11 | medium | Preserve inventory locking during capacity queries | actionable | Pushed; upstream PR blocked: `fix/kbfl-capacity-preserve-lock` |
| FG12 | medium | Check empty-slot quantity capacity | actionable | Pushed; upstream PR blocked: `fix/kbfl-empty-slot-capacity` |
| FG13 | medium | Reserve complete batch capacity | actionable | Pushed; upstream PR blocked: `fix/kbfl-batch-inventory-capacity` |
| FG14 | medium | Preserve eggs and rolled hatchery output when full | blocked | Production-roll and full-output inventory transaction fixtures unavailable |
| FG15 | medium | Check required egg consumption count | actionable | Pushed; upstream PR blocked: `fix/klib-hatchery-egg-input-count` |
| FG16 | medium | Enforce configured incubator and tank tiers | blocked | Tier progression is authored in unavailable assets |
| FG17 | medium | Handle missing hatching data in visuals | actionable | Pushed; upstream PR blocked: `fix/klib-incubator-null-hatching-data` |
| FG18 | medium | Protect deferred hatchery actor lifetime | actionable | Pushed; upstream PR blocked: `fix/klib-hatchery-deferred-discovery-lifetime` |
| FG19 | medium | Correct ore information unlock predicate | actionable | Pushed; upstream PR blocked: `fix/klib-ore-unlock-check` |
| FG20 | medium | Honor allowlist priority | actionable | Pushed; upstream PR blocked: `fix/kapi-allowlist-priority` |
| FG21 | medium | Preserve explicit resource denials after miner scanning | actionable | Pushed; upstream PR blocked: `fix/kapi-allowlist-after-miner-defaults` |
| FG22 | medium | Reject null allowlist entries before logging | actionable | Pushed; upstream PR blocked: `fix/kapi-null-allowlist-entries` |
| FG23 | medium | Fail safely when swatch slots are exhausted | requiring a decision | Choose swatch-overflow/save-slot behavior; do not guess a representation change |
| FG24 | medium | Avoid stale delayed swatch color replay | blocked | Delayed color replication workflow and multiplayer fixtures unavailable |
| FG25 | medium | Validate debug hologram build-mode indices | actionable | Pushed; upstream PR blocked: `fix/kbfl-hologram-mode-bounds` |
| FG26 | medium | Authorize machine RPCs and destructive recipe changes | actionable | Pushed; upstream PR blocked: `fix/klib-machine-rpc-interaction`; Partial: interaction authorization fixed; buffered-item recovery semantics still blocked |
| FG27 | low | Return skip-unlock enumeration results | actionable | Pushed; upstream PR blocked: `fix/klib-skip-unlock-enumeration` |
| FG28 | low | Write correct instanced RGB channels | actionable | Pushed; upstream PR blocked: `fix/klib-instanced-lamp-rgb` |
| FG29 | low | Return selected class configuration | actionable | Pushed; upstream PR blocked: `fix/kbfl-config-class-value` |
| FG30 | low | Calculate floating-point orbit steps in KBFL and RSS | actionable | Pushed; upstream PR blocked: `fix/kbfl-orbit-point-spacing` |
| FG31 | low | Remove location-export comparison allocation leak | actionable | Pushed; upstream PR blocked: `fix/kbfl-location-export-allocation` |
| FG32 | low | Implement unfiltered asset discovery contract | actionable | Pushed; upstream PR blocked: `fix/kapi-unfiltered-data-assets` |
| FG33 | low | Load pending soft swatch class references | actionable | Pushed; upstream PR blocked: `fix/kbfl-load-pending-swatch-class` |
| FG34 | low | Provide requirement world context | actionable | Pushed; upstream PR blocked: `fix/kbfl-spawn-requirement-lifecycle` |
| FG35 | low | Clear live requirement instances, retain configuration | actionable | Pushed; upstream PR blocked: `fix/kbfl-spawn-requirement-lifecycle` |
| FG36 | low | Align widget volume-rate units | actionable | Pushed; upstream PR blocked: `fix/kbfl-widget-fluid-rate-units` |
| DF01 | high | Address schematic fields within research node structs | actionable | Pushed; upstream PR blocked: `fix/kdf-research-field-address` |
| DF02 | high | Reject abstract classes before construction | actionable | Pushed; upstream PR blocked: `fix/kdf-abstract-construction` |
| DF03 | medium | Preserve map and set uniqueness | blocked | Reflected map/set uniqueness and ownership fixtures unavailable |
| DF04 | medium | Preserve integer arithmetic and range checks | requiring a decision | Integer arithmetic rounding/overflow compatibility policy needs agreement |
| DF05 | medium | Round-trip scalar instanced objects | actionable | Pushed; upstream PR blocked: `fix/kdf-instanced-roundtrip` |
| DF06 | medium | Finalize undo and redo property writes | actionable | Pushed; upstream PR blocked: `fix/kdf-undo-fixups` |
| DF07 | medium | Restore pack scope for deferred and lazy patches | actionable | Pushed; upstream PR blocked: `fix/kdf-late-pack-scope` |
| DF08 | medium | Preserve distinct runtime patch rules | actionable | Pushed; upstream PR blocked: `fix/kdf-rule-identity` |
| DF09 | medium | Order includes topologically | actionable | Pushed; upstream PR blocked: `fix/kdf-include-order` |
| DF10 | medium | Check sink integer ranges | actionable | Pushed; upstream PR blocked: `fix/kdf-sink-range` |
| DF11 | medium | Use unique sink row identities | actionable | Pushed; upstream PR blocked: `fix/kdf-sink-row-identity` |
| DF12 | medium | Detect case-equivalent Unreal identities | actionable | Pushed; upstream PR blocked: `fix/kdf-identity-case` |
| DF13 | medium | Register texture identities consistently | blocked | Texture registration identity fixtures unavailable |
| DF14 | medium | Validate historical generated-class parents | blocked | Historical generated-class parent/save compatibility fixtures unavailable |
| DF15 | medium | Protect generated compatibility manifest recovery | blocked | Compatibility manifest recovery requires historical startup/save fixtures |
| DF16 | medium | Validate existing-class registration kinds | blocked | Existing-class registration kind fixtures unavailable |
| DF17 | medium | Bound automatic research road generation | requiring a decision | Agree maximum generated road length and coordinate bounds |
| DF18 | medium | Separate scalar strings and names from localized text | actionable | Pushed; upstream PR blocked: `fix/kdf-editor-strings` |
| DF19 | medium | Create editor output directories | actionable | Pushed; upstream PR blocked: `fix/kdf-editor-directories` |
| DF20 | medium | Contain draft output paths and prevent accidental overwrites | actionable | Pushed; upstream PR blocked: `fix/kdf-draft-paths` |
| DF21 | medium | Restrict client live editing | blocked | Client live-editor authority and UI fixtures unavailable |
| DF22 | low | Retain deferred diagnostics and operation records | actionable | Pushed; upstream PR blocked: `fix/kdf-deferred-report` |
| DF23 | low | Release abandoned startup delegates | actionable | Pushed; upstream PR blocked: `fix/kdf-deferred-cleanup` |
| DF24 | low | Export only effective differences | blocked | Composite baseline and effective-difference export semantics need fixtures |
| DF25 | low | Canonicalize property identities for diff export | actionable | Pushed; upstream PR blocked: `fix/kdf-canonical-snapshots` |
| DF26 | low | Retain undo history across editor reopening | actionable | Pushed; upstream PR blocked: `fix/kdf-editor-history` |
| DF27 | low | Prevent same-named export file collisions | requiring a decision | Choose exported object identity and filename collision behavior |
| DF28 | low | Strictly parse bounded integer indices | actionable | Pushed; upstream PR blocked: `fix/kdf-operation-indices` |
| DF29 | low | Use resolved fields for stack-size fixups | actionable | Pushed; upstream PR blocked: `fix/kdf-stack-fixup-case` |
| DF30 | low | Explain known condition skips in reports | actionable | Already known diagnostic omission; deferred in this publication batch, no implementation prepared |
| DF31 | low | Preserve vendored upstream license notices | blocked | Exact vendored dependency provenance and packaged upstream notice set not established |
| DF32 | low | Document the C++-only handler interface | actionable | Pushed; upstream PR blocked: `fix/kdf-handler-docs` |
| DF33 | low | Resolve checked-in README action links | blocked | Generated/published README destination ownership and intended action URLs unavailable |
| DF34 | medium | Known per-process pack synchronization limitation | requiring a decision | Known per-process pack scope; synchronization protocol needs explicit investment scope |
| RS01 | high | Migrate templates without pre-load overwrite or failed-save loss | actionable | Pushed; upstream PR blocked: `fix/rss-template-migration`; Partial: load-first/save-success migration fixed; cross-file interruption idempotency and file recovery remain unverified |
| RS02 | medium | Serialize and protect asynchronous catalog saves | actionable | Pushed; upstream PR blocked: `fix/rss-catalog-save-order` |
| RS03 | medium | Preserve catalogs after unsuccessful parsing | actionable | Pushed; upstream PR blocked: `fix/rss-template-migration`, `fix/rss-catalog-load-preservation` |
| RS04 | medium | Reject silently dropped template entries | actionable | Pushed; upstream PR blocked: `fix/rss-template-migration` |
| RS05 | medium | Establish shared template library behavior | requiring a decision | Shared template library and active-library Blueprint contract need confirmation |
| RS06 | medium | Activate image requests after delegate binding | actionable | Pushed; upstream PR blocked: `fix/rss-image-action-activation` |
| RS07 | medium | Allow cached images without live DNS | actionable | Pushed; upstream PR blocked: `fix/rss-offline-cached-images` |
| RS08 | medium | Enforce actual remote destination policy | blocked | Actual backend connection/redirect destination controls unavailable; structural checks do not resolve rebinding |
| RS09 | medium | Bound image HTTP receive buffers | actionable | Pushed; upstream PR blocked: `fix/rss-bounded-image-http-receive` |
| RS10 | medium | Apply cache configuration before initialization | blocked | Safe live reconfiguration depends on unresolved cache shutdown/in-flight lifecycle (RS15) |
| RS11 | low | Honor or remove unused cache flags | requiring a decision | Implement unused cache flags or retire their promised behavior |
| RS12 | medium | Deduplicate disk loads and correct replacement accounting | actionable | Not prepared in this batch; replacement accounting and disk-load deduplication require separate cache fixtures |
| RS13 | medium | Recover poisoned image cache payloads | actionable | Not prepared in this batch; poisoned-cache fallback needs disk/decode failure fixtures |
| RS14 | medium | Balance sign image cache references | blocked | Per-sign duplicate URL and load/release ownership fixtures unavailable |
| RS15 | medium | Cancel or invalidate cache work at shutdown | blocked | Shutdown cancellation/generation contract needs HTTP and disk callback ordering fixtures |
| RS16 | medium | Correlate API requests and complete once | actionable | Pushed; upstream PR blocked: `fix/rss-api-request-correlation`; Async action correlation fixed; legacy global Blueprint multicast contracts remain unchanged |
| RS17 | medium | Retain API actions through asynchronous completion | actionable | Pushed; upstream PR blocked: `fix/rss-api-action-lifetime` |
| RS18 | medium | Reject malformed successful-status API responses | actionable | Pushed; upstream PR blocked: `fix/rss-api-list-validation` |
| RS19 | low | Return actual design limit data | requiring a decision | Design-limit response schema requires the backend contract |
| RS20 | low | Route API handlers by operation and encode IDs | actionable | Pushed; upstream PR blocked: `fix/rss-api-operation-routing` |
| RS21 | low | Own raw decoded buffers on every failure path | actionable | Pushed; upstream PR blocked: `fix/rss-image-decode-buffer-ownership` |
| RS22 | low | Correct half-size image resampling or retire the option | requiring a decision | Support correct half-size resampling or retire the option |
| RS23 | medium | Establish rendering behavior when pooling is disabled | requiring a decision | Support a nonpool render fallback or remove the toggle |
| RS24 | medium | Pass elapsed time through rendering gates | actionable | Not prepared in this batch; elapsed-time render gate needs frame fixtures |
| RS25 | low | Cull using sign bounds and camera projection | blocked | Actual sign bounds/camera projection/render fixtures unavailable |
| RS26 | medium | Send vanilla paste to server authority | blocked | Canonical server target and copied-data protocol must be established; changing RPC direction alone is insufficient |
| RS27 | low | Support interface-only sign paste eligibility | actionable | Pushed; upstream PR blocked: `fix/rss-interface-sign-paste` |
| RS28 | low | Use selected hologram build mode | actionable | Pushed; upstream PR blocked: `fix/rss-hologram-selected-mode` |
| RS29 | low | Apply belt scroll rotation | actionable | Pushed; upstream PR blocked: `fix/rss-belt-scroll-rotation` |
| RS30 | low | Honor exposed arrow-material behavior | requiring a decision | Arrow material flag behavior depends on the Blueprint material contract |
| RS31 | low | Remove template-export comparison allocation leak | actionable | Pushed; upstream PR blocked: `fix/rss-template-export-allocation` |
| RS32 | low | Reconcile unindexed disk cache files | blocked | Orphaned disk-cache reconciliation and interruption recovery fixtures unavailable |
| RS33 | medium | Clean up rejected cached image actions | actionable | Pushed; upstream PR blocked: `fix/rss-cache-action-rejection-cleanup` |
| RS34 | low | Correct Sign Gun controls documentation | actionable | Pushed; upstream PR blocked: `fix/rss-sign-gun-controls-docs` |
| RS35 | low | Document and expose supported layer limits | actionable | Pushed; upstream PR blocked: `fix/rss-layer-limit-docs`; Partial: documentation fixed; Blueprint UI limit exposure unavailable |
| SB01 | medium | Define blueprint destination collision policy | requiring a decision | Choose behavior for conflicting blueprint names/install destinations |
| SB02 | medium | Recover interrupted two-file blueprint installation | blocked | Two-file install interruption/recovery contract needs actual engine file fixtures |
| SB03 | low | Make Blueprint migration failure transactional | blocked | Blueprint graph transactions and failure recovery fixtures unavailable |
| DEV01 | low | Explain public-source setup and focused verification | actionable | No setup documentation change prepared in this publication batch; exact supported bootstrap requires verification |
| DEV02 | low | Correct separate blueprint backup guidance | blocked | Incorrect backup guidance appears external; owned source/location not available in this checkout |
| DEV03 | low | Resolve stale KAPI localization tasks | blocked | Localization task source is generated dashboard metadata, unavailable here |
| O01 | opportunity | Inspect effective property and mapping provenance | actionable | Pushed; upstream PR blocked: `feat/kdf-property-provenance` |
| O02 | opportunity | Parameterized sign templates | requiring a decision | Parameterized template instance/application semantics need product agreement |
| O03 | opportunity | Export session recipes to a named planner | requiring a decision | Deferred by explicit user decision; named planner consumer and import contract required |
| O04 | opportunity | Compare slug habitat requirements | blocked | Existing habitat UI/data assets unavailable; avoid duplicating authored functionality |
| O05 | opportunity | Plan research resource costs | blocked | Existing research/task UI assets unavailable; workflow and scope need confirmation |
| O06 | opportunity | Exchange portable custom swatches | actionable | Pushed; upstream PR blocked: `feat/portable-swatch-text` |
| O07 | opportunity | Preview network capacity for expansion | blocked | Existing network capacity UI assets unavailable; intended new workflow not established |
| O08 | opportunity | Convert recorded placements into spawn authoring data | blocked | Existing placement/converter assets unavailable; authoring contract not established |
| O09 | opportunity | Export runtime capabilities to a named tool | requiring a decision | Deferred by explicit user decision; named tool consumer and schema contract required |

## PR-sized change inventory

90 focused branches are pushed and remote-head verified, covering 96 IDs with
the partial scopes noted above. PR creation remains blocked because upstream
pull requests are disabled. 52 IDs have no prepared implementation. Full
descriptions, test fixtures, limitations and integration checks are recorded in
[REVIEW_DELIVERY.md](REVIEW_DELIVERY.md).

| IDs | Focused branch / review diff | Dependencies | Verification |
|---|---|---|---|
| PL01 | [fix/restore-http-certificate-verification](https://github.com/MGRAFF2006/public-source/compare/main...fix/restore-http-certificate-verification) | main | Diff hygiene and source review pass; native checks unrun |
| PL02 | [fix/redact-http-credential-logs](https://github.com/MGRAFF2006/public-source/compare/main...fix/redact-http-credential-logs) | main | Diff hygiene and source review pass; native checks unrun |
| PL03 | [fix/inventory-transfer-conservation](https://github.com/MGRAFF2006/public-source/compare/main...fix/inventory-transfer-conservation) | main | Diff hygiene and source review pass; native checks unrun |
| PL04 | [fix/rco-authoritative-access](https://github.com/MGRAFF2006/public-source/compare/main...fix/rco-authoritative-access) | main | Diff hygiene and source review pass; native checks unrun |
| PL05, PL06 | [fix/faxit-capacity-stock](https://github.com/MGRAFF2006/public-source/compare/main...fix/faxit-capacity-stock) | main | Diff hygiene and source review pass; native checks unrun |
| PL07, PL08 | [fix/faxit-invalid-operations](https://github.com/MGRAFF2006/public-source/compare/main...fix/faxit-invalid-operations) | main | Diff hygiene and source review pass; native checks unrun |
| PL09 | [fix/faxit-snapshot-lock](https://github.com/MGRAFF2006/public-source/compare/main...fix/faxit-snapshot-lock) | main | Diff hygiene and source review pass; native checks unrun |
| PL10 | [fix/http-request-uobject-lifetime](https://github.com/MGRAFF2006/public-source/compare/main...fix/http-request-uobject-lifetime) | main | Diff hygiene and source review pass; native checks unrun |
| PL15 | [fix/item-descriptor-name](https://github.com/MGRAFF2006/public-source/compare/main...fix/item-descriptor-name) | main | Diff hygiene and source review pass; native checks unrun |
| PL17 | [fix/fg-color-overrides](https://github.com/MGRAFF2006/public-source/compare/main...fix/fg-color-overrides) | main | Diff hygiene and source review pass; native checks unrun |
| PL18 | [fix/indicator-pulse-data](https://github.com/MGRAFF2006/public-source/compare/main...fix/indicator-pulse-data) | main | Diff hygiene and source review pass; native checks unrun |
| PL19 | [fix/swatch-removal](https://github.com/MGRAFF2006/public-source/compare/main...fix/swatch-removal) | main | Diff hygiene and source review pass; native checks unrun |
| PL20 | [fix/delivery-core-events](https://github.com/MGRAFF2006/public-source/compare/main...fix/delivery-core-events) | main | Diff hygiene and source review pass; native checks unrun |
| PL21 | [fix/dynamic-generator-capacity](https://github.com/MGRAFF2006/public-source/compare/main...fix/dynamic-generator-capacity) | main | Diff hygiene and source review pass; native checks unrun |
| PL22 | [fix/production-progress](https://github.com/MGRAFF2006/public-source/compare/main...fix/production-progress) | main | Diff hygiene and source review pass; native checks unrun |
| PL24 | [fix/modular-origin-snap](https://github.com/MGRAFF2006/public-source/compare/main...fix/modular-origin-snap) | main | Diff hygiene and source review pass; native checks unrun |
| PL25 | [fix/mesh-replacement-transform](https://github.com/MGRAFF2006/public-source/compare/main...fix/mesh-replacement-transform) | main | Diff hygiene and source review pass; native checks unrun |
| PL26 | [fix/variable-power-phase](https://github.com/MGRAFF2006/public-source/compare/main...fix/variable-power-phase) | main | Diff hygiene and source review pass; native checks unrun |
| PL27 | [fix/research-prerequisite-edges](https://github.com/MGRAFF2006/public-source/compare/main...fix/research-prerequisite-edges) | main | Diff hygiene and source review pass; native checks unrun |
| O06 | [feat/portable-swatch-text](https://github.com/MGRAFF2006/public-source/compare/main...feat/portable-swatch-text) | main | Diff hygiene and source review pass; native checks unrun |
| FG02 | [fix/kbfl-debug-rpc-authorization](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-debug-rpc-authorization) | main | Diff hygiene and source review pass; native checks unrun |
| FG26 | [fix/klib-machine-rpc-interaction](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-machine-rpc-interaction) | main | Diff hygiene and source review pass; native checks unrun |
| FG03 | [fix/kbfl-overwrite-start-once](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-overwrite-start-once) | main | Diff hygiene and source review pass; native checks unrun |
| FG04 | [fix/kbfl-sparse-collection-merge](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-sparse-collection-merge) | main | Diff hygiene and source review pass; native checks unrun |
| FG05 | [fix/kbfl-recipe-ignore-initial-pass](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-recipe-ignore-initial-pass) | main | Diff hygiene and source review pass; native checks unrun |
| FG06 | [fix/kbfl-research-removal-paths](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-research-removal-paths) | main | Diff hygiene and source review pass; native checks unrun |
| FG07, FG34, FG35 | [fix/kbfl-spawn-requirement-lifecycle](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-spawn-requirement-lifecycle) | main | Diff hygiene and source review pass; native checks unrun |
| FG08 | [fix/kbfl-disabled-spawn-descriptors](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-disabled-spawn-descriptors) | main | Diff hygiene and source review pass; native checks unrun |
| FG09 | [fix/kbfl-pie-world-matching](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-pie-world-matching) | main | Diff hygiene and source review pass; native checks unrun |
| FG11 | [fix/kbfl-capacity-preserve-lock](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-capacity-preserve-lock) | main | Diff hygiene and source review pass; native checks unrun |
| FG12 | [fix/kbfl-empty-slot-capacity](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-empty-slot-capacity) | main | Diff hygiene and source review pass; native checks unrun |
| FG13 | [fix/kbfl-batch-inventory-capacity](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-batch-inventory-capacity) | main | Diff hygiene and source review pass; native checks unrun |
| FG15 | [fix/klib-hatchery-egg-input-count](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-hatchery-egg-input-count) | main | Diff hygiene and source review pass; native checks unrun |
| FG17 | [fix/klib-incubator-null-hatching-data](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-incubator-null-hatching-data) | main | Diff hygiene and source review pass; native checks unrun |
| FG18 | [fix/klib-hatchery-deferred-discovery-lifetime](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-hatchery-deferred-discovery-lifetime) | main | Diff hygiene and source review pass; native checks unrun |
| FG19 | [fix/klib-ore-unlock-check](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-ore-unlock-check) | main | Diff hygiene and source review pass; native checks unrun |
| FG20 | [fix/kapi-allowlist-priority](https://github.com/MGRAFF2006/public-source/compare/main...fix/kapi-allowlist-priority) | main | Diff hygiene and source review pass; native checks unrun |
| FG21 | [fix/kapi-allowlist-after-miner-defaults](https://github.com/MGRAFF2006/public-source/compare/main...fix/kapi-allowlist-after-miner-defaults) | main | Diff hygiene and source review pass; native checks unrun |
| FG22 | [fix/kapi-null-allowlist-entries](https://github.com/MGRAFF2006/public-source/compare/main...fix/kapi-null-allowlist-entries) | main | Diff hygiene and source review pass; native checks unrun |
| FG25 | [fix/kbfl-hologram-mode-bounds](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-hologram-mode-bounds) | main | Diff hygiene and source review pass; native checks unrun |
| FG27 | [fix/klib-skip-unlock-enumeration](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-skip-unlock-enumeration) | main | Diff hygiene and source review pass; native checks unrun |
| FG28 | [fix/klib-instanced-lamp-rgb](https://github.com/MGRAFF2006/public-source/compare/main...fix/klib-instanced-lamp-rgb) | main | Diff hygiene and source review pass; native checks unrun |
| FG29 | [fix/kbfl-config-class-value](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-config-class-value) | main | Diff hygiene and source review pass; native checks unrun |
| FG31 | [fix/kbfl-location-export-allocation](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-location-export-allocation) | main | Diff hygiene and source review pass; native checks unrun |
| FG32 | [fix/kapi-unfiltered-data-assets](https://github.com/MGRAFF2006/public-source/compare/main...fix/kapi-unfiltered-data-assets) | main | Diff hygiene and source review pass; native checks unrun |
| FG33 | [fix/kbfl-load-pending-swatch-class](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-load-pending-swatch-class) | main | Diff hygiene and source review pass; native checks unrun |
| FG36 | [fix/kbfl-widget-fluid-rate-units](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-widget-fluid-rate-units) | main | Diff hygiene and source review pass; native checks unrun |
| DF01 | [fix/kdf-research-field-address](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-research-field-address) | main | Diff hygiene and source review pass; native checks unrun |
| DF02 | [fix/kdf-abstract-construction](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-abstract-construction) | main | Diff hygiene and source review pass; native checks unrun |
| DF05 | [fix/kdf-instanced-roundtrip](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-instanced-roundtrip) | main | Diff hygiene and source review pass; native checks unrun |
| DF06 | [fix/kdf-undo-fixups](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-undo-fixups) | main | Diff hygiene and source review pass; native checks unrun |
| DF07 | [fix/kdf-late-pack-scope](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-late-pack-scope) | main | Diff hygiene and source review pass; native checks unrun |
| DF08 | [fix/kdf-rule-identity](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-rule-identity) | main | Diff hygiene and source review pass; native checks unrun |
| DF09 | [fix/kdf-include-order](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-include-order) | main | Diff hygiene and source review pass; native checks unrun |
| DF10 | [fix/kdf-sink-range](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-sink-range) | main | Diff hygiene and source review pass; native checks unrun |
| DF11 | [fix/kdf-sink-row-identity](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-sink-row-identity) | main | Diff hygiene and source review pass; native checks unrun |
| DF12 | [fix/kdf-identity-case](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-identity-case) | main | Diff hygiene and source review pass; native checks unrun |
| DF18 | [fix/kdf-editor-strings](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-editor-strings) | main | Diff hygiene and source review pass; native checks unrun |
| DF19 | [fix/kdf-editor-directories](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-editor-directories) | main | Diff hygiene and source review pass; native checks unrun |
| DF20 | [fix/kdf-draft-paths](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-draft-paths) | main | Diff hygiene and source review pass; native checks unrun |
| DF22 | [fix/kdf-deferred-report](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-deferred-report) | main | Diff hygiene and source review pass; native checks unrun |
| DF23 | [fix/kdf-deferred-cleanup](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-deferred-cleanup) | main | Diff hygiene and source review pass; native checks unrun |
| DF25 | [fix/kdf-canonical-snapshots](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-canonical-snapshots) | main | Diff hygiene and source review pass; native checks unrun |
| DF26 | [fix/kdf-editor-history](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-editor-history) | main | Diff hygiene and source review pass; native checks unrun |
| DF28 | [fix/kdf-operation-indices](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-operation-indices) | main | Diff hygiene and source review pass; native checks unrun |
| DF29 | [fix/kdf-stack-fixup-case](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-stack-fixup-case) | main | Diff hygiene and source review pass; native checks unrun |
| DF32 | [fix/kdf-handler-docs](https://github.com/MGRAFF2006/public-source/compare/main...fix/kdf-handler-docs) | main | Diff hygiene and source review pass; documentation checked |
| O01 | [feat/kdf-property-provenance](https://github.com/MGRAFF2006/public-source/compare/main...feat/kdf-property-provenance) | `fix/kdf-deferred-report` | Diff hygiene and source review pass; native checks unrun |
| RS06 | [fix/rss-image-action-activation](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-image-action-activation) | main | Diff hygiene and source review pass; native checks unrun |
| RS09 | [fix/rss-bounded-image-http-receive](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-bounded-image-http-receive) | main | Diff hygiene and source review pass; native checks unrun |
| RS17 | [fix/rss-api-action-lifetime](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-api-action-lifetime) | main | Diff hygiene and source review pass; native checks unrun |
| RS18 | [fix/rss-api-list-validation](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-api-list-validation) | main | Diff hygiene and source review pass; native checks unrun |
| RS20 | [fix/rss-api-operation-routing](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-api-operation-routing) | main | Diff hygiene and source review pass; native checks unrun |
| RS16 | [fix/rss-api-request-correlation](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-api-request-correlation) | `fix/rss-api-action-lifetime`, `fix/rss-api-operation-routing` | Diff hygiene and source review pass; native checks unrun |
| RS21 | [fix/rss-image-decode-buffer-ownership](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-image-decode-buffer-ownership) | main | Diff hygiene and source review pass; native checks unrun |
| RS33 | [fix/rss-cache-action-rejection-cleanup](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-cache-action-rejection-cleanup) | main | Diff hygiene and source review pass; native checks unrun |
| PL11 | [fix/json-numeric-read-contract](https://github.com/MGRAFF2006/public-source/compare/main...fix/json-numeric-read-contract) | main | Diff hygiene and source review pass; native checks unrun |
| PL12, PL13 | [fix/json-object-read-contract](https://github.com/MGRAFF2006/public-source/compare/main...fix/json-object-read-contract) | main | Diff hygiene and source review pass; native checks unrun |
| PL14 | [fix/news-feed-config-key](https://github.com/MGRAFF2006/public-source/compare/main...fix/news-feed-config-key) | main | Diff hygiene and source review pass; native checks unrun |
| FG30 | [fix/kbfl-orbit-point-spacing](https://github.com/MGRAFF2006/public-source/compare/main...fix/kbfl-orbit-point-spacing) | main | Diff hygiene and source review pass; native checks unrun |
| RS07 | [fix/rss-offline-cached-images](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-offline-cached-images) | main | Diff hygiene and source review pass; native checks unrun |
| RS27 | [fix/rss-interface-sign-paste](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-interface-sign-paste) | main | Diff hygiene and source review pass; native checks unrun |
| RS28 | [fix/rss-hologram-selected-mode](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-hologram-selected-mode) | main | Diff hygiene and source review pass; native checks unrun |
| RS29 | [fix/rss-belt-scroll-rotation](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-belt-scroll-rotation) | main | Diff hygiene and source review pass; native checks unrun |
| RS31 | [fix/rss-template-export-allocation](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-template-export-allocation) | main | Diff hygiene and source review pass; native checks unrun |
| RS34 | [fix/rss-sign-gun-controls-docs](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-sign-gun-controls-docs) | main | Diff hygiene and source review pass; documentation checked |
| RS35 | [fix/rss-layer-limit-docs](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-layer-limit-docs) | main | Diff hygiene and source review pass; documentation checked |
| RS01, RS03, RS04 | [fix/rss-template-migration](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-template-migration) | main | Diff hygiene and source review pass; native checks unrun |
| RS02 | [fix/rss-catalog-save-order](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-catalog-save-order) | main | Diff hygiene and source review pass; native checks unrun |
| RS03 | [fix/rss-catalog-load-preservation](https://github.com/MGRAFF2006/public-source/compare/main...fix/rss-catalog-load-preservation) | main | Diff hygiene and source review pass; native checks unrun |


## Decisions and external checks

- GitHub: account login works. An upstream repository administrator must enable
  pull requests. User selected restoring upstream access; no substitute fork PRs
  were opened. Recheck the repository setting before resuming PR creation.
- PL04/FG02/FG26: policy A selected explicitly: own character for inventory,
  normal nearby interaction for machine changes, host/admin debug.
- O03/O09: defer speculative integrations until a named consumer/import contract.
- PL02: local log disclosure can be fixed now. Removing credential-bearing URL
  paths needs the unavailable benefit-service API contract; do not change it by
  guessing a new authentication protocol.
- Engine/asset-dependent items need the exact SDK, restored fixtures and focused
  multiplayer/save/load/render verification described in the review.

## Verification policy

Use actual production code in regression tests when practical. Prefer existing
Unreal automation conventions and small standalone checks of dependency-free
production helpers. Do not add source-text tests or mock copies of algorithms.
Record unrun tests explicitly. `git diff --check` only verifies patch hygiene.
