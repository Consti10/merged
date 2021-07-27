import { ElementRef } from '@angular/core';
import { WebGLProgramInfo } from './WebGLProgramInfo';

class Texture {
  gl: WebGLRenderingContext;
  texture: WebGLTexture;
  constructor(gl: WebGLRenderingContext) {
    this.gl = gl;
    this.texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, this.texture);

    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);

    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
  }

  bind(n: number, program: WebGLProgram, name: string): void {
    this.gl.activeTexture([this.gl.TEXTURE0, this.gl.TEXTURE1, this.gl.TEXTURE2][n]);
    this.gl.bindTexture(this.gl.TEXTURE_2D, this.texture);
    this.gl.uniform1i(this.gl.getUniformLocation(program, name), n);
  }

  fill(width: number, height: number, data: Uint8Array): void {
    this.gl.bindTexture(this.gl.TEXTURE_2D, this.texture);
    this.gl.texImage2D(this.gl.TEXTURE_2D, 0, this.gl.LUMINANCE, width, height, 0, this.gl.LUMINANCE, this.gl.UNSIGNED_BYTE, data);
  }
}

export default class WebGLUtils {
  public gl: WebGLRenderingContext;

  private canvas: ElementRef<HTMLCanvasElement>;
  private programInfo: WebGLProgramInfo;

  private yTexture: Texture;
  private uTexture: Texture;
  private vTexture: Texture;

  constructor(canvas: ElementRef<HTMLCanvasElement>) {
    this.canvas = canvas;

    this.gl = <WebGLRenderingContext>(this.canvas.nativeElement.getContext('webgl') ||
      this.canvas.nativeElement.getContext('experimental-webgl'));

    if (!this.gl) {
      alert('This browser does not support webgl');
      return null;
    }
  }

