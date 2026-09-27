import crypto from 'crypto';
import { analyzeWithCppEngine } from '../services/cppEngineService.js';
import { publishAnalysisJob } from '../config/rabbitmq.js';

export async function handleAnalyzeCode(req, res) {
  try {
    const { sourceCode, referenceCode, language } = req.body;


    if (!sourceCode || typeof sourceCode !== 'string' || sourceCode.trim().length === 0) {
      return res.status(400).json({
        success: false,
        error: 'sourceCode is required and must be a non-empty string.'
      });
    }

    const startTime = performance.now();
    const jobId = crypto.randomUUID();

    // 2. Phase 1 (Synchronous Fast Path): Call C++ DSA Engine
    const cppMetrics = await analyzeWithCppEngine({
      sourceCode,
      referenceCode,
      language: language || 'cpp'
    });

    // 3. Phase 2 (Asynchronous Event Path): Push job to RabbitMQ for Python GenAI Worker
    const queuedSuccessfully = publishAnalysisJob({
      jobId,
      sourceCode,
      language: language || 'cpp',
      cppMetrics,
      createdAt: new Date().toISOString()
    });

    const executionTimeMs = Number((performance.now() - startTime).toFixed(2));

    
    return res.status(202).json({
      success: true,
      jobId,
      status: queuedSuccessfully ? 'CPP_ANALYZED_AI_QUEUED' : 'CPP_ANALYZED_ONLY',
      gatewayLatencyMs: executionTimeMs,
      cppReport: cppMetrics
    });
  } catch (error) {
    console.error('[AnalyzeController] Error:', error);
    return res.status(500).json({
      success: false,
      error: 'Internal server error during code analysis.'
    });
  }
}

