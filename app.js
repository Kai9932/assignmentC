const app = document.querySelector("#app");
const toast = document.querySelector("#toast");
let selectedRole = "patient";
let currentUser = null;
let toastTimer;

async function api(url, options = {}) {
  const response = await fetch(url, {
    credentials: "same-origin",
    ...options,
    headers: { "Content-Type": "application/json", ...(options.headers || {}) },
  });
  const type = response.headers.get("content-type") || "";
  const data = type.includes("application/json") ? await response.json() : null;
  if (!response.ok) throw new Error(data?.error || "Request could not be completed.");
  return data;
}

function showToast(message, isError = false) {
  toast.textContent = message;
  toast.className = `toast show${isError ? " error" : ""}`;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => { toast.className = "toast"; }, 3100);
}

function escapeHtml(value = "") {
  return String(value).replace(/[&<>"']/g, (char) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;",
  })[char]);
}

function brand() {
  return `<div class="brand"><img class="brand-logo" src="/xiamen-seal.jpg" alt="Xiamen University Malaysia seal"><span class="brand-name">Xiamen Medical Center</span></div>`;
}

function renderAuth(setupRequired = false, formMode = "login") {
  currentUser = null;
  const isSetup = setupRequired && selectedRole === "admin";
  const mode = isSetup ? "setup" : formMode;
  const isRegister = mode === "register";
  const title = isSetup ? "Set up your admin access" : isRegister ? "Create your patient account" : "Sign in to continue";
  const subtitle = isSetup
    ? "Create the primary administrator account to manage your clinic."
    : isRegister
      ? "A few details to get your care journey started."
      : "Choose your portal to access your care dashboard.";
  const loginNote = selectedRole === "patient"
    ? "Patient accounts are created with patient sign-up. Contact your administrator for a doctor or admin account."
    : selectedRole === "doctor"
      ? "Doctor accounts are created by a clinic administrator."
      : "Admin access is managed by the clinic administrator.";
  app.innerHTML = `
    <div class="auth-layout">
      <section class="welcome-panel">
        <div class="auth-brand">
          <img class="auth-brand-seal" src="/xiamen-seal.jpg" alt="Xiamen University seal">
          <span class="auth-brand-copy"><strong>Xiamen Medical Center</strong><small>HOSPITAL INFORMATION SYSTEM</small></span>
        </div>
        <div class="welcome-content">
          <div class="welcome-pill"><span class="calendar-mark" aria-hidden="true"></span> Hospital Appointments</div>
          <h1>Your health,<br>in good hands.</h1>
          <span class="welcome-rule" aria-hidden="true"></span>
          <p>A simpler way to connect with your care team, manage appointments, and keep your next visit on track.</p>
          <div class="welcome-perks" aria-label="Care portal benefits">
            <div class="perk"><span class="perk-icon" aria-hidden="true"><span class="calendar-mark"></span></span><span>Book<br>appointments</span></div>
            <i aria-hidden="true"></i>
            <div class="perk"><span class="perk-icon people-mark" aria-hidden="true"></span><span>Access<br>care team</span></div>
            <i aria-hidden="true"></i>
            <div class="perk"><span class="perk-icon history-mark" aria-hidden="true"></span><span>Track<br>your visits</span></div>
          </div>
        </div>
        <img class="hospital-image" src="/hospital-building.png" alt="Xiamen Medical Center hospital building">
      </section>
      <section class="auth-main auth-main-plain">
        <div class="auth-card auth-card-plain">
          <div class="medical-cross" aria-hidden="true"></div>
          <div class="auth-welcome-label">${isSetup ? "FIRST-TIME SETUP" : isRegister ? "PATIENT REGISTRATION" : "WELCOME TO XIAMEN MEDICAL CENTER"}</div>
          <h2>${title}</h2>
          <p class="subcopy">${subtitle}</p>
          <div class="role-switch" role="tablist" aria-label="Choose account type">
            ${["patient", "doctor", "admin"].map((role) => `<button type="button" role="tab" aria-selected="${selectedRole === role}" tabindex="${selectedRole === role ? "0" : "-1"}" class="${selectedRole === role ? "active" : ""}" data-role="${role}">${role[0].toUpperCase()}${role.slice(1)}</button>`).join("")}
          </div>
          <form id="auth-form" class="form-stack" novalidate>
            ${mode === "setup" || mode === "register" ? `
              <div class="field"><label for="name">${mode === "setup" ? "Administrator name" : "Full name"}</label><input id="name" name="name" autocomplete="name" placeholder="Your full name" required maxlength="80"></div>
              ${mode === "setup" ? `<div class="field"><label for="department">Department</label><input id="department" name="department" placeholder="Clinic administration" required maxlength="80"></div>` : `
                <div class="form-row"><div class="field"><label for="age">Age</label><input id="age" name="age" type="number" min="1" max="100" inputmode="numeric" placeholder="e.g. 28" required></div>
                <div class="field"><label for="ailment">Reason for visit</label><input id="ailment" name="ailment" placeholder="What can we help with?" required maxlength="240"></div></div>`}
            ` : ""}
            <div class="field"><label for="username">Username</label><input id="username" name="username" autocomplete="username" placeholder="${mode === "login" ? "Letters and numbers" : "Choose a username"}" required minlength="${mode === "login" ? 1 : 3}" maxlength="30"></div>
            <div class="field"><label for="password">Password</label><input id="password" name="password" type="password" autocomplete="${mode === "login" ? "current-password" : "new-password"}" placeholder="${mode === "login" ? "Enter your password" : "At least 8 characters"}" required minlength="${mode === "login" ? 1 : 8}" maxlength="128"></div>
            <button class="primary-button auth-submit" type="submit">${isSetup ? "Create administrator account" : isRegister ? "Create patient account" : "Sign in"}</button>
          </form>
          ${selectedRole === "patient" && !isSetup ? `<div class="form-foot"><span>${mode === "register" ? "Already have an account?" : "New to Xiamen Medical Center?"}</span> <button class="text-button" type="button" id="toggle-mode">${mode === "register" ? "Sign in" : "Create patient account"}</button></div>` : ""}
          ${isSetup ? `<div class="setup-note first-time-setup">Your first administrator receives full access. Additional admin accounts can be created later by an administrator.</div>` : ""}
          ${mode === "login" && !isSetup ? `<div class="setup-note patient-info">${escapeHtml(loginNote)}${selectedRole !== "patient" ? " Contact your administrator if you need access." : ""}</div>` : ""}
        </div>
      </section>
    </div>`;

  app.querySelectorAll("[data-role]").forEach((button) => button.addEventListener("click", () => {
    selectedRole = button.dataset.role;
    renderAuth(setupRequired, selectedRole === "patient" ? formMode : "login");
  }));
  app.querySelectorAll("[data-role]").forEach((button, index, buttons) => button.addEventListener("keydown", (event) => {
    if (!["ArrowLeft", "ArrowRight"].includes(event.key)) return;
    event.preventDefault();
    const direction = event.key === "ArrowRight" ? 1 : -1;
    buttons[(index + direction + buttons.length) % buttons.length].click();
    app.querySelector(`[data-role="${selectedRole}"]`)?.focus();
  }));
  const toggle = app.querySelector("#toggle-mode");
  if (toggle) toggle.addEventListener("click", () => renderAuth(setupRequired, mode === "register" ? "login" : "register"));
  app.querySelector("#auth-form").addEventListener("submit", (event) => {
    if (!event.currentTarget.reportValidity()) {
      event.preventDefault();
      return;
    }
    submitAuth(event);
  });
}

