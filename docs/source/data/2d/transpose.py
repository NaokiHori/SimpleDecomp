import sys
import matplotlib
matplotlib.use("Agg")
import pyvista
from pyvista import themes


def get_bounds(xmin, ymin, zmin):
    eps = 0.15
    xmax = xmin + 1.
    ymax = ymin + 1.
    zmax = zmin + 1.
    return (xmin+eps, xmax-eps, ymin+eps, ymax-eps, zmin+eps, zmax-eps)

def get_mysize(total, nprocs, myrank):
    return (total+myrank)//nprocs

def get_offset(total, nprocs, myrank):
    retval = 0
    for rank in range(myrank):
        mysize = get_mysize(total, nprocs, rank)
        retval += mysize
    return retval

def get_myrank(nprocs, total, index):
    for myrank in range(nprocs):
        offset = get_offset(total, nprocs, myrank)
        mysize = get_mysize(total, nprocs, myrank)
        ubound = mysize + offset
        if index < ubound:
            return myrank

def get_color(myrank, nprocs):
    colormap = matplotlib.cm.get_cmap("gist_rainbow")
    rgba = colormap(myrank/(nprocs-1))
    return rgba[:3]

def x1pencil(plotter, sizes, nprocs):
    nprocs_total = nprocs[0]*nprocs[1]
    # rank x
    rankxs = list()
    for i in range(sizes[0]):
        rankxs.append(get_myrank(nprocs[0], sizes[0], i))
    # rank y
    rankys = list()
    for j in range(sizes[1]):
        rankys.append(get_myrank(nprocs[1], sizes[1], j))
    # colors
    colors = list()
    for n in range(nprocs_total):
        colors.append(get_color(n, nprocs_total))
    for j in range(sizes[1]):
        ranky = rankys[j]
        for i in range(sizes[0]):
            rankx = rankxs[i]
            myrank = ranky
            box = pyvista.Box(bounds=get_bounds(i, j, 0))
            plotter.add_mesh(box, color=colors[myrank])

def y1pencil(plotter, sizes, nprocs):
    nprocs_total = nprocs[0]*nprocs[1]
    # rank x
    rankxs = list()
    for i in range(sizes[0]):
        rankxs.append(get_myrank(nprocs[1], sizes[0], i))
    # rank y
    rankys = list()
    for j in range(sizes[1]):
        rankys.append(get_myrank(nprocs[0], sizes[1], j))
    # colors
    colors = list()
    for n in range(nprocs_total):
        colors.append(get_color(n, nprocs_total))
    for j in range(sizes[1]):
        ranky = rankys[j]
        for i in range(sizes[0]):
            rankx = rankxs[i]
            myrank = rankx
            box = pyvista.Box(bounds=get_bounds(i, j, 0))
            plotter.add_mesh(box, color=colors[myrank])

def main(pencil):
    sizes = [24, 18]
    # number of processes in each direction
    nprocs = [1, 6]
    # init theme
    my_theme = themes.DefaultTheme()
    my_theme.lighting = True
    my_theme.show_edges = False
    my_theme.background = "#000000"
    my_theme.window_size = [800, 800]
    # init plotter, set camera
    plotter = pyvista.Plotter(theme=my_theme, off_screen=True)
    factor = 1.25
    plotter.camera.position = (-sizes[0]*0.9*factor, -sizes[1]*1.3*factor, 30.)
    plotter.camera.focal_point = (sizes[0]*0.5, sizes[1]*0.5, 3.)
    plotter.add_axes()
    if pencil == "x1":
        x1pencil(plotter, sizes, nprocs)
    if pencil == "y1":
        y1pencil(plotter, sizes, nprocs)
    plotter.show(screenshot=f"{pencil}pencil.png")
    plotter.close()

if __name__ == "__main__":
    assert(len(sys.argv[1]) == 2)
    main(sys.argv[1])

