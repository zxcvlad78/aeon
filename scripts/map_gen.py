# random map generation

import json
import random
import pyperclip

width: int = 29
height: int = 31
total: int = width * height

tiles: list = [random.randint(1, 5) for _ in range(total)]

data: dict = {
    "width": width,
    "height": height,
    "tiles": tiles
}

pyperclip.copy(json.dumps(data, indent=2))