async function submitAuth(event) {
  event.preventDefault();
  const form = Object.fromEntries(new FormData(event.currentTarget));
  const setupRequired = document.querySelector(".first-time-setup") !== null;
  const isRegister = document.querySelector("#ailment") !== null;
  try {
    const result = await api(setupRequired ? "/api/setup" : isRegister ? "/api/register" : "/api/login", {
      method: "POST",
      body: JSON.stringify({ ...form, role: selectedRole }),
    });
    currentUser = result.user;
    renderDashboard();
  } catch (error) {
    showToast(error.message, true);
  }
}

function header(user) {
  return `<header class="topbar">
    ${brand()}
    <div class="topbar-actions">
      <div class="user-chip"><span class="avatar">${escapeHtml(user.name.slice(0, 1).toUpperCase())}</span><span>${escapeHtml(user.name)}<span class="mobile-label"> · ${escapeHtml(user.role)}</span></span></div>
      <button class="quiet-button" type="button" id="logout-button">Sign out</button>
    </div>
  </header>`;
}

function dateParts(date) {
  const parsed = new Date(`${date}T12:00:00`);
  return {
    day: Number.isNaN(parsed.getTime()) ? "—" : parsed.toLocaleDateString(undefined, { day: "2-digit" }),
    month: Number.isNaN(parsed.getTime()) ? "" : parsed.toLocaleDateString(undefined, { month: "short" }),
  };
}

