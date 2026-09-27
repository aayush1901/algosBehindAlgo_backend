export async function analyzeWithCppEngine({ sourceCode, referenceCode, language }) {
  const cppEngineUrl = process.env.CPP_ENGINE_URL || 'http://localhost:8080';

  try {
    const response = await fetch(`${cppEngineUrl}/internal/analyze-dsa`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        sourceCode,
        referenceCode: referenceCode || '',
        language: language || 'cpp'
      }),
      // Timeout after 3 seconds so Gateway never hangs if C++ service is down
      signal: AbortSignal.timeout(3000)
    });

    if (!response.ok) {
      throw new Error(`C++ Engine responded with status ${response.status}`);
    }

    return await response.json();
  } catch (error) {
    console.warn(`[CppEngineService] C++ Engine offline (${error.message}). Returning fallback stub for dev testing.`);

  
    return {
      engineMode: 'FALLBACK_JS_STUB',
      callGraph: {
        topoOrder: ['helper', 'solve', 'main'],
        hasRecursionCycle: false,
        detectedCycles: []
      },
      controlFlow: {
        maxLoopDepth: 2,
        estimatedComplexity: 'O(N^2)',
        hasDeadCode: false,
        deadCodeLines: []
      },
      similarity: {
        scorePercentage: referenceCode ? 75.0 : 0.0,
        windowSizeK: 5
      }
    };
  }
}