  initGL(isVideo: boolean): void {
    let gl = this.gl;
    let vsSource: string;
    let fsSource: string;

    if (isVideo) {
      vsSource = `
        attribute highp vec4 aVertexPosition;
        attribute vec2 aTextureCoord;
        varying highp vec2 vTextureCoord;

        void main() {
          gl_Position = aVertexPosition;
          vTextureCoord = aTextureCoord;
        }
      `;

      fsSource = `
        precision highp float;
        varying lowp vec2 vTextureCoord;
        uniform sampler2D YTexture;
        uniform sampler2D UTexture;
        uniform sampler2D VTexture;
        const mat4 YUV2RGB = mat4
        (
          1.1643828125, 0, 1.59602734375, -.87078515625,
          1.1643828125, -.39176171875, -.81296875, .52959375,
          1.1643828125, 2.017234375, 0, -1.081390625,
          0, 0, 0, 1
        );
        void main(void) {
          gl_FragColor = vec4( texture2D(YTexture, vTextureCoord).x, texture2D(UTexture, vTextureCoord).x, texture2D(VTexture, vTextureCoord).x, 1) * YUV2RGB;
        }
      `;
    } else {
      vsSource = `
        attribute vec4 aVertexPosition;

        uniform vec2 uResolutionMatrix;
        void main() {

          vec2 zeroToOne = aVertexPosition.xy / uResolutionMatrix;
          vec2 zeroToTwo = zeroToOne * 2.0;
          vec2 clipSpace = zeroToTwo - 1.0;

          gl_Position = vec4(clipSpace, aVertexPosition.zw);
        }
      `;
      fsSource = `
        void main() {
          gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
      `;
    }
    this.programInfo = new WebGLProgramInfo();
    this.programInfo.program = this.initShaderProgram(gl, vsSource, fsSource);
    this.programInfo.attribLocations = {
      vertexPosition: gl.getAttribLocation(this.programInfo.program, 'aVertexPosition'),
      textureCoord: gl.getAttribLocation(this.programInfo.program, 'aTextureCoord'),
    },
    this.programInfo.uniformLocations = {
      resolutionMatrix: gl.getUniformLocation(this.programInfo.program, 'uResolutionMatrix'),
      projectionMatrix: gl.getUniformLocation(this.programInfo.program, 'uProjectionMatrix'),
      modelViewMatrix: gl.getUniformLocation(this.programInfo.program, 'uModelViewMatrix'),
    };

    gl.useProgram(this.programInfo.program);

    if (isVideo) {
      let verticesBuffer = gl.createBuffer();
      gl.bindBuffer(gl.ARRAY_BUFFER, verticesBuffer);
      gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([1.0, 1.0, 0.0, -1.0, 1.0, 0.0, 1.0, -1.0, 0.0, -1.0, -1.0, 0.0]), gl.STATIC_DRAW);
      gl.vertexAttribPointer(this.programInfo.attribLocations.vertexPosition, 3, gl.FLOAT, false, 0, 0);
      let texCoordBuffer = gl.createBuffer();
      gl.bindBuffer(gl.ARRAY_BUFFER, texCoordBuffer);
      gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0]), gl.STATIC_DRAW);
      gl.vertexAttribPointer(this.programInfo.attribLocations.textureCoord, 2, gl.FLOAT, false, 0, 0);
      gl.enableVertexAttribArray(
        this.programInfo.attribLocations.vertexPosition
      );
      gl.enableVertexAttribArray(
        this.programInfo.attribLocations.textureCoord
      );

      this.yTexture = new Texture(gl);
      this.uTexture = new Texture(gl);
      this.vTexture = new Texture(gl);
      this.yTexture.bind(0, this.programInfo.program, "YTexture");
      this.uTexture.bind(1, this.programInfo.program, "UTexture");
      this.vTexture.bind(2, this.programInfo.program, "VTexture");
    } else {
      const positionBuffer: WebGLBuffer = gl.createBuffer();
      gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

      const positions = [
        gl.canvas.width / 2 + 60, gl.canvas.height / 2 + 60,
        gl.canvas.width / 2 - 60, gl.canvas.height / 2 + 60,
        gl.canvas.width / 2 + 60, gl.canvas.height / 2 - 60,
        gl.canvas.width / 2 - 60, gl.canvas.height / 2 - 60,
      ];

      gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
      gl.vertexAttribPointer(this.programInfo.attribLocations.vertexPosition, 2, gl.FLOAT, false, 0, 0);
      gl.enableVertexAttribArray(this.programInfo.attribLocations.vertexPosition);

      gl.uniform2f(this.programInfo.uniformLocations.resolutionMatrix,
        gl.canvas.width,
        gl.canvas.height);
    }
  }

  drawScene(gl: WebGLRenderingContext, programInfo: WebGLProgramInfo, buffers: any): void {
    gl.clearColor(0.0, 0.0, 0.0, 0.0);
    gl.colorMask(true, true, true, true);
    gl.clear(gl.COLOR_BUFFER_BIT);

    {
      const numComponents = 2;
      const type = gl.FLOAT;
      const normalize = false;
      const stride = 0;
      const offset = 0;

      gl.bindBuffer(gl.ARRAY_BUFFER, buffers.position);
      gl.vertexAttribPointer(
        programInfo.attribLocations.vertexPosition,
        numComponents,
        type,
        normalize,
        stride,
        offset
      );
      gl.enableVertexAttribArray(
        programInfo.attribLocations.vertexPosition
      );
    }
    gl.useProgram(programInfo.program);

    gl.uniform2f(programInfo.uniformLocations.resolutionMatrix,
      gl.canvas.width,
      gl.canvas.height);

    {
      const offset = 0;
      const vertexCount = 4;
      gl.drawArrays(gl.TRIANGLE_STRIP, offset, vertexCount);
    }
  }

  renderVideoFrame(videoFrame: Uint8Array, width: number, height: number, uOffset: number, vOffset: number): void {
    if (!this.gl) {
      console.error("Render frame failed due to WebGL not supported.");
      return;
    }

    let gl = this.gl;
    if (gl.canvas.width != this.canvas.nativeElement.clientWidth
      || gl.canvas.height != this.canvas.nativeElement.clientHeight) {
        gl.canvas.width = this.canvas.nativeElement.clientWidth;
        gl.canvas.height = this.canvas.nativeElement.height;
    }
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    gl.clearColor(0.0, 0.0, 0.0, 0.0);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);

    this.yTexture.fill(width, height, videoFrame.subarray(0, uOffset));
    this.uTexture.fill(width >> 1, height >> 1, videoFrame.subarray(uOffset, uOffset + vOffset));
    this.vTexture.fill(width >> 1, height >> 1, videoFrame.subarray(uOffset + vOffset, videoFrame.length));

    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
  };

  loadShader(gl: WebGLRenderingContext, type: GLenum, source: string): WebGLShader {
    const shader = gl.createShader(type);
    gl.shaderSource(shader, source);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
      alert('An error occured compiling the shaders: ' + gl.getShaderInfoLog(shader));
      gl.deleteShader(shader);
      return null;
    }

    return shader;
  }

  initShaderProgram(gl: WebGLRenderingContext, vsSource: string, fsSource: string): WebGLProgram {
    const vertexShader: WebGLShader = this.loadShader(gl, gl.VERTEX_SHADER, vsSource);
    const fragmentShader: WebGLShader = this.loadShader(gl, gl.FRAGMENT_SHADER, fsSource);

    const shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);

    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
      alert('Unable to initialize the shader program: ' + gl.getProgramInfoLog(shaderProgram));
      gl.deleteProgram(shaderProgram);
      return null;
    }

    return shaderProgram;
  }

  fullscreen() {
    let canvas = this.canvas;
    if (canvas.nativeElement.requestFullscreen) {
      canvas.nativeElement.requestFullscreen();
    } else {
      alert("This browser doesn't supporter fullscreen");
    }
  };

  exitfullscreen() {
    if (document.exitFullscreen) {
      document.exitFullscreen();
    } else {
      alert("Exit fullscreen doesn't work");
    }
  }
}
