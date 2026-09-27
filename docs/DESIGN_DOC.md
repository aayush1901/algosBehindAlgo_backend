cat << 'EOF' > docs/DESIGN_DOC.md
# System Design Document: algosBehindAlgo Backend

## 1. Overview & Objective
`algosBehindAlgo` is an event-driven, polyglot code analysis backend. It combines a deterministic **C++20 Algorithmic Engine** (for sub-5ms structural, graph, and token-level analysis) with an asynchronous **Python GenAI Worker** orchestrated via **RabbitMQ** and a **Node.js/TypeScript API Gateway**.

## 2. High-Level Architecture (Two-Phase Hybrid Pipeline)
To balance instant user feedback with long-running LLM generation (3-8s), the system separates execution into two phases:

### Phase 1: Synchronous Fast Path (< 15ms)
1. **API Gateway (`gateway-node`)** receives `POST /api/v1/analyze` with `{ sourceCode, referenceCode, language }`.
2. Gateway validates the payload via **Zod** and sends an internal synchronous HTTP request to the **C++20 Engine (`engine-cpp`)**.
3. **C++20 Engine** executes the Lexer and 3 core algorithms in-memory, returning deterministic metrics in ~2-5ms.
4. Gateway persists the initial record in **MongoDB** (`status: "AI_QUEUED"`), publishes an async job containing `{ jobId, sourceCode, cppMetrics }` to **RabbitMQ** (`code_analysis_queue`), and immediately returns `HTTP 202 Accepted` with the C++ report to the client.

### Phase 2: Asynchronous Event-Driven Path (Background)
1. **Python Worker (`ai-python`)** consumes the message from **RabbitMQ** with `prefetch_count = 1`.
2. Using the deterministic `cppMetrics` (loop nesting depth, recursion cycles, dead code lines), Python prompts **Gemini API** with structured JSON schema enforcement to generate adversarial stress-test inputs that target the code's exact complexity bottleneck.
3. Python updates the **MongoDB** document (`status: "COMPLETED"`) and sends an explicit `ACK` to RabbitMQ.

## 3. Core C++20 Algorithms (`engine-cpp`)
| Module | Algorithm | Input Structure | Output / Detection | Complexity |
|---|---|---|---|---|
| **CallGraphAnalyzer** | **Kahn's Topological Sort (BFS)** | Directed graph $G(V,E)$ of function declarations & calls | Bottom-up unit testing order + **Direct/Mutual Recursion Cycle Detection** (`visited < V`) | $O(V + E)$ |
| **ControlFlowAnalyzer** | **Tree/Graph DFS** | Scope block tree (`for`, `while`, `if`, `return`, `break`) | **Max Loop Nesting Depth** (static time complexity estimate) + **Unreachable Dead Code** lines | $O(V + E)$ |
| **SimilarityEngine** | **Rabin-Karp Rolling Hash** | Canonicalized token stream ($K=5$ sliding window) | Plagiarism/Structural Similarity score immune to variable/function renaming | $O(N)$ |

## 4. Fault Tolerance & Production Patterns
- **RabbitMQ Manual Acknowledgments (`basic_ack`):** If the Python AI worker crashes or Gemini times out, the message is not lost; RabbitMQ re-queues it or routes it to a Dead Letter Queue (DLQ).
- **Graceful Degradation:** Even if the Python AI service or RabbitMQ is temporarily unavailable, Phase 1 still returns the complete C++ deterministic analysis to the user.
- **Cold-Start Mitigation (Render Deployment):** Gateway exposes a `/api/v1/warmup` endpoint that asynchronously pings internal health checks of `engine-cpp` and `ai-python`.
EOF

git rm -f docs/.gitkeep
git add docs/DESIGN_DOC.md
git commit -m "docs: add system architecture and algorithm design document"
git push origin main