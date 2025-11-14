import tkinter as tk
from tkinter import filedialog, messagebox
from PIL import Image, ImageDraw, ImageFont, ImageTk
import os

class AsciiTextureApp:
    def __init__(self, root):
        self.root = root
        self.root.title("ASCIIテクスチャ生成ツール（光学補正・カーニング対応）")
        self.root.geometry("480x240")

        self.font_path = None
        self.texture_size = tk.IntVar(value=512)
        self.output_name = tk.StringVar(value="ascii_texture.png")
        self.outline_enabled = tk.BooleanVar(value=False)
        self.optical_adjust_enabled = tk.BooleanVar(value=True)
        self.kerning_enabled = tk.BooleanVar(value=True)

        self.create_widgets()

    def create_widgets(self):
        tk.Button(self.root, text="フォントを選択", command=self.select_font).grid(row=0, column=0, padx=10, pady=10, sticky="ew")

        tk.Label(self.root, text="テクスチャサイズ:").grid(row=1, column=0, sticky="e")
        tk.Entry(self.root, textvariable=self.texture_size, width=10).grid(row=1, column=1, sticky="w")

        tk.Label(self.root, text="出力ファイル名:").grid(row=2, column=0, sticky="e")
        tk.Entry(self.root, textvariable=self.output_name, width=30).grid(row=2, column=1, sticky="w")

        tk.Checkbutton(self.root, text="縁取りを有効にする", variable=self.outline_enabled).grid(row=3, column=0, columnspan=2)
        tk.Checkbutton(self.root, text="光学的補正を有効にする", variable=self.optical_adjust_enabled).grid(row=4, column=0, columnspan=2)
        tk.Checkbutton(self.root, text="カーニング調整を有効にする", variable=self.kerning_enabled).grid(row=5, column=0, columnspan=2)

        tk.Button(self.root, text="画像を生成", command=self.generate_texture).grid(row=6, column=0, columnspan=2, pady=10)

        self.preview_label = tk.Label(self.root)
        self.preview_label.grid(row=7, column=0, columnspan=2, padx=10, pady=10)

    def select_font(self):
        path = filedialog.askopenfilename(
            title="フォントを選択",
            initialdir=os.getcwd(),
            filetypes=[("TrueTypeフォント", "*.ttf"), ("すべてのファイル", "*.*")]
        )
        if path:
            self.font_path = path
            font_name = os.path.splitext(os.path.basename(path))[0]
            self.output_name.set(f"{font_name}_ascii_{self.texture_size.get()}.png")
            messagebox.showinfo("フォント選択", f"選択されたフォント:\n{os.path.basename(path)}")

    def find_max_font_size(self, font_path, cell_w, cell_h, margin):
        for size in range(1, 500):
            font = ImageFont.truetype(font_path, size)
            ascent, descent = font.getmetrics()
            if ascent + descent > (cell_h - 2 * margin):
                return size - 1
        return 100

    def draw_text_with_outline(self, draw, position, text, font):
        x, y = position
        outline_color = (0, 0, 0, 255)
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                if dx != 0 or dy != 0:
                    draw.text((x + dx, y + dy), text, font=font, fill=outline_color)
        draw.text((x, y), text, font=font, fill=(255, 255, 255, 255))

    def generate_texture(self):
        if not self.font_path:
            messagebox.showerror("エラー", "まずフォントを選択してください。")
            return

        try:
            tex_size = int(self.texture_size.get())
        except ValueError:
            messagebox.showerror("エラー", "テクスチャサイズは整数で入力してください。")
            return

        output_path = self.output_name.get()
        if not output_path.lower().endswith(".png"):
            output_path += ".png"

        cols, rows = 16, 16
        cell_w = tex_size // cols
        cell_h = tex_size // rows
        margin = 2

        font_size = self.find_max_font_size(self.font_path, cell_w, cell_h, margin)
        font = ImageFont.truetype(self.font_path, font_size)

        ascent, descent = font.getmetrics()
        line_height = ascent + descent
        y_offset = (cell_h - line_height) // 2

        ascii_chars = [chr(i) for i in range(32, 127)]

        # 光学的補正テーブル（符号逆転済み）
        optical_adjustments = {
            'Q': 2,
            'g': 1,
            'y': 1,
            'p': 1,
            'j': 1,
            'a': -1,
            'c': -1,
            'e': -1,
            't': -1,
        }

        image = Image.new("RGBA", (tex_size, tex_size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(image)

        for i, ch in enumerate(ascii_chars):
            col = i % cols
            row = i // cols
            x = col * cell_w
            y = row * cell_h

            bbox = font.getbbox(ch)
            w = bbox[2] - bbox[0]
            offset_x = bbox[0]

            # カーニング調整あり
            if self.kerning_enabled.get():
                text_x = x + (cell_w - w) // 2 - offset_x
            else:
                text_x = x + margin

            text_y = y + y_offset

            # 光学補正適用（逆符号に修正）
            if self.optical_adjust_enabled.get():
                adjust_y = optical_adjustments.get(ch, 0)
                text_y -= adjust_y

            # マージン制限
            text_x = max(x + margin, text_x)
            text_y = max(y + margin, text_y)

            if self.outline_enabled.get():
                self.draw_text_with_outline(draw, (text_x, text_y), ch, font)
            else:
                draw.text((text_x, text_y), ch, font=font, fill=(255, 255, 255, 255))

        image.save(output_path)

        preview_img = image.resize((min(tex_size, 256),) * 2)
        self.preview_image = ImageTk.PhotoImage(preview_img)
        self.preview_label.config(image=self.preview_image)

        messagebox.showinfo("完了", f"保存完了:\n{output_path}\nフォントサイズ: {font_size}px")

if __name__ == "__main__":
    root = tk.Tk()
    app = AsciiTextureApp(root)
    root.mainloop()