function statusBadge(status) {
  return `<span class="status status-${status.toLowerCase()}">${escapeHtml(status)}</span>`;
}

function appointmentRow(appointment, role) {
  const date = dateParts(appointment.date);
  const title = role === "patient" ? `Dr. ${appointment.doctorName}` : appointment.patientName;
  const detail = role === "patient"
    ? `${appointment.specialty} · ${appointment.time} ${appointment.timeZone || ""}`
    : `${appointment.date} · ${appointment.time} ${appointment.timeZone || ""}${role === "admin" ? ` · ${appointment.patientName}` : ""}`;
  let actions = "";
  if (role === "patient" && !["Cancelled", "Rejected"].includes(appointment.status)) {
    actions = `<div class="row-actions"><button class="danger-button" data-cancel="${escapeHtml(appointment.id)}" type="button">Cancel visit</button></div>`;
  }
  if (role === "doctor" && appointment.status === "Pending") {
    actions = `<div class="row-actions"><button class="secondary-button" data-status="${escapeHtml(appointment.id)}" data-next="Confirmed" type="button">Confirm</button><button class="danger-button" data-status="${escapeHtml(appointment.id)}" data-next="Rejected" type="button">Decline</button></div>`;
  }
  return `<article class="appointment-row">
    <div class="date-block"><b>${date.day}</b><span>${date.month}</span></div>
    <div><div class="appointment-title">${escapeHtml(title)}</div><div class="appointment-meta">${escapeHtml(detail)}</div>${actions}</div>
    ${statusBadge(appointment.status)}
  </article>`;
}

function emptyState(message) {
  return `<div class="empty-state"><div class="empty-icon">+</div>${escapeHtml(message)}</div>`;
}

function panel(title, content, caption = "") {
  return `<section class="panel"><div class="panel-heading"><div><h2>${title}</h2>${caption ? `<p class="panel-caption">${caption}</p>` : ""}</div></div>${content}</section>`;
}

function statCard(label, value, detail) {
  return `<div class="stat-card"><div class="stat-label">${label}</div><div class="stat-number">${value}</div><div class="stat-detail">${detail}</div></div>`;
}

function profilePanel(user) {
  const lines = user.role === "patient"
    ? [["Patient ID", user.id], ["Age", `${user.age} years`], ["Reason for visit", user.ailment]]
    : user.role === "doctor"
      ? [["Doctor ID", user.id], ["Specialty", user.specialty], ["Room", user.room]]
      : [["Admin ID", user.id], ["Department", user.department], ["Access level", `${user.accessLevel} of 3`]];
  return panel("Your profile", `<div class="profile-card">${lines.map(([label, value]) => `<div class="profile-line"><span>${label}</span><b>${escapeHtml(value)}</b></div>`).join("")}</div>`);
}

function renderTimeWheel(name, label, values, selected) {
  const selectedValue = String(selected);
  const options = values.map((value) => {
    const optionValue = String(value);
    const isSelected = optionValue === selectedValue;
    return `<button id="time-wheel-option-${name}-${escapeHtml(optionValue)}" class="time-wheel-option" type="button" role="option" tabindex="-1" data-wheel-value="${escapeHtml(optionValue)}" aria-selected="${isSelected}">${escapeHtml(optionValue)}</button>`;
  }).join("");
  return `<div class="time-wheel-column">
    <span class="time-part-label">${label}</span>
    <div class="time-wheel-shell">
      <div id="time-wheel-${name}" class="time-wheel" data-time-wheel="${name}" role="listbox" aria-label="${label}" aria-activedescendant="time-wheel-option-${name}-${escapeHtml(selectedValue)}" tabindex="0">
        <div class="time-wheel-spacer" aria-hidden="true"></div>
        ${options}
        <div class="time-wheel-spacer" aria-hidden="true"></div>
      </div>
    </div>
  </div>`;
}

