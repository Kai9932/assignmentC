const http = require("node:http");
const fs = require("node:fs");
const path = require("node:path");
const crypto = require("node:crypto");
const { initializeStore, readTable, writeTable } = require("./dataStore");
const { initializeSessionStore, createSession, getSession, deleteSession } = require("./sessionStore");

const PORT = Number(process.env.PORT || 5000);
const SESSION_SECRET = process.env.SESSION_SECRET;
if (!SESSION_SECRET) {
  console.error("SESSION_SECRET is required. Configure it in Replit Secrets.");
  process.exit(1);
}

const PUBLIC_DIR = path.join(__dirname, "public");

function csvCell(value) {
  const text = String(value ?? "");
  return /[",\r\n]/.test(text) ? `"${text.replaceAll('"', '""')}"` : text;
}

function idFor(prefix, records) {
  const max = records.reduce((highest, record) => {
    const match = record.id?.match(new RegExp(`^${prefix}-(\\d+)$`));
    return Math.max(highest, match ? Number(match[1]) : 0);
  }, 0);
  return `${prefix}-${String(max + 1).padStart(4, "0")}`;
}

function hashPassword(password) {
  const salt = crypto.randomBytes(16).toString("hex");
  const hash = crypto.scryptSync(password, salt, 64).toString("hex");
  return `scrypt$${salt}$${hash}`;
}

function checkPassword(password, stored) {
  const [algorithm, salt, savedHex] = String(stored || "").split("$");
  if (algorithm !== "scrypt" || !salt || !savedHex) return false;
  const saved = Buffer.from(savedHex, "hex");
  const candidate = crypto.scryptSync(password, salt, saved.length);
  return saved.length === candidate.length && crypto.timingSafeEqual(saved, candidate);
}

function publicUser(role, record) {
  if (!record) return null;
  if (role === "patient") return { id: record.id, name: record.name, age: Number(record.age), ailment: record.ailment, role };
  if (role === "doctor") return { id: record.id, name: record.name, specialty: record.specialty, room: record.room, role };
  return { id: record.id, name: record.name, department: record.department, accessLevel: Number(record.accessLevel), role: "admin" };
}

function userRecord(role, id) {
  const table = role === "patient" ? "patients" : role === "doctor" ? "doctors" : "admins";
  return readTable(table).find((record) => record.id === id);
}

function userForSession(session) {
  const record = userRecord(session.role, session.id);
  return publicUser(session.role, record);
}

function cookieIsSecure(req) {
  return req.headers["x-forwarded-proto"] === "https" || req.socket.encrypted;
}

function setSessionCookie(res, req, sid) {
  const signature = crypto.createHmac("sha256", SESSION_SECRET).update(sid).digest("hex");
  const secure = cookieIsSecure(req) ? "; Secure" : "";
  res.setHeader("Set-Cookie", `clinic_session=${sid}.${signature}; Path=/; HttpOnly; SameSite=Strict; Max-Age=28800${secure}`);
}

function clearSessionCookie(res, req) {
  const secure = cookieIsSecure(req) ? "; Secure" : "";
  res.setHeader("Set-Cookie", `clinic_session=; Path=/; HttpOnly; SameSite=Strict; Max-Age=0${secure}`);
}

async function currentSession(req, res) {
  const cookie = req.headers.cookie?.split(";").map((part) => part.trim()).find((part) => part.startsWith("clinic_session="));
  if (!cookie) return null;
  const value = cookie.slice("clinic_session=".length);
  const dot = value.indexOf(".");
  if (dot < 0) return null;
  const sid = value.slice(0, dot);
  const signature = value.slice(dot + 1);
  const expected = crypto.createHmac("sha256", SESSION_SECRET).update(sid).digest("hex");
  const signatureBytes = Buffer.from(signature);
  const expectedBytes = Buffer.from(expected);
  if (signatureBytes.length !== expectedBytes.length || !crypto.timingSafeEqual(signatureBytes, expectedBytes)) return null;
  const session = await getSession(sid);
  if (!session) return null;
  const user = userForSession(session);
  if (!user) {
    await deleteSession(sid);
    return null;
  }
  setSessionCookie(res, req, sid);
  return { sid, session, user };
}

function sendJson(res, status, data) {
  res.writeHead(status, {
    "Content-Type": "application/json; charset=utf-8",
    "Cache-Control": "no-store",
    "X-Content-Type-Options": "nosniff",
    "Referrer-Policy": "same-origin",
  });
  res.end(JSON.stringify(data));
}

function readBody(req) {
  return new Promise((resolve, reject) => {
    let body = "";
    req.on("data", (chunk) => {
      body += chunk;
      if (body.length > 1024 * 1024) {
        reject(Object.assign(new Error("Request is too large."), { status: 413 }));
        req.destroy();
      }
    });
    req.on("end", () => {
      if (!body) return resolve({});
      try {
        resolve(JSON.parse(body));
      } catch {
        reject(Object.assign(new Error("Request body must be valid JSON."), { status: 400 }));
      }
    });
    req.on("error", reject);
  });
}

function requireRole(auth, roles) {
  if (!auth) throw Object.assign(new Error("Please sign in to continue."), { status: 401 });
  if (!roles.includes(auth.user.role)) throw Object.assign(new Error("You do not have access to this action."), { status: 403 });
  return auth.user;
}

function cleanText(value, label, max = 120) {
  const text = String(value ?? "").trim();
  if (!text || text.length > max) {
    throw Object.assign(new Error(`${label} is required and must be ${max} characters or fewer.`), { status: 400 });
  }
  return text;
}

function validName(value) {
  return /^[\p{L}][\p{L}\p{M} .'-]{0,79}$/u.test(value);
}

function validUsername(value) {
  return /^[A-Za-z][A-Za-z0-9]{2,29}$/.test(value);
}

function validatePassword(value) {
  if (typeof value !== "string" || value.length < 8 || value.length > 128) {
    throw Object.assign(new Error("Password must be between 8 and 128 characters."), { status: 400 });
  }
}

function validateAge(value) {
  const age = Number(value);
  if (!Number.isInteger(age) || age < 1 || age > 100) {
    throw Object.assign(new Error("Age must be a whole number from 1 to 100."), { status: 400 });
  }
  return age;
}

function joinAppointments(records = readTable("appointments")) {
  const patients = readTable("patients");
  const doctors = readTable("doctors");
  return records.map((appointment) => {
    const patient = patients.find((entry) => entry.id === appointment.patientId);
    const doctor = doctors.find((entry) => entry.id === appointment.doctorId);
    return {
      ...appointment,
      patientName: patient?.name ?? "Patient record unavailable",
      doctorName: doctor?.name ?? "Doctor record unavailable",
      specialty: doctor?.specialty ?? "",
    };
  }).sort((a, b) => `${a.date} ${a.time}`.localeCompare(`${b.date} ${b.time}`));
}

function appointmentDate(date, time, timeZone) {
  if (!/^\d{4}-\d{2}-\d{2}$/.test(date) || !/^(?:[01]\d|2[0-3]):[0-5]\d$/.test(time)) return false;
  const parsed = new Date(`${date}T00:00:00.000Z`);
  if (!Number.isFinite(parsed.getTime()) || parsed.toISOString().slice(0, 10) !== date) return false;
  try {
    const parts = new Intl.DateTimeFormat("en", {
      timeZone,
      year: "numeric",
      month: "2-digit",
      day: "2-digit",
      hour: "2-digit",
      minute: "2-digit",
      hourCycle: "h23",
    }).formatToParts(new Date());
    const localNow = Object.fromEntries(parts.map(({ type, value }) => [type, value]));
    return `${date}T${time}` > `${localNow.year}-${localNow.month}-${localNow.day}T${localNow.hour}:${localNow.minute}`;
  } catch {
    return false;
  }
}

function errorResponse(error, res) {
  const status = error.status || 500;
  if (status >= 500) console.error(error);
  sendJson(res, status, { error: status >= 500 ? "Something went wrong. Please try again." : error.message });
}

async function handleApi(req, res, url) {
  const route = url.pathname;
  const auth = await currentSession(req, res);

  if (req.method === "GET" && route === "/api/session") {
    const admins = readTable("admins");
    return sendJson(res, 200, {
      setupRequired: admins.length === 0,
      user: auth?.user ?? null,
      counts: auth?.user.role === "admin" ? {
        patients: readTable("patients").length,
        doctors: readTable("doctors").length,
        appointments: readTable("appointments").length,
      } : null,
    });
  }

  if (req.method === "GET" && route === "/api/doctors") {
    return sendJson(res, 200, readTable("doctors").map(({ id, name, specialty, room }) => ({ id, name, specialty, room })));
  }

  if (req.method === "POST" && route === "/api/setup") {
    if (readTable("admins").length) throw Object.assign(new Error("Initial setup is already complete."), { status: 409 });
    const body = await readBody(req);
    const name = cleanText(body.name, "Name", 80);
    const department = cleanText(body.department, "Department", 80);
    const username = cleanText(body.username, "Username", 30);
    validatePassword(body.password);
    if (!validName(name)) throw Object.assign(new Error("Enter a valid name."), { status: 400 });
    if (!validUsername(username)) throw Object.assign(new Error("Username must start with a letter and contain 3–30 letters or numbers."), { status: 400 });
    const admins = readTable("admins");
    if (admins.length) throw Object.assign(new Error("Initial setup is already complete."), { status: 409 });
    if (admins.some((entry) => entry.username.toLowerCase() === username.toLowerCase())) {
      throw Object.assign(new Error("That username is already taken."), { status: 409 });
    }
    const admin = {
      id: idFor("ADM", admins), name, department, accessLevel: "3", username,
      passwordHash: hashPassword(body.password),
    };
    await writeTable("admins", [...admins, admin]);
    const sid = await createSession("admin", admin.id);
    setSessionCookie(res, req, sid);
    return sendJson(res, 201, { user: publicUser("admin", admin) });
  }

  if (req.method === "POST" && route === "/api/register") {
    const body = await readBody(req);
    const name = cleanText(body.name, "Name", 80);
    const ailment = cleanText(body.ailment, "Reason for visit", 240);
    const age = validateAge(body.age);
    const username = cleanText(body.username, "Username", 30);
    validatePassword(body.password);
    if (!validName(name)) throw Object.assign(new Error("Enter a valid name."), { status: 400 });
    if (!validUsername(username)) throw Object.assign(new Error("Username must start with a letter and contain 3–30 letters or numbers."), { status: 400 });
    const patients = readTable("patients");
    if (patients.some((entry) => entry.username.toLowerCase() === username.toLowerCase())) {
      throw Object.assign(new Error("That username is already taken."), { status: 409 });
    }
    const patient = {
      id: idFor("PAT", patients), name, age: String(age), ailment, username,
      passwordHash: hashPassword(body.password), createdAt: new Date().toISOString(),
    };
    await writeTable("patients", [...patients, patient]);
    const sid = await createSession("patient", patient.id);
    setSessionCookie(res, req, sid);
    return sendJson(res, 201, { user: publicUser("patient", patient) });
  }

  if (req.method === "POST" && route === "/api/login") {
    const body = await readBody(req);
    const role = ["patient", "doctor", "admin"].includes(body.role) ? body.role : null;
    if (!role) throw Object.assign(new Error("Choose a valid account type."), { status: 400 });
    const table = role === "patient" ? "patients" : role === "doctor" ? "doctors" : "admins";
    const account = readTable(table).find((entry) => entry.username.toLowerCase() === String(body.username || "").trim().toLowerCase());
    if (!account || !checkPassword(String(body.password || ""), account.passwordHash)) {
      throw Object.assign(new Error("Username or password is incorrect."), { status: 401 });
    }
    const sid = await createSession(role, account.id);
    setSessionCookie(res, req, sid);
    return sendJson(res, 200, { user: publicUser(role, account) });
  }

  if (req.method === "POST" && route === "/api/logout") {
    if (auth) await deleteSession(auth.sid);
    clearSessionCookie(res, req);
    return sendJson(res, 200, { ok: true });
  }

  if (req.method === "GET" && route === "/api/appointments") {
    const user = requireRole(auth, ["patient", "doctor", "admin"]);
    let appointments = joinAppointments();
    if (user.role === "patient") appointments = appointments.filter((entry) => entry.patientId === user.id);
    if (user.role === "doctor") appointments = appointments.filter((entry) => entry.doctorId === user.id);
    return sendJson(res, 200, appointments);
  }

  if (req.method === "POST" && route === "/api/appointments") {
    const user = requireRole(auth, ["patient"]);
    const body = await readBody(req);
    const date = cleanText(body.date, "Date", 10);
    const time = cleanText(body.time, "Time", 5);
    const timeZone = cleanText(body.timeZone || "Asia/Kuala_Lumpur", "Time zone", 60);
    if (!appointmentDate(date, time, timeZone)) {
      throw Object.assign(new Error("Choose a valid future date and time."), { status: 400 });
    }
    const doctors = readTable("doctors");
    const doctor = doctors.find((entry) => entry.id === body.doctorId);
    if (!doctor) throw Object.assign(new Error("Choose an available doctor."), { status: 400 });
    const appointments = readTable("appointments");
    const activeForPatient = appointments.filter((entry) => entry.patientId === user.id && entry.status !== "Cancelled");
    if (activeForPatient.length >= 3) throw Object.assign(new Error("You can have up to three active appointments."), { status: 409 });
    const occupied = appointments.some((entry) => entry.doctorId === doctor.id && entry.date === date && entry.time === time && !["Cancelled", "Rejected"].includes(entry.status));
    if (occupied) throw Object.assign(new Error("That doctor is already booked at this time. Choose another slot."), { status: 409 });
    const appointment = {
      id: idFor("APT", appointments), patientId: user.id, doctorId: doctor.id,
      date, time, timeZone, status: "Pending", createdAt: new Date().toISOString(),
    };
    await writeTable("appointments", [...appointments, appointment]);
    return sendJson(res, 201, joinAppointments([...appointments, appointment]).find((entry) => entry.id === appointment.id));
  }

  const appointmentMatch = route.match(/^\/api\/appointments\/([^/]+)$/);
  if (req.method === "PATCH" && appointmentMatch) {
    const user = requireRole(auth, ["patient", "doctor"]);
    const appointmentId = decodeURIComponent(appointmentMatch[1]);
    const appointments = readTable("appointments");
    const index = appointments.findIndex((entry) => entry.id === appointmentId);
    if (index < 0) throw Object.assign(new Error("Appointment not found."), { status: 404 });
    const appointment = appointments[index];
    const body = await readBody(req);
    if (user.role === "patient") {
      if (appointment.patientId !== user.id || appointment.status === "Cancelled" || appointment.status === "Rejected") {
        throw Object.assign(new Error("This appointment cannot be cancelled."), { status: 403 });
      }
      appointments[index] = { ...appointment, status: "Cancelled" };
    } else {
      if (appointment.doctorId !== user.id || appointment.status !== "Pending") {
        throw Object.assign(new Error("Only your pending appointments can be updated."), { status: 403 });
      }
      if (!["Confirmed", "Rejected"].includes(body.status)) {
        throw Object.assign(new Error("Choose Confirmed or Rejected."), { status: 400 });
      }
      appointments[index] = { ...appointment, status: body.status };
    }
    await writeTable("appointments", appointments);
    return sendJson(res, 200, joinAppointments(appointments).find((entry) => entry.id === appointmentId));
  }

  if (req.method === "GET" && route === "/api/patients") {
    requireRole(auth, ["admin"]);
    return sendJson(res, 200, readTable("patients").map(({ id, name, age, ailment, createdAt }) => ({ id, name, age: Number(age), ailment, createdAt })));
  }

  if (req.method === "POST" && route === "/api/admin/patients") {
    const user = requireRole(auth, ["admin"]);
    if (user.accessLevel < 2) throw Object.assign(new Error("Access level 2 or higher is needed to register patients."), { status: 403 });
    const body = await readBody(req);
    const name = cleanText(body.name, "Name", 80);
    const ailment = cleanText(body.ailment, "Reason for visit", 240);
    const age = validateAge(body.age);
    const username = cleanText(body.username, "Username", 30);
    validatePassword(body.password);
    if (!validName(name)) throw Object.assign(new Error("Enter a valid name."), { status: 400 });
    if (!validUsername(username)) throw Object.assign(new Error("Username must start with a letter and contain 3–30 letters or numbers."), { status: 400 });
    const patients = readTable("patients");
    if ([...patients, ...readTable("doctors"), ...readTable("admins")].some((entry) => entry.username.toLowerCase() === username.toLowerCase())) {
      throw Object.assign(new Error("That username is already taken."), { status: 409 });
    }
    const patient = {
      id: idFor("PAT", patients), name, age: String(age), ailment, username,
      passwordHash: hashPassword(body.password), createdAt: new Date().toISOString(),
    };
    await writeTable("patients", [...patients, patient]);
    return sendJson(res, 201, { user: publicUser("patient", patient) });
  }

  if (req.method === "GET" && route === "/api/admin/doctors") {
    requireRole(auth, ["admin"]);
    return sendJson(res, 200, readTable("doctors").map(({ id, name, specialty, room, username }) => ({ id, name, specialty, room, username })));
  }

  const exportMatch = route.match(/^\/api\/admin\/export\/(patients|doctors|appointments|admins)$/);
  if (req.method === "GET" && exportMatch) {
    requireRole(auth, ["admin"]);
    const table = exportMatch[1];
    let rows;
    if (table === "patients") {
      rows = readTable("patients").map(({ id, name, ailment, age, createdAt }) => ({ id, name, ailment, age, createdAt }));
    } else if (table === "doctors") {
      rows = readTable("doctors").map(({ id, name, specialty, room }) => ({ id, name, specialty, room }));
    } else if (table === "appointments") {
      rows = joinAppointments().map(({ id, patientId, doctorId, date, time, status, timeZone, createdAt, patientName, doctorName, specialty }) => ({
        id, patientId, doctorId, date, time, status, timeZone, createdAt, patientName, doctorName, specialty,
      }));
    } else {
      rows = readTable("admins").map(({ username, id, name, department, accessLevel }) => ({ username, id, name, department, accessLevel }));
    }
    const headers = rows.length ? Object.keys(rows[0]) : table === "patients"
      ? ["id", "name", "ailment", "age", "createdAt"]
      : table === "doctors"
        ? ["id", "name", "specialty", "room"]
        : table === "appointments"
          ? ["id", "patientId", "doctorId", "date", "time", "status", "timeZone", "createdAt", "patientName", "doctorName", "specialty"]
          : ["username", "id", "name", "department", "accessLevel"];
    const spreadsheetCell = (value) => {
      const text = String(value ?? "");
      const safeText = /^[\s]*[=+\-@]/.test(text) ? `'${text}` : text;
      return csvCell(safeText);
    };
    const csv = [headers.join(","), ...rows.map((row) => headers.map((key) => spreadsheetCell(row[key])).join(","))].join("\r\n") + "\r\n";
    res.writeHead(200, {
      "Content-Type": "text/csv; charset=utf-8",
      "Content-Disposition": `attachment; filename="${table}.csv"`,
      "Cache-Control": "no-store",
      "X-Content-Type-Options": "nosniff",
    });
    return res.end(csv);
  }

  if (req.method === "POST" && route === "/api/admin/doctors") {
    const user = requireRole(auth, ["admin"]);
    if (user.accessLevel < 2) throw Object.assign(new Error("Access level 2 or higher is needed to add doctors."), { status: 403 });
    const body = await readBody(req);
    const name = cleanText(body.name, "Name", 80);
    const specialty = cleanText(body.specialty, "Specialty", 80);
    const room = cleanText(body.room, "Room", 20);
    const username = cleanText(body.username, "Username", 30);
    validatePassword(body.password);
    if (!validName(name)) throw Object.assign(new Error("Enter a valid name."), { status: 400 });
    if (!validUsername(username)) throw Object.assign(new Error("Username must start with a letter and contain 3–30 letters or numbers."), { status: 400 });
    const doctors = readTable("doctors");
    if ([...readTable("patients"), ...doctors, ...readTable("admins")].some((entry) => entry.username.toLowerCase() === username.toLowerCase())) {
      throw Object.assign(new Error("That username is already taken."), { status: 409 });
    }
    const doctor = {
      id: idFor("DOC", doctors), name, specialty, room, username,
      passwordHash: hashPassword(body.password),
    };
    await writeTable("doctors", [...doctors, doctor]);
    return sendJson(res, 201, { id: doctor.id, name, specialty, room, username });
  }

  if (req.method === "POST" && route === "/api/admin/admins") {
    const user = requireRole(auth, ["admin"]);
    if (user.accessLevel < 3) throw Object.assign(new Error("Only level 3 admins can create admin accounts."), { status: 403 });
    const body = await readBody(req);
    const name = cleanText(body.name, "Name", 80);
    const department = cleanText(body.department, "Department", 80);
    const username = cleanText(body.username, "Username", 30);
    const accessLevel = Number(body.accessLevel);
    validatePassword(body.password);
    if (!validName(name)) throw Object.assign(new Error("Enter a valid name."), { status: 400 });
    if (!validUsername(username)) throw Object.assign(new Error("Username must start with a letter and contain 3–30 letters or numbers."), { status: 400 });
    if (![1, 2, 3].includes(accessLevel)) throw Object.assign(new Error("Choose access level 1, 2, or 3."), { status: 400 });
    const admins = readTable("admins");
    if ([...readTable("patients"), ...readTable("doctors"), ...admins].some((entry) => entry.username.toLowerCase() === username.toLowerCase())) {
      throw Object.assign(new Error("That username is already taken."), { status: 409 });
    }
    const admin = { id: idFor("ADM", admins), name, department, accessLevel: String(accessLevel), username, passwordHash: hashPassword(body.password) };
    await writeTable("admins", [...admins, admin]);
    return sendJson(res, 201, { user: publicUser("admin", admin) });
  }

  const patientMatch = route.match(/^\/api\/admin\/patients\/([^/]+)$/);
  if (req.method === "PATCH" && patientMatch) {
    const user = requireRole(auth, ["admin"]);
    if (user.accessLevel < 2) throw Object.assign(new Error("Access level 2 or higher is needed to edit patient records."), { status: 403 });
    const patientId = decodeURIComponent(patientMatch[1]);
    const patients = readTable("patients");
    const index = patients.findIndex((entry) => entry.id === patientId);
    if (index < 0) throw Object.assign(new Error("Patient not found."), { status: 404 });
    const body = await readBody(req);
    patients[index] = {
      ...patients[index],
      age: String(body.age === undefined ? patients[index].age : validateAge(body.age)),
      ailment: cleanText(body.ailment ?? patients[index].ailment, "Reason for visit", 240),
    };
    await writeTable("patients", patients);
    const { id, name, age, ailment } = patients[index];
    return sendJson(res, 200, { id, name, age: Number(age), ailment });
  }

  if (req.method === "DELETE" && patientMatch) {
    const user = requireRole(auth, ["admin"]);
    if (user.accessLevel < 3) throw Object.assign(new Error("Only level 3 admins can delete patient records."), { status: 403 });
    const patientId = decodeURIComponent(patientMatch[1]);
    const patients = readTable("patients");
    if (!patients.some((entry) => entry.id === patientId)) throw Object.assign(new Error("Patient not found."), { status: 404 });
    await writeTable("patients", patients.filter((entry) => entry.id !== patientId));
    await writeTable("appointments", readTable("appointments").filter((entry) => entry.patientId !== patientId));
    return sendJson(res, 200, { ok: true });
  }

  throw Object.assign(new Error("Not found."), { status: 404 });
}

const contentTypes = {
  ".html": "text/html; charset=utf-8",
  ".css": "text/css; charset=utf-8",
  ".js": "text/javascript; charset=utf-8",
  ".svg": "image/svg+xml",
  ".jpg": "image/jpeg",
  ".jpeg": "image/jpeg",
  ".png": "image/png",
  ".ico": "image/x-icon",
};

let apiRequestQueue = Promise.resolve();
const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, "http://localhost");
  if (url.pathname.startsWith("/api/")) {
    const processRequest = async () => {
      try {
        await handleApi(req, res, url);
      } catch (error) {
        if (!res.headersSent && !res.destroyed) errorResponse(error, res);
      }
    };
    const queuedRequest = apiRequestQueue.then(processRequest, processRequest);
    apiRequestQueue = queuedRequest.catch(() => {});
    await queuedRequest;
    return;
  }
  const requestedPath = url.pathname === "/" ? "index.html" : decodeURIComponent(url.pathname.slice(1));
  const filePath = path.resolve(PUBLIC_DIR, requestedPath);
  if (!filePath.startsWith(`${PUBLIC_DIR}${path.sep}`)) {
    res.writeHead(403).end("Forbidden");
    return;
  }
  fs.readFile(filePath, (error, content) => {
    if (error) {
      res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" }).end("Not found");
      return;
    }
    res.writeHead(200, {
      "Content-Type": contentTypes[path.extname(filePath)] || "application/octet-stream",
      "X-Content-Type-Options": "nosniff",
      "Cache-Control": [".html", ".css", ".js"].includes(path.extname(filePath)) ? "no-cache" : "public, max-age=300",
    });
    res.end(content);
  });
});

Promise.all([initializeStore(), initializeSessionStore()])
  .then(() => server.listen(PORT, "0.0.0.0", () => console.log(`Hospital Appointment System listening on port ${PORT}`)))
  .catch((error) => {
    console.error(`Unable to initialize application storage: ${error.message}`);
    process.exit(1);
  });
