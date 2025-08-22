import express from "express";
import http from "http";
import { Server as SocketIOServer } from "socket.io";
import cors from "cors";
import fetch from "node-fetch";
import dotenv from "dotenv";
import path from "path";
import { fileURLToPath } from "url";

dotenv.config();
const app = express();
const server = http.createServer(app);
const io = new SocketIOServer(server, {
  cors: { origin: "*" },
});

app.use(cors());
app.use(express.json());

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// In-memory store (simple demo)
const state = {
  latest: {}, // by deviceId
  history: [], // [{ts, deviceId, temperature, humidity, pressure, o2, waterLevel, relay}]
};

// Serve static dashboard
app.use(express.static(path.join(__dirname, "public")));

// Ingest endpoint for ESP32S3
app.post("/ingest", (req, res) => {
  try {
    const {
      deviceId = "xiao-esp32s3",
      temperature,
      humidity,
      pressure,
      o2,
      waterLevel,
      relay,
      ts,
    } = req.body || {};

    const reading = {
      ts: ts || Date.now(),
      deviceId,
      temperature: Number(temperature),
      humidity: humidity != null ? Number(humidity) : null,
      pressure: pressure != null ? Number(pressure) : null,
      o2: o2 != null ? Number(o2) : null,
      waterLevel: waterLevel != null ? Number(waterLevel) : null,
      relay: !!relay,
    };

    state.latest[deviceId] = reading;
    state.history.push(reading);
    if (state.history.length > 2000) state.history.shift();

    io.emit("reading", reading);
    return res.json({ ok: true });
  } catch (e) {
    console.error(e);
    return res.status(400).json({ ok: false, error: e.message });
  }
});

// Fetch latest/history
app.get("/data/latest", (req, res) => {
  res.json({ latest: state.latest });
});
app.get("/data/history", (req, res) => {
  res.json({ history: state.history.slice(-500) });
});

// Meta Llama (Groq) advisory endpoint — addressing HEAT only (no vision)
app.post("/advice/heat", async (req, res) => {
  try {
    const { temperature, humidity } = req.body || {};
    const key = process.env.GROQ_API_KEY;

    if (!key) {
      return res.status(500).json({ ok: false, error: "GROQ_API_KEY not set" });
    }

    const sys = `You are an on-device climate advisor for a household/SME in a hot, humid city. 
Return:
- heat_index (°C) estimated using temp & humidity (explain briefly),
- risk_level (low/med/high),
- immediate_actions (bullet list, 4 items max),
- energy_saving_tip (1 sentence),
- no vision or camera suggestions.`;

    const user = `Current conditions: temperature=${temperature}°C, humidity=${
      humidity?.toFixed?.(1) ?? humidity
    }%.
Context: you are an expert care taker for an elderly person who is also expert in environmental issue and heat adaptation. 
Sensors: ESP32S3 Xiao + temp sensor.
Goal: concise practical guidance on how to cool down and protect the elderly from heat stroke.`;

    const resp = await fetch(
      "https://api.groq.com/openai/v1/chat/completions",
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${key}`,
        },
        body: JSON.stringify({
          model: "llama-3.3-70b-versatile",
          messages: [
            { role: "system", content: sys },
            { role: "user", content: user },
          ],
          temperature: 0.2,
          max_tokens: 350,
        }),
      }
    );

    const data = await resp.json();

    // Debug log
    console.log("Groq raw response:", JSON.stringify(data, null, 2));

    // Safely extract assistant text
    const text =
      data?.choices?.[0]?.message?.content ??
      data?.choices?.[0]?.delta?.content ??
      "No response from Groq";

    res.json({ ok: true, text });
  } catch (e) {
    console.error("Error in /advice/heat:", e);
    res.status(500).json({ ok: false, error: e.message });
  }
});

const PORT = process.env.PORT || 8080;
server.listen(PORT, () => console.log(`Server running on port ${PORT}`));

// WebSocket up
io.on("connection", (socket) => {
  console.log("Client connected");
  socket.emit("bootstrap", { history: state.history.slice(-200) });
});
