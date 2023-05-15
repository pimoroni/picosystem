#include <math.h>
#include <array>

#include "picosystem.hpp"

using namespace picosystem;

voice_t piano = voice(20, 200, 50, 50);

struct note_t {
  uint32_t f, d;
};

std::array<note_t, 25> notes = {{
  { _, 4},
  {C4, 2}, { _, 1}, {G3, 1},
  {C4, 2}, { _, 1}, {G3, 1},
  {C4, 1}, {G3, 1}, {C4, 1}, {E4, 1},
  {G4, 4},
  {F4, 2}, { _, 1}, {D4, 1},
  {F4, 2}, { _, 1}, {D4, 1},
  {F4, 1}, {D4, 1}, {B3, 1}, {D4, 1},
  {G3, 4},
  { _, 4},
  { _, 4},
}};

uint32_t bl = 200;  // beat length (ms)
uint32_t ns = 15;   // note spacing
uint32_t sc = 62;  // stave centre
uint32_t ss = 6;   // stave spacing
uint32_t sg = 6;   // stave gap

static uint32_t current = 0;
static uint32_t song_tick = 0;
static uint32_t note_tick = 0;

// convert frequency in hz to a note offset (from middle C)
int32_t frequency_to_note(int32_t f) {
  int32_t steps = log2(float(f) / A4) * 12.0f;
  int32_t octave = (steps / 12) * 7;
  int32_t note_map[12] = {1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7};
  int32_t note = note_map[(12 + (steps % 12)) % 12];
  return octave + note - 3;
}

// returns the y coordinate of the note so that it can be drawn at the
// correct position on the stave
int32_t get_note_y(uint32_t i) {
  note_t n = notes[i];
  uint32_t ny = sc - frequency_to_note(n.f) * (ss / 2);
  if(i == current) {
    ny += sin((float(note_tick) / (n.d * bl)) * 3.1415927) * 8.0f;
  }
  return ny;
}

// returns the offset of the note by adding up the duration of all notes that
// come before it
int32_t note_offset(uint32_t i) {
  uint32_t o = 0;
  for(uint32_t j = 0; j < i; j++) {
    o += notes[j].d;
  }
  return o;
}

// returns the length of the song by adding all of the note durations
int32_t song_length() {
  uint32_t o = 0;
  for(auto n : notes) {
    o += n.d;
  }
  return o;
}

// draw a note shape (full, semi, rest, etc) at position x, y in the
// current pen
void draw_note_shape(note_t n, int32_t x, int32_t y) {
  if(n.f != 0) {
    if(n.d == 1) {
      fcircle(x, y, 3);
      vline(x + 3, y - 14, 14);
    } else if(n.d == 2) {
      circle(x, y, 3);
      vline(x + 3, y - 14, 14);
    } else if(n.d == 4) {
      ellipse(x, y, 4, 3);
    }

    if(n.f == C4) {
      hline(x - 4, y, 9);
    }
  } else {
    int32_t y = sc;
    line(x - 3, y - 7, x + 2, y - 2);
    line(x + 2, y - 2, x - 3, y + 2);
    line(x - 3, y + 2, x + 3, y + 7);
  }
}

// draw the note and its shadow
void draw_note(uint32_t i, int32_t x, color_t c) {
  note_t n = notes[i];
  color_t shadow = rgb(0, 0, 0);
  int32_t y = get_note_y(i);

  pen(shadow);
  draw_note_shape(n, x + 2, y + 2);
  pen(c);
  draw_note_shape(n, x, y);
}

// initialise the world
void init() {
}

// process user input and update the world state
void update(uint32_t tick) {
  note_tick += 25;
  song_tick += 25;

  if(note_tick > (notes[current].d * bl)) {
    // if current note has finished playing then move onto the next one
    if(++current >= notes.size()) {
      current = 0;
      song_tick = 0;
    }

    // reset the note tick counter and start playing the note
    note_tick = 0;
    play(piano, notes[current].f, notes[current].d * bl);
  }
}

// draw the world
void draw(uint32_t tick) {
  // reset the camera and clear the framebuffer
  camera();
  pen(1, 2, 3);
  clear();

  // draw title and bouncing notes
  pen(15, 15, 15);
  text("Mini Mozart Maestro", 13, 10);
  sprite(189, 0, 10 + sin(time() / 100.0f) * 4.0f);
  sprite(189, 110, 10 + sin((time() + 500) / 100.0f) * 4.0f);

  // draw staves
  pen(6, 6, 6);
  for(int i = 0; i < 5; i++) {
    hline(0, sc - i * ss - sg, 240);
    hline(0, sc + i * ss + sg, 240);
  }

  // set the camera offset to track the music as it plays
  camera(-90 + ((song_tick * ns) / bl), 0);

  // draw bar separators
  for(int i = 0; i < song_length(); i += 4) {
    vline(i * ns - (ns / 2), sc - (ss * 5), (ss * 10));
  }

  // draw notes
  uint32_t offset = 0;
  for(int i = 0; i < notes.size(); i++) {
    color_t c = i == current ? rgb(15, 15, 15) : rgb(11, 13, 12);
    draw_note(i, note_offset(i) * ns, c);
  }

  // decorate with dancing flowers
  camera();
  for(int i = 0; i < 18; i+=4) {
    int y = sin(time() / 200.0f + i / 2.0f) * 3.0f;
    sprite(181 + (y > 0 ? 1 : 0), i * 8 + 2, 100 + y, 1, 1, 16, 16); // dancing flowers
  }
}