function bookingPanel(doctors) {
  const tomorrow = new Date();
  tomorrow.setDate(tomorrow.getDate() + 1);
  const minDate = `${tomorrow.getFullYear()}-${String(tomorrow.getMonth() + 1).padStart(2, "0")}-${String(tomorrow.getDate()).padStart(2, "0")}`;
  const timeZone = Intl.DateTimeFormat().resolvedOptions().timeZone || "Asia/Kuala_Lumpur";
  return panel("Book a visit", `
    <form id="booking-form" class="booking-form">
      <input type="hidden" name="timeZone" value="${escapeHtml(timeZone)}">
      <div class="field"><label for="doctorId">Choose a doctor</label><select id="doctorId" name="doctorId" required><option value="">Select a doctor</option>${doctors.map((doctor) => `<option value="${escapeHtml(doctor.id)}">${escapeHtml(doctor.name)} · ${escapeHtml(doctor.specialty)}</option>`).join("")}</select></div>
      <div class="field"><label for="appointment-date">Date</label>
        <div class="date-picker-control">
          <input id="appointment-date" type="date" name="date" min="${minDate}" required>
          <button class="picker-button" id="open-calendar" type="button" aria-label="Open appointment calendar" title="Open calendar">
            <svg viewBox="0 0 24 24" aria-hidden="true"><rect x="3.5" y="5" width="17" height="16" rx="2.5"></rect><path d="M7.5 3v4M16.5 3v4M4 9.5h16M8 13h.01M12 13h.01M16 13h.01M8 17h.01M12 17h.01"></path></svg>
          </button>
        </div>
      </div>
      <div class="field">
        <div id="appointment-time-label" class="field-label">Appointment time <span class="field-hint-inline">Scroll to choose</span></div>
        <input id="appointment-hour" name="hour" type="hidden" value="9">
        <input id="appointment-minute" name="minute" type="hidden" value="00">
        <input id="appointment-period" name="period" type="hidden" value="AM">
        <div class="time-picker-card" role="group" aria-labelledby="appointment-time-label">
          <div class="time-picker-readout">
            <span class="time-picker-caption">Selected time</span>
            <output id="appointment-time-preview" aria-live="polite">9:00 AM</output>
          </div>
          <div class="time-wheels" aria-describedby="appointment-time-help">
            ${renderTimeWheel("hour", "Hour", Array.from({ length: 12 }, (_, index) => index + 1), "9")}
            ${renderTimeWheel("minute", "Minute", Array.from({ length: 60 }, (_, index) => String(index).padStart(2, "0")), "00")}
            ${renderTimeWheel("period", "AM / PM", ["AM", "PM"], "AM")}
          </div>
          <p id="appointment-time-help" class="time-picker-help">Swipe or scroll each column. You can also use the arrow keys.</p>
        </div>
      </div>
      ${doctors.length ? `<button class="primary-button" type="submit">Request appointment</button>` : `<p class="panel-caption">No doctors are available yet. Please check back later.</p>`}
    </form>`);
}

function patientTable(patients, user) {
  if (!patients.length) return emptyState("No patient accounts yet.");
  return `<div class="table-wrap"><table><thead><tr><th>Patient</th><th>Age</th><th>Reason for visit</th>${user.accessLevel >= 2 ? "<th>Actions</th>" : ""}</tr></thead><tbody>
    ${patients.map((patient) => `<tr><td>${escapeHtml(patient.name)}<br><span class="panel-caption">${escapeHtml(patient.id)}</span></td><td>${escapeHtml(patient.age)}</td><td>${escapeHtml(patient.ailment)}</td>${user.accessLevel >= 2 ? `<td><button class="quiet-button" data-edit="${escapeHtml(patient.id)}" data-age="${escapeHtml(patient.age)}" data-ailment="${escapeHtml(patient.ailment)}">Edit</button> ${user.accessLevel >= 3 ? `<button class="danger-button" data-delete="${escapeHtml(patient.id)}">Delete</button>` : ""}</td>` : ""}</tr>`).join("")}
  </tbody></table></div>`;
}

