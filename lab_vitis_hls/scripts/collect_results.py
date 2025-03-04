import pathlib
import re
import subprocess


LAB_DIR_PATH = pathlib.Path(__file__).parent.parent.resolve()

results_path = LAB_DIR_PATH / "results" / "results.csv"
results_path.parent.mkdir(parents=True, exist_ok=True)

with results_path.open("w") as f:
    f.write("k,accuracy,fmax,bram,dsp,ff,lut\n")

    for k in range(1, 6):

        # Run C simulation
        subprocess.run(["make", "c_simulation", f"K={k}"])

        dat_path = LAB_DIR_PATH / "temp" / "hls" / "csim" / "build" / "out.dat"
        txt = dat_path.read_text()

        m = re.search("Overall Accuracy = (.*?)%", txt)
        accuracy = float(m.group(1))

        # Run C synthesis
        subprocess.run(["make", "c_synthesis", f"K={k}"])
        rpt_path = (
            LAB_DIR_PATH / "temp" / "hls" / "syn" / "report" / "digitrec_csynth.rpt"
        )

        txt = rpt_path.read_text()
        m = re.search(
            r"^== Utilization Estimates.*?\|Total\s+\|\s+(\d+)\|\s+(\d+)\|\s+(\d+)\|\s+(\d+)",
            txt,
            re.MULTILINE | re.DOTALL,
        )
        assert m
        bram = int(m.group(1))
        dsp = int(m.group(2))
        ff = int(m.group(3))
        lut = int(m.group(4))

        m = re.search(
            r"^\+ Timing:.*?^\s+\|ap_clk.*?\|.*?\|\s+(.*?) ns",
            txt,
            re.MULTILINE | re.DOTALL,
        )
        assert m
        fmax = float(m.group(1))

        f.write(f"{k},{accuracy},{fmax},{bram},{dsp},{ff},{lut}\n")
