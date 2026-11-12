const crypto = require("node:crypto");
const { Pool } = require("pg");

const SESSION_TTL = 8 * 60 * 60 * 1000;
const allowedRoles = new Set(["patient", "doctor", "admin"]);

if (!process.env.DATABASE_URL) {
  throw new Error("DATABASE_URL is required for persistent sessions.");
}

const pool = new Pool({
  connectionString: process.env.DATABASE_URL,
  max: 10,
  idleTimeoutMillis: 30_000,
});

async function initializeSessionStore() {
  await pool.query("SELECT 1");
  await pool.query("DELETE FROM public.clinic_sessions WHERE expires_at <= NOW()");
  const cleanupTimer = setInterval(() => {
    pool.query("DELETE FROM public.clinic_sessions WHERE expires_at <= NOW()")
      .catch((error) => console.warn(`Expired session cleanup failed: ${error.message}`));
  }, 60 * 60 * 1000);
  cleanupTimer.unref();
  console.log("Persistent session store is ready.");
}

async function createSession(role, userId) {
  if (!allowedRoles.has(role)) throw new Error("Invalid session role.");
  const sessionId = crypto.randomBytes(32).toString("hex");
  const expiresAt = new Date(Date.now() + SESSION_TTL);
  await pool.query(
    `INSERT INTO public.clinic_sessions (session_id, user_role, user_id, expires_at)
     VALUES ($1, $2, $3, $4)`,
    [sessionId, role, String(userId), expiresAt],
  );
  return sessionId;
}

async function getSession(sessionId) {
  const result = await pool.query(
    `SELECT user_role, user_id, expires_at
     FROM public.clinic_sessions
     WHERE session_id = $1`,
    [sessionId],
  );
  const record = result.rows[0];
  if (!record) return null;

  const currentExpiry = new Date(record.expires_at).getTime();
  if (!Number.isFinite(currentExpiry) || currentExpiry <= Date.now()) {
    await deleteSession(sessionId);
    return null;
  }

  const expiresAt = new Date(Date.now() + SESSION_TTL);
  await pool.query(
    `UPDATE public.clinic_sessions
     SET expires_at = $1, last_seen_at = NOW()
     WHERE session_id = $2`,
    [expiresAt, sessionId],
  );
  return {
    role: record.user_role,
    id: record.user_id,
    expiresAt: expiresAt.getTime(),
  };
}

async function deleteSession(sessionId) {
  await pool.query("DELETE FROM public.clinic_sessions WHERE session_id = $1", [sessionId]);
}

module.exports = {
  initializeSessionStore,
  createSession,
  getSession,
  deleteSession,
};
