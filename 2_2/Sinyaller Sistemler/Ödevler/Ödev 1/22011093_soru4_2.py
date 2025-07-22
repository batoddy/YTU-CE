import sounddevice as sd
import numpy as np
import scipy.io.wavfile as wav
import matplotlib.pyplot as plt


# def plot(x, y, x_indis, y_indis,z):
#     fig, axs = plt.subplots(2, 2, figsize=(8, 6), sharex=False)

#     y_min = min(min(x), min(y), min(z_my), min(z_np))
#     y_max = max(max(x), max(y), max(z_my), max(z_np))
#     padding = (y_max - y_min) * 0.1
#     y_min -= padding
#     y_max += padding

#     x_min = min(min(x_indis), min(y_indis), min(z_indis))
#     x_max = max(max(x_indis), max(y_indis), max(z_indis))
#     x_padding = (x_max - x_min) * 0.8
#     x_min -= x_padding
#     x_max += x_padding

#     for ax in axs.flat:
#         ax.set_ylim(y_min, y_max)
#         ax.set_xlim(x_min, x_max)

#     axs[0, 0].stem(x_indis, x, basefmt=" ", linefmt="r-", markerfmt="ro")
#     axs[0, 0].set_title("x[n]")
#     axs[0, 0].set_ylabel("Genlik")
#     axs[0, 0].grid(True)

#     axs[0, 1].stem(y_indis, y, basefmt=" ", linefmt="b-", markerfmt="bo")
#     axs[0, 1].set_title("y[n]")
#     axs[0, 1].set_ylabel("Genlik")
#     axs[0, 1].grid(True)

#     axs[1, 0].stem(z_indis, z_my, basefmt=" ", linefmt="orange", markerfmt="o")
#     axs[1, 0].set_title("z[n] = x[n] * y[n] (Batuhan)")
#     axs[1, 0].set_xlabel("Zaman (n)")
#     axs[1, 0].set_ylabel("Genlik")
#     axs[1, 0].grid(True)

#     axs[1, 1].stem(z_indis, z_np, basefmt=" ", linefmt="purple", markerfmt="o")
#     axs[1, 1].set_title("z[n] = x[n] * y[n] (Numpy)")
#     axs[1, 1].set_xlabel("Zaman (n)")
#     axs[1, 1].set_ylabel("Genlik")
#     axs[1, 1].grid(True)

#     plt.tight_layout()
#     plt.show()


fs, x_5sec = wav.read("kayit_5sec.wav")
# sd.play(x_5sec, samplerate=fs)
# sd.wait()

fs, x_10sec = wav.read("kayit_10sec.wav")
# sd.play(x_10sec, samplerate=fs)
# sd.wait()

fs, y1_3 = wav.read("kayit_y1_3.wav")
# sd.play(y1_3, samplerate=fs)
# sd.wait()

fs, y1_4 = wav.read("kayit_y1_4.wav")
# sd.play(y1_4, samplerate=fs)
# sd.wait()

fs, y1_5 = wav.read("kayit_y1_5.wav")
# sd.play(y1_5, samplerate=fs)
# sd.wait()

fs, y2_3 = wav.read("kayit_y2_3.wav")
# sd.play(y2_3, samplerate=fs)
# sd.wait()

fs, y2_4 = wav.read("kayit_y2_4.wav")
# sd.play(y2_4, samplerate=fs)
# sd.wait()

fs, y2_5 = wav.read("kayit_y2_5.wav")
# sd.play(y2_5, samplerate=fs)
# sd.wait()

fs, myy1_3 = wav.read("kayit_myy1_3.wav")
# sd.play(y1_3, samplerate=fs)
# sd.wait()

fs, myy1_4 = wav.read("kayit_myy1_4.wav")
# sd.play(y1_4, samplerate=fs)
# sd.wait()

fs, myy1_5 = wav.read("kayit_myy1_5.wav")
# sd.play(y1_5, samplerate=fs)
# sd.wait()

fs, myy2_3 = wav.read("kayit_myy2_3.wav")
# sd.play(y2_3, samplerate=fs)
# sd.wait()

fs, myy2_4 = wav.read("kayit_myy2_4.wav")
# sd.play(y2_4, samplerate=fs)
# sd.wait()

fs, myy2_5 = wav.read("kayit_myy2_5.wav")
# sd.play(y2_5, samplerate=fs)
# sd.wait()

plt.plot(range(len(x_5sec)), x_5sec)
plt.show()

plt.plot(range(len(x_10sec)), x_10sec)
plt.show()

plt.plot(range(len(y1_3)), y1_3)
plt.show()


plt.plot(range(len(y1_4)), y1_4)
plt.show()


plt.plot(range(len(y1_5)), y1_5)
plt.show()
