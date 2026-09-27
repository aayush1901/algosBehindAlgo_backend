import express from 'express';
import cors from 'cors';
import dotenv from 'dotenv';
import analyzeRoutes from './routes/analyzeRoutes.js';
import { connectRabbitMQ } from './config/rabbitmq.js';

// Load environment variables from root .env if present
dotenv.config({ path: '../.env' });

const app = express();
const PORT = process.env.PORT || 4000;

// Built-in Middlewares
app.use(cors());
app.use(express.json({ limit: '1mb' })); // Parse JSON bodies up to 1MB

// Health Check Route (Used by Render and Frontend Warmup Pings)
app.get('/health', (req, res) => {
  res.status(200).json({
    service: 'algosBehindAlgo-gateway-node',
    status: 'OK',
    timestamp: new Date().toISOString()
  });
});

// API v1 Routes
app.use('/api/v1', analyzeRoutes);

// Start Server & Initialize RabbitMQ Connection
async function startServer() {
  await connectRabbitMQ();

  app.listen(PORT, () => {
    console.log(`[Gateway] Node.js API Gateway running on http://localhost:${PORT}`);
  });
}

startServer();