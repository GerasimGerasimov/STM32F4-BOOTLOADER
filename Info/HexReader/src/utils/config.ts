import fs = require ("fs");

export function getSettingsFormConfigFile (): any {
  let filename = process.argv[2];
  try {
    const s: string =  fs.readFileSync(filename, 'utf8');
    return JSON.parse(s);
  } catch (e) {
    console.log('Reading config-file failed', e);
  }
}