function doctorsTable(doctors) {
  if (!doctors.length) return emptyState("Add a doctor to open appointment booking.");
  return `<div class="table-wrap"><table><thead><tr><th>Doctor</th><th>Specialty</th><th>Room</th></tr></thead><tbody>${doctors.map((doctor) => `<tr><td>${escapeHtml(doctor.name)}<br><span class="panel-caption">${escapeHtml(doctor.id)}</span></td><td>${escapeHtml(doctor.specialty)}</td><td>${escapeHtml(doctor.room)}</td></tr>`).join("")}</tbody></table></div>`;
}

function adminForms(user) {
  const exportsPanel = `<section class="panel"><div class="panel-heading"><div><h2>Spreadsheet exports</h2><p class="panel-caption">Download a CSV file for each record type.</p></div></div>
    <div class="export-links">
      <a class="secondary-button" href="/api/admin/export/patients">Patients.csv</a>
      <a class="secondary-button" href="/api/admin/export/doctors">Doctors.csv</a>
      <a class="secondary-button" href="/api/admin/export/appointments">Appointments.csv</a>
      <a class="secondary-button" href="/api/admin/export/admins">Admins.csv</a>
    </div>
  </section>`;
  const doctorForm = user.accessLevel >= 2 ? `<section class="panel"><div class="panel-heading"><div><h2>Add a doctor</h2><p class="panel-caption">Create a doctor portal account.</p></div></div>
    <form id="doctor-form" class="form-stack">
      <div class="field"><label for="doctor-name">Full name</label><input id="doctor-name" name="name" required maxlength="80"></div>
      <div class="field"><label for="doctor-specialty">Specialty</label><input id="doctor-specialty" name="specialty" placeholder="Family medicine" required maxlength="80"></div>
      <div class="field"><label for="doctor-room">Room</label><input id="doctor-room" name="room" placeholder="e.g. 204" required maxlength="20"></div>
      <div class="field"><label for="doctor-username">Username</label><input id="doctor-username" name="username" required minlength="3" maxlength="30"></div>
      <div class="field"><label for="doctor-password">Temporary password</label><input id="doctor-password" name="password" type="password" required minlength="8" maxlength="128"></div>
      <button class="primary-button" type="submit">Add doctor</button>
    </form>
  </section>` : "";
  const adminForm = user.accessLevel >= 3 ? `<section class="panel"><div class="panel-heading"><div><h2>Add an admin</h2><p class="panel-caption">Assign an access level from 1 to 3.</p></div></div>
    <form id="admin-form" class="form-stack">
      <div class="field"><label for="admin-name">Full name</label><input id="admin-name" name="name" required maxlength="80"></div>
      <div class="field"><label for="admin-department">Department</label><input id="admin-department" name="department" required maxlength="80"></div>
      <div class="field"><label for="admin-access">Access level</label><select id="admin-access" name="accessLevel"><option value="1">1 · View records</option><option value="2">2 · Manage records</option><option value="3">3 · Full access</option></select></div>
      <div class="field"><label for="admin-username">Username</label><input id="admin-username" name="username" required minlength="3" maxlength="30"></div>
      <div class="field"><label for="admin-password">Temporary password</label><input id="admin-password" name="password" type="password" required minlength="8" maxlength="128"></div>
      <button class="secondary-button" type="submit">Create admin account</button>
    </form>
  </section>` : "";
  const patientForm = user.accessLevel >= 2 ? `<section class="panel"><div class="panel-heading"><div><h2>Register a patient</h2><p class="panel-caption">Create a patient portal account.</p></div></div>
    <form id="patient-form" class="form-stack">
      <div class="field"><label for="patient-name">Full name</label><input id="patient-name" name="name" required maxlength="80"></div>
      <div class="field"><label for="patient-age">Age</label><input id="patient-age" name="age" type="number" min="1" max="100" required></div>
      <div class="field"><label for="patient-ailment">Reason for visit</label><input id="patient-ailment" name="ailment" required maxlength="240"></div>
      <div class="field"><label for="patient-username">Username</label><input id="patient-username" name="username" required minlength="3" maxlength="30"></div>
      <div class="field"><label for="patient-password">Temporary password</label><input id="patient-password" name="password" type="password" required minlength="8" maxlength="128"></div>
      <button class="primary-button" type="submit">Register patient</button>
    </form>
  </section>` : "";
  return `${exportsPanel}${patientForm}${doctorForm}${adminForm}`;
}

