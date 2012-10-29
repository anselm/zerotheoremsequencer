

uniform sampler2D remap;
uniform sampler2D pretty;

uniform sampler2D img9;
uniform sampler2D img10;
uniform sampler2D img11;

uniform sampler2D img12;
uniform sampler2D img13;
uniform sampler2D img14;

uniform sampler2D img15;
uniform sampler2D img16;
uniform sampler2D img17;

uniform float fader;

void main() {

  vec4 pval = texture2D(pretty, gl_TexCoord[0].xy);

  vec2 rval = texture2D(remap,  gl_TexCoord[0].st).xy;
  float x = rval.r;
  float y = rval.g;
  float tw = 1.0/5.0;

  // use pretty for non remapped space
  if(x <= 0.0 || y <= 0.0) {
    discard;
  }

  // face f1,f2,f3
  else if(y < tw+tw) {
    if(x < tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw)*5.0);
      gl_FragColor = texture2D(img15, coord2) * pval * fader + pval * (1.0 - fader);
    } else if(x < tw+tw) {
      vec2 coord2 = vec2(1.0-(x-tw)*5.0,1.0-(y-tw)*5.0);
      gl_FragColor = texture2D(img16, coord2) * pval * fader + pval * (1.0 - fader);
    } else {
      vec2 coord2 = vec2(1.0-(x-tw-tw)*5.0,1.0-(y-tw)*5.0);
      gl_FragColor = texture2D(img17, coord2) * pval * fader + pval * (1.0 - fader);
    }
  }

 else if(x < tw+tw+tw) {
   discard;
 }

  // face d1,d2,d3
  else if(x < tw+tw+tw+tw) {
    x-=(tw+tw+tw);
    if(y < tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw)*5.0);
      gl_FragColor = texture2D(img9, coord2) * pval * fader + pval * (1.0 - fader);
    } else if(y < tw+tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img10, coord2) * pval * fader + pval * (1.0 - fader);
    } else  {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img11, coord2) * pval * fader + pval * (1.0 - fader);
    } 
  }

  // face e1,e2,e3
  else {
    x-=(tw+tw+tw+tw);
    if(y < tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw)*5.0);
      gl_FragColor = texture2D(img12, coord2) * pval * fader + pval * (1.0 - fader);
    } else if(y < tw+tw+tw+tw) {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img13, coord2) * pval * fader + pval * (1.0 - fader);
    } else {
      vec2 coord2 = vec2(1.0-x*5.0,1.0-(y-tw-tw-tw-tw)*5.0);
      gl_FragColor = texture2D(img14, coord2) * pval * fader + pval * (1.0 - fader);
    } 
  }



}