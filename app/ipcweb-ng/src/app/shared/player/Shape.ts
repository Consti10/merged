export interface Point {
  x: number;
  y: number;
}

export class Shape {
  fill: boolean;
  color: string;
  strokeWidth: number;
  pointArray: Array<Point>;

  constructor() {
    this.pointArray = [];
    this.fill = false;
    this.color = "#00001F";
    this.strokeWidth = 3;
  }

  get Points(): Array<Point> {
    return this.pointArray;
  }

  set Points(points: Array<Point>) {
    for (const p of points) {
      this.AddPoint(p);
    }
  }

  AddPoint(point: Point): void {
    this.pointArray.push(point);
  }

}

export class Rectangle extends Shape {
  x: number;
  y: number;
  width: number;
  height: number;

  get x1(): number {
    return this.x + this.width;
  }

  set x1(x: number) {
    this.width = x - this.x;
    this.pointArray[1].x = this.x + this.width;
  }

  get y1(): number {
    return this.y + this.height;
  }

  set y1(y: number) {
    this.height = y - this.y;
    this.pointArray[1].y = this.y + this.height;
  }


  constructor(x: number, y: number, width: number, height: number) {
    super();
    this.x = x;
    this.y = y;
    this.width = width;
    this.height = height;

    const p1: Point = { x:this.x, y:this.y };
    const p2: Point = { x:this.x + this.width, y:this.y + this.height };
    this.pointArray.push(p1);
    this.pointArray.push(p2);
  }

}