function renderDashboard() {
  const user = currentUser;
  if (!user) return;
  app.innerHTML = `<div class="dashboard">${header(user)}<main class="main-content" id="dashboard-content"><div class="loading-card">Loading your dashboard…</div></main></div>`;
  app.querySelector("#logout-button").addEventListener("click", logout);
  if (user.role === "patient") loadPatientDashboard(user);
  else if (user.role === "doctor") loadDoctorDashboard(user);
  else loadAdminDashboard(user);
}

async function loadPatientDashboard(user) {
  const main = document.querySelector("#dashboard-content");
  try {
    const [appointments, doctors] = await Promise.all([api("/api/appointments"), api("/api/doctors")]);
    const active = appointments.filter((appointment) => !["Cancelled", "Rejected"].includes(appointment.status));
    const upcoming = appointments.filter((appointment) => new Date(`${appointment.date}T${appointment.time}`) >= new Date());
    main.innerHTML = `
      <div class="page-heading"><div><div class="eyebrow">Patient portal</div><h1>Good to see you, ${escapeHtml(user.name.split(" ")[0])}.</h1><p>Your care plan and upcoming visits, all in one place.</p></div></div>
      <div class="stats-grid">${statCard("UPCOMING VISITS", upcoming.length, "Your next steps in care")}${statCard("ACTIVE REQUESTS", active.length, "Including pending confirmations")}${statCard("CARE TEAM", doctors.length, "Doctors available to book")}</div>
      <div class="content-grid"><div>${panel("Your appointments", appointments.length ? `<div class="appointment-list">${appointments.map((entry) => appointmentRow(entry, "patient")).join("")}</div>` : emptyState("No appointments yet. Book your first visit when you’re ready."), "Your visits and appointment requests")}</div>
        <div>${bookingPanel(doctors)}${profilePanel(user)}</div>
      </div>`;
    document.querySelector("#booking-form")?.addEventListener("submit", bookAppointment);
    bindBookingPickers();
    bindAppointmentActions();
  } catch (error) { showToast(error.message, true); }
}

async function loadDoctorDashboard(user) {
  const main = document.querySelector("#dashboard-content");
  try {
    const appointments = await api("/api/appointments");
    const pending = appointments.filter((entry) => entry.status === "Pending");
    const today = new Date().toISOString().slice(0, 10);
    const todayAppointments = appointments.filter((entry) => entry.date === today && entry.status !== "Cancelled");
    main.innerHTML = `
      <div class="page-heading"><div><div class="eyebrow">Doctor portal</div><h1>Welcome, Dr. ${escapeHtml(user.name)}.</h1><p>Review your clinic schedule and patient requests.</p></div></div>
      <div class="stats-grid">${statCard("TODAY", todayAppointments.length, "Visits on your schedule")}${statCard("NEEDS REVIEW", pending.length, "Pending appointment requests")}${statCard("TOTAL VISITS", appointments.length, "Appointments assigned to you")}</div>
      <div class="content-grid"><div>${panel("Your appointments", appointments.length ? `<div class="appointment-list">${appointments.map((entry) => appointmentRow(entry, "doctor")).join("")}</div>` : emptyState("You have no assigned appointments yet."), "Confirm or decline pending requests")}</div><div>${profilePanel(user)}</div></div>`;
    bindAppointmentActions();
  } catch (error) { showToast(error.message, true); }
}

async function loadAdminDashboard(user) {
  const main = document.querySelector("#dashboard-content");
  try {
    const [summary, patients, doctors, appointments] = await Promise.all([
      api("/api/session"), api("/api/patients"), api("/api/admin/doctors"), api("/api/appointments"),
    ]);
    main.innerHTML = `
      <div class="page-heading"><div><div class="eyebrow">Administration · Level ${user.accessLevel}</div><h1>Clinic overview</h1><p>Manage clinic records and appointment activity.</p></div></div>
      <div class="stats-grid">${statCard("PATIENTS", summary.counts.patients, "Registered patient accounts")}${statCard("DOCTORS", summary.counts.doctors, "Care team accounts")}${statCard("APPOINTMENTS", summary.counts.appointments, "All appointment records")}</div>
      <div class="admin-layout">
        <div class="admin-main">
          ${panel("Patient records", patientTable(patients, user), "Personal health information — handle with care")}
          <div class="panel"><div class="panel-heading"><div><h2>Appointment schedule</h2><p class="panel-caption">All clinic appointments</p></div></div>${appointments.length ? `<div class="appointment-list">${appointments.map((entry) => appointmentRow(entry, "admin")).join("")}</div>` : emptyState("No appointments have been booked.")}</div>
          ${panel("Care team", doctorsTable(doctors), "Doctors currently available for booking")}
        </div>
        <aside class="admin-side">${adminForms(user)}${profilePanel(user)}</aside>
      </div>`;
    document.querySelector("#doctor-form")?.addEventListener("submit", createDoctor);
    document.querySelector("#admin-form")?.addEventListener("submit", createAdmin);
    document.querySelector("#patient-form")?.addEventListener("submit", createPatient);
    bindPatientActions(user);
  } catch (error) { showToast(error.message, true); }
}

