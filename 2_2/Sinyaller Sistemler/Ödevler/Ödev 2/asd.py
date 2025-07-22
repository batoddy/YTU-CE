import matplotlib.pyplot as plt
import numpy as np

PI = np.pi

def draw_signal_sin(a,f,p):
    t = np.linspace(0, 1, 1000)

    sin_x = a * np.sin(2*PI*f*t + p)
    
    return (sin_x)
    
def draw_signal_cos(a,f,p):
    t = np.linspace(0, 1, 1000)

    cos_x = a * np.cos(2*PI*f*t + p)
    
    return (cos_x)
    
    
def plot_signal(sinx, cosx, a, f, p):
    print("Grafik çiziliyor..")
    
    t = np.linspace(0, 1, 1000)
    fig, axs = plt.subplots(2, 1, figsize=(8, 6), sharex=True)

    axs[0].plot(t, sinx)
    axs[0].set_title(f"Sinüs Dalgası: A={a}, f={f} Hz, φ={p} rad")
    axs[0].set_ylabel("Genlik")
    axs[0].grid(True)

    axs[1].plot(t, cosx)
    axs[1].set_title(f"Kosinüs Dalgası: A={a}, f={f} Hz, φ={p} rad")
    axs[1].set_xlabel("Zaman (saniye)")
    axs[1].set_ylabel("Genlik")
    axs[1].grid(True)

    plt.tight_layout()
    plt.show()

def main():

    a = int(input("A:"))
    f = int(input("f:"))
    p = PI / int(input("phase:"))
    
    sinx = draw_signal_sin(a,f,p)
    cosx = draw_signal_cos(a,f,p)
    
    plot_signal(sinx,cosx,a,f,p)

if "__main__":
    main()