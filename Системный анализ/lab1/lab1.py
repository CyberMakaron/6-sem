# This Python file uses the following encoding: utf-8
import sys
import random
import numpy
import math

from scipy.stats import norm
import matplotlib.pyplot as pyplot

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from PyQt5.QtWidgets import  * #QApplication, QMainWindow, QMenu, QVBoxLayout, QHBoxLayout, QFormLayout, QGridLayout, QSizePolicy, QMessageBox, QWidget, QPushButton, QLineEdit, QLabel
from PyQt5.QtGui import QIcon

n = 6
mx = 3
sigma = 1
sz = 10000


def getRandomNumbers(sz):
    result = []
    for i in range (0, sz):
        result.append(random.random())
    return result

def getGaussian(sz):
    result = []
    for i in range(0, sz):
        v = getRandomNumbers(n)
        x = sigma * math.sqrt(2) * (numpy.sum(v) - n / 2) + mx
        result.append(x)
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
        result = getGaussian(sz)
        min = numpy.min(result)
        max = numpy.max(result)
        step = (abs(min) + max)/ 10
        a = numpy.arange(min, max, step)
        data = numpy.full_like(a, 0)
        x_axis = numpy.arange(min - 1, max + 1, 0.001)
        
        ax = self.figure.add_subplot(111)

        ax.plot(x_axis, norm.pdf(x_axis, mx, sigma), color = "mediumblue")
        ax.hist(result, 40, density=True, color = "tomato")

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

        #self.muEdit = QLineEdit()
        #self.sigmaEdit = QLineEdit()

        #self.muLabel = QLabel("mu")
        #self.sigmaLabel = QLabel("sigma")


        #self.dataLayout.addWidget(self.muLabel)
        #self.dataLayout.addWidget(self.muEdit)
        #self.dataLayout.addWidget(self.sigmaLabel)
        #self.dataLayout.addWidget(self.sigmaEdit)
        self.plotLayout.addLayout(self.dataLayout)

    def connectUI(self):
        self.generateButton.clicked.connect(self.plotWidget.plot)

    #def setParams(newMu, newSigma):
    #    mu = newMu
    #    sigma = newSigma

if __name__ == "__main__":
    app = QApplication([])
    window = MainWindow()
    window.resize(800, 600)
    window.setWindowTitle("Лабораторная №1 Клесов")
    window.show()
    sys.exit(app.exec_())
