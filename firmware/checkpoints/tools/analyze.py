# analyze.py - tremor reduction per mode.   Usage:  python analyze.py data/run01.csv
import sys, numpy as np, pandas as pd
from scipy.signal import butter, filtfilt, welch

df = pd.read_csv(sys.argv[1]); fs = 100.0                    # logger rate
b, a = butter(2, [3 / (fs / 2), 12 / (fs / 2)], btype="band")
rms = lambda x: np.sqrt(np.mean(np.square(x)))
switched = df["mode"].diff().fillna(0).ne(0)
recent = switched.rolling(int(fs), min_periods=1).max()     # 1 s after a mode switch
settled = ~recent.astype(bool).to_numpy()                   # skip those samples
for ax in ("roll", "pitch"):
    h = filtfilt(b, a, df[f"hand_{ax}"]); p = filtfilt(b, a, df[f"plat_{ax}"])
    f, P = welch(h, fs, nperseg=min(512, len(h)))           # the input this run actually had
    print(f"{ax}: input {f[np.argmax(P)]:.1f} Hz, about +/-{np.sqrt(2) * rms(h):.1f} deg")
    for m in sorted(df["mode"].unique()):
        k = (df["mode"] == m).to_numpy() & settled
        red = 100 * (1 - rms(p[k]) / rms(h[k]))
        print(f"  {'on ' if m else 'off'}: handle {rms(h[k]):.2f}, spoon {rms(p[k]):.2f} "
              f"deg RMS, reduction {red:.0f}%")
