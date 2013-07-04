import chiplotle
import random
from chiplotle.hpgl.commands import PA, PR, PU, PD

plotters = chiplotle.instantiate_plotters()
plotter = plotters[0]
plotter.select_pen(1)
coords = [(x, random.randint(0, 1000)) for x in range(0, 1000, 10)]
plotter.write(PD(coords))
plotter.select_pen(0)
