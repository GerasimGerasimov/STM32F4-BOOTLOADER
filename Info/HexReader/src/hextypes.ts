export interface TFlashSegmenDescription {
  start: string;
  size: number | string ;
}

export interface TFlashSegmen extends TFlashSegmenDescription {
  code: Array<number>;
}