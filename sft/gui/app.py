import sys
import argparse
import serial
from serial.tools import list_ports
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as animation

from PyQt5 import QtWidgets
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas

matplotlib.use("Qt5Agg")


DEFAULT_PORT = "COM3"
DEFAULT_BAUDRATE = 9600

WINDOW_TITLE = "Static Fire Testing Curve"
X_LABEL = "Sample Number"
Y_LABEL = "Thrust (N)"
MAX_DATA_POINTS = 1000


def parse_args():
    parser = argparse.ArgumentParser(description="Static Fire Testing Curve")
    parser.add_argument(
        "-p",
        "--port",
        type=str,
        default=DEFAULT_PORT,
        help="Serial port (e.g., COM3 or /dev/ttyUSB0)",
    )
    parser.add_argument(
        "-b",
        "--baudrate",
        type=int,
        default=DEFAULT_BAUDRATE,
        help="Baud rate for serial communication (e.g., 9600)",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        default="plot.png",
        help="Output filename for the plot",
    )
    return parser.parse_args()


def search_port():
    # print the ports available
    print("Available ports:")
    ports = list_ports.comports()
    for port, desc, hwid in sorted(ports):
        print(f"{port}: {desc}")


args = parse_args()
try:
    ser = serial.Serial(args.port, args.baudrate, timeout=1)
except serial.SerialException:
    print(f"Unable to open port {args.port}")
    search_port()
    sys.exit(1)

out_filename = args.output or "plot.txt"
data = []


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle(WINDOW_TITLE)
        self.setGeometry(100, 100, 800, 600)

        central_widget = QtWidgets.QWidget()
        self.setCentralWidget(central_widget)
        layout = QtWidgets.QVBoxLayout(central_widget)

        self.fig, self.ax = plt.subplots()
        self.canvas = FigureCanvas(self.fig)

        self.start_button = QtWidgets.QPushButton("Start")
        self.start_button.clicked.connect(lambda: self.ani.event_source.start())

        # save the data to a file
        self.save_button = QtWidgets.QPushButton("Save")
        self.save_button.clicked.connect(self.save_plot)

        layout.addWidget(self.canvas)
        self.ani = animation.FuncAnimation(
            self.fig, self.update_plot, interval=20, cache_frame_data=False
        )

    def update_plot(self, frame):
        try:
            line = ser.readline().decode("utf-8").strip()
            if line:
                try:
                    value = float(line)
                    data.append(value)

                    if len(data) > 1000:
                        data.pop(0)

                    self.ax.clear()
                    self.ax.set_xlim(max(0, len(data) - MAX_DATA_POINTS), len(data))

                    self.ax.plot(data)
                    self.ax.set_title(WINDOW_TITLE)
                    self.ax.set_xlabel(X_LABEL)
                    self.ax.set_ylabel(Y_LABEL)

                    self.canvas.draw()
                except ValueError:
                    print("Unable to convert value to float")
        except:
            # not the best way to handle this, but it works
            pass

    def save_plot(self):
        self.fig.savefig(out_filename)

    def closeEvent(self, event):
        ser.close()
        event.accept()


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())

# close the serial port when the window is closed
ser.close()
