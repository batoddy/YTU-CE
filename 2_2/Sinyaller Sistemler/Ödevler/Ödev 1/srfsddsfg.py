import sounddevice as sd
import numpy as np
import scipy.io.wavfile as wav
import matplotlib.pyplot as plt

# Gerekli tüm ses dosyalarını tekrar yükleyerek değişkenlere atayalım
file_paths_extended = {
    "X1 (5 sec)": "kayit_5sec.wav",
    "X2 (10 sec)": "kayit_10sec.wav",
    "Y1 (M=3, hazır)": "y1_3.wav",
    "Y1 (M=4, hazır)": "y1_4.wav",
    "Y1 (M=5, hazır)": "y1_5.wav",
    "Y2 (M=3, hazır)": "y2_3.wav",
    "Y2 (M=4, hazır)": "y2_4.wav",
    "Y2 (M=5, hazır)": "y2_5.wav",
    "myY1 (M=3, kendi)": "myy1_3.wav",
    "myY1 (M=4, kendi)": "myy1_4.wav",
    "myY1 (M=5, kendi)": "myy1_5.wav",
    "myY2 (M=3, kendi)": "myy2_3.wav",
    "myY2 (M=4, kendi)": "myy2_4.wav",
    "myY2 (M=5, kendi)": "myy2_5.wav",
}

# WAV dosyalarını oku
signals_loaded = {}
for title, path in file_paths_extended.items():
    rate, data = wav.read(path)
    signals_loaded[title] = data

# Her biri için ayrı ayrı plot
# Verilen kodu yalnızca istenen düzende değiştireceğim: "X1 (Giriş)" anahtarı "X1 (5 sec)" olarak düzeltilecek

# Figür: Üstte orijinal sinyal, altta M=3, M=4, M=5 için subplotlar
# 1. Figür - Hazır fonksiyon ile üretilen çıkışlar (Y1)
fig, axs = plt.subplots(4, 1, figsize=(14, 10), sharex=False)

axs[0].plot(signals_loaded["X1 (5 sec)"], color="black")
axs[0].set_title("Orijinal Giriş Sinyali (X1 - 5 saniye)")
axs[0].set_ylabel("Genlik")
axs[0].grid(True)

axs[1].plot(signals_loaded["Y1 (M=3, hazır)"])
axs[1].set_title("Çıkış Sinyali (Hazır Fonksiyon, M = 3)")
axs[1].set_ylabel("Genlik")
axs[1].grid(True)

axs[2].plot(signals_loaded["Y1 (M=4, hazır)"])
axs[2].set_title("Çıkış Sinyali (Hazır Fonksiyon, M = 4)")
axs[2].set_ylabel("Genlik")
axs[2].grid(True)

axs[3].plot(signals_loaded["Y1 (M=5, hazır)"])
axs[3].set_title("Çıkış Sinyali (Hazır Fonksiyon, M = 5)")
axs[3].set_ylabel("Genlik")
axs[3].set_xlabel("Zaman (örnek)")
axs[3].grid(True)

plt.tight_layout()
plt.show()

# 2. Figür - Kendi fonksiyonunla üretilen çıkışlar (myY1)
fig, axs = plt.subplots(4, 1, figsize=(14, 10), sharex=False)

axs[0].plot(signals_loaded["X1 (5 sec)"], color="black")
axs[0].set_title("Orijinal Giriş Sinyali (X1 - 5 saniye)")
axs[0].set_ylabel("Genlik")
axs[0].grid(True)

axs[1].plot(signals_loaded["myY1 (M=3, kendi)"])
axs[1].set_title("Çıkış Sinyali (Kendi Fonksiyonum, M = 3)")
axs[1].set_ylabel("Genlik")
axs[1].grid(True)

axs[2].plot(signals_loaded["myY1 (M=4, kendi)"])
axs[2].set_title("Çıkış Sinyali (Kendi Fonksiyonum, M = 4)")
axs[2].set_ylabel("Genlik")
axs[2].grid(True)

axs[3].plot(signals_loaded["myY1 (M=5, kendi)"])
axs[3].set_title("Çıkış Sinyali (Kendi Fonksiyonum, M = 5)")
axs[3].set_ylabel("Genlik")
axs[3].set_xlabel("Zaman (örnek)")
axs[3].grid(True)

plt.tight_layout()
plt.show()
