"""
Flood-fill the background pixels of cricket sprites in images.h.
Replaces edge-connected 0xFFFF pixels with BG_COLOR (0x9772),
leaving interior white pixels (uniform/content) untouched.

BG_COLOR = ST7735_Color565(144, 238, 144)
         = ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3)
         = 0x9772
"""
import re
from collections import deque

BG_COLOR   = 0x9772
TRANSPARENT = 0xFFFF

IMAGES_H = r'c:\Users\Joshi\OneDrive\Documents\MSPM0_ValvanoWare\ECE319K_Lab9H\images\images.h'

# (width, height) of each sprite as drawn by ST7735_DrawBitmap
SPRITE_DIMS = {
    'bowler1':       (16, 33),
    'bowler2':       (16, 33),
    'bowler3':       (16, 33),
    'bowler4':       (16, 33),
    'bowler5':       (16, 33),
    'bowler6':       (16, 33),
    'bowler7':       (16, 33),
    'batsman_idle':  (34, 46),
    'left_batsman1': (34, 46),
    'left_batsman2': (34, 46),
    'left_batsman3': (34, 46),
    'right_batsman1':(34, 46),
    'right_batsman2':(34, 46),
    'right_batsman3':(34, 46),
    'right_batsman4':(34, 46),
    'stump1':        (13, 13),
    'stump2':        (13, 13),
}


def flood_fill_background(pixels, W, H):
    """BFS from every edge pixel that is TRANSPARENT; returns set of 1-D indices."""
    visited = set()
    queue = deque()
    for row in range(H):
        for col in range(W):
            if row in (0, H-1) or col in (0, W-1):
                idx = row * W + col
                if pixels[idx] == TRANSPARENT and idx not in visited:
                    visited.add(idx)
                    queue.append((row, col))
    while queue:
        row, col = queue.popleft()
        for dr, dc in ((-1,0),(1,0),(0,-1),(0,1)):
            nr, nc = row+dr, col+dc
            if 0 <= nr < H and 0 <= nc < W:
                idx = nr * W + nc
                if idx not in visited and pixels[idx] == TRANSPARENT:
                    visited.add(idx)
                    queue.append((nr, nc))
    return visited


with open(IMAGES_H, 'r') as f:
    content = f.read()

for name, (W, H) in SPRITE_DIMS.items():
    pat = (r'(const unsigned short ' + re.escape(name) +
           r'\[\] = \{)(.*?)(\};)')
    m = re.search(pat, content, re.DOTALL)
    if not m:
        print(f'WARNING: {name} not found — skipping')
        continue

    body  = m.group(2)
    hexes = re.findall(r'0x[0-9A-Fa-f]+', body)
    pixels = [int(v, 16) for v in hexes]

    expected = W * H
    if len(pixels) != expected:
        print(f'WARNING: {name} has {len(pixels)} pixels, expected {expected} — skipping')
        continue

    bg = flood_fill_background(pixels, W, H)
    for idx in bg:
        pixels[idx] = BG_COLOR

    # Rebuild body: replace hex tokens in order
    counter = [0]
    def replace_hex(mat):
        val = pixels[counter[0]]
        counter[0] += 1
        return f'0x{val:04X}'

    new_body = re.sub(r'0x[0-9A-Fa-f]+', replace_hex, body)
    new_block = m.group(1) + new_body + m.group(3)
    content = content[:m.start()] + new_block + content[m.end():]

    interior = sum(1 for i,p in enumerate(pixels) if p == TRANSPARENT)
    print(f'{name}: {len(bg)} bg pixels -> 0x{BG_COLOR:04X}, '
          f'{interior} interior white pixels kept', flush=True)

with open(IMAGES_H, 'w') as f:
    f.write(content)

print('\nDone — images.h updated.')
