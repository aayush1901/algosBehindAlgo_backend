import { Router } from 'express';
import { handleAnalyzeCode } from '../controllers/analyzeController.js';

const router = Router();

router.post('/analyze', handleAnalyzeCode);

export default router;