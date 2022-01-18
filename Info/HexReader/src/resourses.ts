import { TFlashSegmen } from "./hextypes";

export function getResourses(resourses: any):Array<TFlashSegmen> {
  const res: Array<TFlashSegmen> = [];
  if (!isResourcesValid(resourses)) return res;
  return res;
}

function isResourcesValid(resourses: any): boolean {
  if (resourses === undefined) return false;
  if (!('start' in resourses)) return false;
  if (!('resourses' in resourses)) return false;
  return true;
}