async function bookAppointment(event) {
  event.preventDefault();
  const formElement = event.currentTarget;
  if (!formElement.reportValidity()) return;
  const form = Object.fromEntries(new FormData(formElement));
  const hour = Number(form.hour);
  const minute = Number(form.minute);
  if (!Number.isInteger(hour) || hour < 1 || hour > 12 || !Number.isInteger(minute) || minute < 0 || minute > 59 || !["AM", "PM"].includes(form.period)) {
    showToast("Please choose a valid appointment time.", true);
    return;
  }
  const hour24 = (hour % 12) + (form.period === "PM" ? 12 : 0);
  form.time = `${String(hour24).padStart(2, "0")}:${String(minute).padStart(2, "0")}`;
  delete form.hour;
  delete form.minute;
  delete form.period;
  try {
    await api("/api/appointments", { method: "POST", body: JSON.stringify(form) });
    showToast("Appointment request sent.");
    loadPatientDashboard(currentUser);
  } catch (error) { showToast(error.message, true); }
}

function bindBookingPickers() {
  const dateInput = document.querySelector("#appointment-date");
  document.querySelector("#open-calendar")?.addEventListener("click", () => {
    try {
      if (typeof dateInput.showPicker === "function") dateInput.showPicker();
      else {
        dateInput.focus();
        dateInput.click();
      }
    } catch {
      dateInput.focus();
      dateInput.click();
    }
  });

  const rowHeight = 40;
  const scrollBehavior = window.matchMedia?.("(prefers-reduced-motion: reduce)").matches ? "auto" : "smooth";
  document.querySelectorAll("[data-time-wheel]").forEach((wheel) => {
    const name = wheel.dataset.timeWheel;
    const hiddenInput = document.getElementById(`appointment-${name}`);
    const options = Array.from(wheel.querySelectorAll(".time-wheel-option"));
    if (!hiddenInput || !options.length) return;

    const updatePreview = () => {
      const hour = Number(document.querySelector("#appointment-hour").value);
      const minute = String(document.querySelector("#appointment-minute").value).padStart(2, "0");
      const period = document.querySelector("#appointment-period").value;
      document.querySelector("#appointment-time-preview").textContent = `${hour}:${minute} ${period}`;
    };
    const setSelectedOption = (index, shouldScroll = false) => {
      const nextIndex = Math.max(0, Math.min(options.length - 1, index));
      const selected = options[nextIndex];
      options.forEach((option, optionIndex) => {
        option.setAttribute("aria-selected", String(optionIndex === nextIndex));
      });
      wheel.setAttribute("aria-activedescendant", selected.id);
      hiddenInput.value = selected.dataset.wheelValue;
      updatePreview();
      if (shouldScroll) wheel.scrollTo({ top: nextIndex * rowHeight, behavior: scrollBehavior });
    };

    const initialIndex = Math.max(0, options.findIndex((option) => option.dataset.wheelValue === hiddenInput.value));
    wheel.scrollTop = initialIndex * rowHeight;
    setSelectedOption(initialIndex);

    let scrollTimer;
    wheel.addEventListener("scroll", () => {
      clearTimeout(scrollTimer);
      scrollTimer = setTimeout(() => {
        setSelectedOption(Math.round(wheel.scrollTop / rowHeight));
      }, 110);
    });
    wheel.addEventListener("click", (event) => {
      const option = event.target.closest(".time-wheel-option");
      if (option && wheel.contains(option)) setSelectedOption(options.indexOf(option), true);
    });
    wheel.addEventListener("keydown", (event) => {
      const currentIndex = options.findIndex((option) => option.getAttribute("aria-selected") === "true");
      let nextIndex = currentIndex;
      if (event.key === "ArrowUp") nextIndex -= 1;
      else if (event.key === "ArrowDown") nextIndex += 1;
      else if (event.key === "PageUp") nextIndex -= 5;
      else if (event.key === "PageDown") nextIndex += 5;
      else if (event.key === "Home") nextIndex = 0;
      else if (event.key === "End") nextIndex = options.length - 1;
      else return;
      event.preventDefault();
      setSelectedOption(nextIndex, true);
    });
  });
}

