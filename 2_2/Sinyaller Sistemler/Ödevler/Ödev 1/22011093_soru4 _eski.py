import sounddevice as sd
import numpy as np
import scipy.io.wavfile as wav
import matplotlib.pyplot as plt

def myConv(x, n, y, m):
    L = n + m - 1  

    z = [0.0] * L
    a=0
    # z[k] = Σ x[i] * y[k - i]
    for k in range(L):
        toplam = 0.0
        i_min = max(0, k - m + 1)
        i_max = min(k + 1, n)
        
        for i in range(i_min, i_max):
            toplam += x[i] * y[k - i]
        # print(a)
        z[k] = toplam

    k_indis = list(range(L))
    return z, k_indis, L

def func(x,M):
    y = [0.0] * len(x)
    for i in range(len(x)):
        sum = 0
        for k in range(M):
            if i-400*k > 0:
                sum += 0.5 * k *x[i - 400*k]
        y[i] = x[i] + sum
    
    return y

def save_wav(filename, fs, data):

    data = np.array(data)  
    data = np.clip(data, -1.0, 1.0)  
    data = (data * 32767).astype(np.int16)  
    wav.write(filename, fs, data)
    

fs ,x1 = wav.read("kayit_5sec.wav")
fas ,x2 = wav.read("kayit_10sec.wav")

# sd.play(x1, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle

# sd.play(x2, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle

y1_3 = func(x1,3)
y1_4 = func(x1,4)
y1_5 = func(x1,5)

y2_3 = func(x2,3)
y2_4 = func(x2,4)
y2_5 = func(x2,5)

# sd.play(y1_3, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y1_4, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y1_5, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle

# sd.play(y2_3, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y2_4, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y2_5, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y1_3, samplerate=44100)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y1_4, samplerate=44100)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y1_5, samplerate=44100)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle

print("Convoling..")
y1_3 = np.convolve(x1, y1_3, mode='full') 
y1_4 = np.convolve(x1, y1_4, mode='full') 
y1_5 = np.convolve(x1, y1_5, mode='full') 
print("y1 convoled.")

save_wav("kayit_y1_3.wav", fs, y1_3)
save_wav("kayit_y1_4.wav", fs, y1_4)
save_wav("kayit_y1_5.wav", 
          fs, y1_5)
print("y1 saved.")
# plt.plot(range(0,len(y1_3)),y1_3)
# plt.grid(True)
# plt.show()

# fsa ,a = wav.read("kayit_y1_3.wav")
# sd.play(a, samplerate=fsa)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# fsa ,a = wav.read("kayit_y1_4.wav")
# sd.play(a, samplerate=fsa)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# fsa ,a = wav.read("kayit_y1_5.wav")
# sd.play(a, samplerate=fsa)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle

# plt.plot(range(0,len(a)),a)
# plt.grid(True)
# plt.show()

print("Convoling y2..")
y2_3 = np.convolve(x2, y2_3, mode='full') 
y2_4 = np.convolve(x2, y2_4, mode='full') 
y2_5 = np.convolve(x2, y2_5, mode='full') 
print("y2 convoled.")

save_wav("kayit_y2_3.wav", fs, y2_3)
save_wav("kayit_y2_4.wav", fs, y2_4)
save_wav("kayit_y2_5.wav", fs, y2_5)
print("y2 saved.")

# fsa ,a = wav.read("kayit_y2_3.wav")
# sd.play(a, samplerate=fsa)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# fsa ,a = wav.read("kayit_y2_4.wav")
# sd.play(a, samplerate=fsa)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# fsa ,a = wav.read("kayit_y2_5.wav")
# sd.play(a, samplerate=fsa)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle

print("Convoling my y1..")
# myY1_3, i, myY1_len = myConv(x1,len(x1),y1_3,len(y1_3))
# save_wav("kayit_myy1_3.wav", fs, myY1_3)
# print("kayit_myy1_3.wav")
# myY1_4, i, myY1_len = myConv(x1,len(x1),y1_4,len(y1_4))
# save_wav("kayit_myy1_4.wav", fs, myY1_4)
# print("kayit_myy1_4.wav")

# myY1_5, i, myY1_len = myConv(x1,len(x1),y1_5,len(y1_5))
# save_wav("kayit_myy1_5.wav", fs, myY1_5)
# print("kayit_myy1_5.wav")

# print("Convoled my y1..")

# print("y1 saving...")
# print("my y1 saved.")

# print("Convoling my y2..")
# myY2_3, i, myY2_len = myConv(x2,len(x2),y2_3,len(y2_3))
# save_wav("kayit_myy2_3.wav", fs, myY2_3)
# print("kayit_myy2_3.wav")
# myY2_4, i, myY2_len = myConv(x2,len(x2),y2_4,len(y2_4))
# save_wav("kayit_myy2_4.wav", fs, myY2_4)
# print("kayit_myy2_4.wav")
# myY2_5, i, myY2_len = myConv(x2,len(x2),y2_5,len(y2_5))
# save_wav("kayit_myy2_5.wav", fs, myY2_5)
# print("kayit_myy2_5.wav")
# print("Convoled my y2..")

# print("y2 saving...")
# print("my y2 saved.")


# print("Convoling 2_3..")
# print("Convoling 2_4..")
# print("Convoling 2_5..")

# print("Convoling done")

# sd.play(myY1_3, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(myY1_4, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(myY1_5, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(myY2_3, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(myY2_4, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(myY2_5, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle

# sd.play(y1_3, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y1_4, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y1_5, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y2_3, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y2_4, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle
# sd.play(y2_5, samplerate=fs)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle


# sd.play(Y1, samplerate=44100)  # 5 saniyelik kayıt
# sd.wait()  # Çalma işlemi bitene kadar bekle

print("end")
