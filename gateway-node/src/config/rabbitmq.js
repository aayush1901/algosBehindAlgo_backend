import amqp from 'amqplib';

let connection = null;
let channel = null;

const QUEUE_NAME =  process.env.ANALYSIS_QUEUE_NAME || 'code_analysis_queue';

export async function connectRabbitMQ(){

   const rabbitUrl = process.env.RABBITMQ_URL;

   if (!rabbitUrl) {
    console.warn('[RabbitMQ] No RABBITMQ_URL set. Running in local fallback mode (Queue disabled).');
    return null;
  }

  try{

      connection = await amqp.connect(rabbitUrl);
      connection.on('error',(err)=>{
         console.error('[RabbitMQ] Connection error:', err.message);
      });
      // our virtual channel inside tcp connection 
      channel = await connection.createChannel();

   // 3. Assert Queue: creates the queue if it doesn't exist yet.
    await channel.assertQueue(QUEUE_NAME, { durable: true });

    console.log(`[RabbitMQ] Connected and queue "${QUEUE_NAME}" is ready.`);
    return channel;


  }catch(error){
   console.warn(`[RabbitMQ] Broker not reachable yet (${error.message}). Server will continue in sync-only mode.`);
    return null;
  }

}

export function publishAnalysisJob(jobPayload) {
  if (!channel) {
    console.warn('[RabbitMQ] Channel not active. Skipping queue publish for jobId:', jobPayload.jobId);
    return false;
  }

  // RabbitMQ transmits raw binary byte buffers, so we serialize our JS object to JSON string -> Buffer
  const messageBuffer = Buffer.from(JSON.stringify(jobPayload));

  // persistent: true tells RabbitMQ to save the message to disk so it isn't lost on crash
  return channel.sendToQueue(QUEUE_NAME, messageBuffer, { persistent: true });
}