function bindAppointmentActions() {
  document.querySelectorAll("[data-cancel]").forEach((button) => button.addEventListener("click", async () => {
    if (!confirm("Cancel this appointment request?")) return;
    try {
      await api(`/api/appointments/${encodeURIComponent(button.dataset.cancel)}`, { method: "PATCH", body: JSON.stringify({ status: "Cancelled" }) });
      showToast("Appointment cancelled.");
      renderDashboard();
    } catch (error) { showToast(error.message, true); }
  }));
  document.querySelectorAll("[data-status]").forEach((button) => button.addEventListener("click", async () => {
    try {
      await api(`/api/appointments/${encodeURIComponent(button.dataset.status)}`, { method: "PATCH", body: JSON.stringify({ status: button.dataset.next }) });
      showToast(`Appointment ${button.dataset.next.toLowerCase()}.`);
      renderDashboard();
    } catch (error) { showToast(error.message, true); }
  }));
}

async function createDoctor(event) {
  event.preventDefault();
  const form = Object.fromEntries(new FormData(event.currentTarget));
  try {
    await api("/api/admin/doctors", { method: "POST", body: JSON.stringify(form) });
    showToast("Doctor account created.");
    renderDashboard();
  } catch (error) { showToast(error.message, true); }
}

async function createPatient(event) {
  event.preventDefault();
  const form = Object.fromEntries(new FormData(event.currentTarget));
  try {
    await api("/api/admin/patients", { method: "POST", body: JSON.stringify(form) });
    showToast("Patient account created.");
    renderDashboard();
  } catch (error) { showToast(error.message, true); }
}

async function createAdmin(event) {
  event.preventDefault();
  const form = Object.fromEntries(new FormData(event.currentTarget));
  try {
    await api("/api/admin/admins", { method: "POST", body: JSON.stringify(form) });
    showToast("Admin account created.");
    renderDashboard();
  } catch (error) { showToast(error.message, true); }
}

function bindPatientActions(user) {
  document.querySelectorAll("[data-edit]").forEach((button) => button.addEventListener("click", async () => {
    const age = prompt("Update patient age:", button.dataset.age);
    if (age === null) return;
    const ailment = prompt("Update reason for visit:", button.dataset.ailment);
    if (ailment === null) return;
    try {
      await api(`/api/admin/patients/${encodeURIComponent(button.dataset.edit)}`, { method: "PATCH", body: JSON.stringify({ age, ailment }) });
      showToast("Patient record updated.");
      renderDashboard();
    } catch (error) { showToast(error.message, true); }
  }));
  document.querySelectorAll("[data-delete]").forEach((button) => button.addEventListener("click", async () => {
    if (!confirm("Delete this patient and their appointment records? This cannot be undone.")) return;
    try {
      await api(`/api/admin/patients/${encodeURIComponent(button.dataset.delete)}`, { method: "DELETE" });
      showToast("Patient record deleted.");
      renderDashboard();
    } catch (error) { showToast(error.message, true); }
  }));
}

async function logout() {
  try { await api("/api/logout", { method: "POST", body: "{}" }); } catch { /* sign out locally even if the session expired */ }
  currentUser = null;
  selectedRole = "patient";
  renderAuth(false);
}

async function boot() {
  try {
    const session = await api("/api/session");
    if (session.user) {
      currentUser = session.user;
      renderDashboard();
    } else {
      if (session.setupRequired) selectedRole = "admin";
      renderAuth(session.setupRequired);
    }
  } catch (error) {
    app.innerHTML = `<section class="loading-card"><img class="loading-seal" src="/xiamen-seal.jpg" alt=""><h2>Xiamen Medical Center is unavailable</h2><p>${escapeHtml(error.message)}</p></section>`;
  }
}

boot();
