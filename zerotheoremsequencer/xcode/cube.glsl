
uniform sampler2D remap;
uniform sampler2D pretty;

uniform sampler2D img0;
uniform sampler2D img1;
uniform sampler2D img2;

uniform sampler2D img3;
uniform sampler2D img4;
uniform sampler2D img5;

uniform sampler2D img6;
uniform sampler2D img7;
uniform sampler2D img8;

uniform float fader;

void main() {

  vec4 pval = texture2D(pretty, gl_TexCoord[0].xy);

  vec2 rval = texture2D(remap,  gl_TexCoord[0].st).xy;
  float x = rval.x;
  float y = rval.y;
  float tw = 0.2;

  // use pretty for non remapped space
  if(x <= 0.0 || y <= 0.0) {
    gl_FragColor = pval;
  }

  // face f1,f2,f3
  else if(y < tw+tw) {
    discard;
  }

  // face a1,a2,a3
  else if(x < tw) {
    if(y < tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw)*5.0);
      gl_FragColor = texture2D(img0, coord2) * pval * fader + pval * ( 1.0 - fader );
    } else if(y < tw+tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img1, coord2) * pval * fader + pval * ( 1.0 - fader );
    } else {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img2, coord2) * pval * fader + pval * ( 1.0 - fader );
    }
  }

  // face b1,b2,b3
  else if(x < tw+tw) {
    x-=tw;
    if(y < tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw)*5.0);
      gl_FragColor = texture2D(img3, coord2) * pval * fader + pval * ( 1.0 - fader );
    } else if(y < tw+tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img4, coord2) * pval * fader + pval * ( 1.0 - fader );
    } else {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img5, coord2) * pval * fader + pval * ( 1.0 - fader );
    }
  }

  // face c1,c2,c3
  else if(x < tw+tw+tw) {
    x-=(tw+tw);
    if(y < tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw)*5.0);
      gl_FragColor = texture2D(img6, coord2) * pval * fader + pval * ( 1.0 - fader );
    } else if(y < tw+tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img7, coord2) * pval * fader + pval * ( 1.0 - fader );
    } else {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img8, coord2) * pval * fader + pval * ( 1.0 - fader );
    }
  }

  // out of our scope
  else {
    discard;
  }

}
