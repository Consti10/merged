export class ErrorMsg {
  error: number;
  msg: string;

  constructor(error: number, msg: string) {
    this.error = error;
    this.msg = msg;
  }
}
