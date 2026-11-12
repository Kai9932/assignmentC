const fs = require("node:fs/promises");
const path = require("node:path");
const { ReplitConnectors } = require("@replit/connectors-sdk");
const sheetsConfig = require("./google-sheets.config.json");

const DATA_DIR = path.join(__dirname, "data");
const tables = {
  patients: ["id", "name", "ailment", "age", "username", "passwordHash", "createdAt"],
  doctors: ["id", "name", "specialty", "room", "username", "passwordHash"],
  admins: ["username", "passwordHash", "id", "name", "department", "accessLevel"],
  appointments: ["id", "patientId", "doctorId", "date", "time", "status", "timeZone", "createdAt"],
};
const connectors = new ReplitConnectors();
const tableCache = Object.create(null);
const rowCounts = Object.create(null);

function toStringCell(value) {
  return String(value ?? "");
}

function parseRows(name, values) {
  const expected = tables[name];
  const header = (values[0] || []).map(toStringCell);
  if (header.join(",") !== expected.join(",")) {
    throw new Error(`Google Sheet for ${name} has unexpected columns. Expected: ${expected.join(",")}`);
  }
  return values.slice(1)
    .filter((row) => row.some((cell) => toStringCell(cell) !== ""))
    .map((row) => Object.fromEntries(
      expected.map((key, index) => [key, toStringCell(row[index])]),
    ));
}

function csvCell(value) {
  const text = toStringCell(value);
  return /[",\r\n]/.test(text) ? `"${text.replaceAll('"', '""')}"` : text;
}

async function writeLocalBackup(name, records) {
  await fs.mkdir(DATA_DIR, { recursive: true });
  const headers = tables[name];
  const content = [
    headers.join(","),
    ...records.map((record) => headers.map((key) => csvCell(record[key])).join(",")),
  ].join("\n") + "\n";
  const target = path.join(DATA_DIR, `${name}.csv`);
  const temporary = `${target}.${process.pid}.tmp`;
  await fs.writeFile(temporary, content, { mode: 0o600 });
  await fs.rename(temporary, target);
}

async function sheetsRequest(endpoint, options = {}) {
  const response = await connectors.proxy("google-sheet", endpoint, options);
  const text = await response.text();
  let result = {};
  try {
    result = text ? JSON.parse(text) : {};
  } catch {
    result = { error: { message: text.slice(0, 300) } };
  }
  if (!response.ok) {
    throw new Error(`Google Sheets request failed (${response.status}): ${result.error?.message || "Unknown provider error."}`);
  }
  return result;
}

function rangeFor(name) {
  const sheetTitle = sheetsConfig.tables[name].sheetTitle.replaceAll("'", "''");
  return `'${sheetTitle}'!A1:Z`;
}

async function initializeStore() {
  await fs.mkdir(DATA_DIR, { recursive: true });
  for (const name of Object.keys(tables)) {
    const config = sheetsConfig.tables[name];
    if (!config?.spreadsheetId || !Number.isInteger(config.sheetId) || !config.sheetTitle) {
      throw new Error(`Google Sheets configuration is missing for ${name}.`);
    }
    const range = encodeURIComponent(rangeFor(name));
    const result = await sheetsRequest(
      `/v4/spreadsheets/${encodeURIComponent(config.spreadsheetId)}/values/${range}`,
    );
    const values = result.values || [];
    if (!values.length) throw new Error(`Google Sheet for ${name} is empty; expected a header row.`);
    const records = parseRows(name, values);
    tableCache[name] = records;
    rowCounts[name] = values.length;
    try {
      await writeLocalBackup(name, records);
    } catch (error) {
      console.warn(`Local ${name}.csv backup could not be refreshed: ${error.message}`);
    }
  }
  console.log(`Loaded ${Object.keys(tables).length} tables from Google Sheets.`);
}

function readTable(name) {
  if (!Object.hasOwn(tables, name)) throw new Error(`Unknown data table: ${name}`);
  if (!tableCache[name]) throw new Error(`Google Sheets table ${name} has not been initialized.`);
  return tableCache[name].map((record) => ({ ...record }));
}

async function writeTable(name, records) {
  const headers = tables[name];
  const config = sheetsConfig.tables[name];
  if (!headers || !config) throw new Error(`Unknown data table: ${name}`);
  const values = [
    headers,
    ...records.map((record) => headers.map((key) => toStringCell(record[key]))),
  ];
  const endRowIndex = Math.max(rowCounts[name] || 1, values.length);
  const requestBody = {
    requests: [{
      updateCells: {
        range: {
          sheetId: config.sheetId,
          startRowIndex: 0,
          endRowIndex,
          startColumnIndex: 0,
          endColumnIndex: headers.length,
        },
        rows: values.map((row) => ({
          values: row.map((value) => ({ userEnteredValue: { stringValue: value } })),
        })),
        fields: "userEnteredValue",
      },
    }],
  };
  await sheetsRequest(
    `/v4/spreadsheets/${encodeURIComponent(config.spreadsheetId)}:batchUpdate`,
    { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(requestBody) },
  );
  const nextRecords = records.map((record) => ({ ...record }));
  tableCache[name] = nextRecords;
  rowCounts[name] = values.length;
  try {
    await writeLocalBackup(name, nextRecords);
  } catch (error) {
    console.warn(`Local ${name}.csv backup could not be refreshed: ${error.message}`);
  }
}

module.exports = { tables, initializeStore, readTable, writeTable };
