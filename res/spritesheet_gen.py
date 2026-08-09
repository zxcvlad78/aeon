import json
import pyperclip
from PIL import Image
import argparse

def main():
    parser = argparse.ArgumentParser(description="Generate spritesheet JSON from an atlas image.")
    parser.add_argument("--xframesize", type=int, default=16, help="Frame width in pixels")
    parser.add_argument("--yframesize", type=int, default=16, help="Frame height in pixels")
    parser.add_argument("--full", action="store_true", help="Generate frames for all rows (full atlas). If not set, only first row is used.")
    parser.add_argument("--name", type=str, default="default", help="Animation name")
    parser.add_argument("--image", type=str, default="atlas.png", help="Path to atlas image")
    args = parser.parse_args()

    # Используем раздельные размеры
    frame_w = args.xframesize
    frame_h = args.yframesize
    full_gen = args.full
    name = args.name

    try:
        with Image.open(args.image) as img:
            width, height = img.size
    except FileNotFoundError:
        print(f"Ошибка: файл '{args.image}' не найден.")
        return

    # Вычисляем количество кадров с учётом разных размеров
    count_x = width // frame_w
    count_y = height // frame_h

    if count_x == 0 or count_y == 0:
        print(f"Ошибка: изображение ({width}x{height}) слишком мало для кадра {frame_w}x{frame_h}px.")
        return

    # Предупреждение, если изображение не делится нацело
    if width % frame_w != 0 or height % frame_h != 0:
        print(f"Предупреждение: размеры изображения не кратны размеру кадра. Будут использованы целые части: {count_x}x{count_y}.")

    print(f"Обнаружено: {count_x} кадров по горизонтали, {count_y} по вертикали.")

    data = {
        "animations": {
            name: {
                "fps": 18.0,
                "looping": False,
                "frames": []
            }
        }
    }

    rows_to_process = count_y if full_gen else 1

    for row in range(rows_to_process):
        for col in range(count_x):
            data["animations"][name]["frames"].append({
                "x": col * frame_w,
                "y": row * frame_h,
                "w": frame_w,
                "h": frame_h
            })

    json_output = json.dumps(data, indent=2)
    pyperclip.copy(json_output)
    print("JSON скопирован в буфер обмена:\n")
    print(json_output)

if __name__ == "__main__":
    main()