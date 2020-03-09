# This Python file uses the following encoding: utf-8
import sys
import random
import numpy
import math

from scipy import integrate
from scipy import optimize as opt
from scipy.stats import norm
import matplotlib.pyplot as pyplot

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from PyQt5.QtWidgets import  * #QApplication, QMainWindow, QMenu, QVBoxLayout, QHBoxLayout, QFormLayout, QGridLayout, QSizePolicy, QMessageBox, QWidget, QPushButton, QLineEdit, QLabel
from PyQt5.QtGui import QIcon

sz = 100000
a = -5
b = 5
r = random.random()

def getRandomNumbers():
    result = []
    for i in range (0, sz):
        result.append(random.random())
    return result

def fx(x):
    return 3*(x**2) + 2*x + 15
    #return x**2 - 4

K = integrate.quad(fx, a, b)[0]

def fxk(x):
    return fx(x)/K

def gx(x):
    g = 1/K * (x**3 + x**2 + 15*x - a**3 - a**2 - 15*a)
    #g = 1/K * ((x**3)/3 - 4*x - (a**3)/3 + 4*a)
    return (g - r)

def solve():
    result = []
    print("\u2589")

    for i in range(0, sz):
        global r
        r = random.random()
        x = opt.root(gx, a)
        result.append(x.x[0])
    return result

class PlotWidget(QWidget):
    def __init__(self, parent=None):
        super(PlotWidget, self).__init__(parent)
        self.initUI()

    def initUI(self):
        self.mainLayout = QVBoxLayout(self)
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.mainLayout.addWidget(self.canvas)

    def plot(self):
        self.figure.clear()
        result = solve()
        x_axis = numpy.arange(a-0.1, b+0.1, 0.0001)
        ax = self.figure.add_subplot(111)
        ax.plot(x_axis, numpy.apply_along_axis(fxk, 0, x_axis), color = "mediumblue")
        ax.hist(result, 50, density=True, color = "tomato")

        self.canvas.draw()

class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.initUI()
        self.connectUI()
        self.plotWidget.plot()

    def initUI(self):
        self.centralWidget = QWidget(self)
        self.plotLayout =  QVBoxLayout(self.centralWidget)
        self.plotWidget = PlotWidget()
        self.plotLayout.addWidget(self.plotWidget)
        self.buttonsLayout = QVBoxLayout(self.centralWidget)
        self.generateButton = QPushButton("Сгенерировать новые случайные числа")
        self.buttonsLayout.addWidget(self.generateButton)
        self.plotLayout.addLayout(self.buttonsLayout)
        self.setCentralWidget(self.centralWidget)

        self.dataLayout = QGridLayout(self.centralWidget)
        self.plotLayout.addLayout(self.dataLayout)

    def connectUI(self):
        self.generateButton.clicked.connect(self.plotWidget.plot)

if __name__ == "__main__":
    app = QApplication([])
    window = MainWindow()
    window.resize(800, 600)
    window.setWindowTitle("Лабораторная №2 Клесов")
    window.show()
    sys.exit(app.exec_())
