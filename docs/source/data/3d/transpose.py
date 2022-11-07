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
    nprocs_total = nprocs[0]*nprocs[1]*nprocs[2]
    # rank x
    rankxs = list()
    for i in range(sizes[0]):
        rankxs.append(get_myrank(nprocs[0], sizes[0], i))
    # rank y
    rankys = list()
    for j in range(sizes[1]):
        rankys.append(get_myrank(nprocs[1], sizes[1], j))
    # rank z
    rankzs = list()
    for k in range(sizes[2]):
        rankzs.append(get_myrank(nprocs[2], sizes[2], k))
    # colors
    colors = list()
    for n in range(nprocs_total):
        colors.append(get_color(n, nprocs_total))
    for k in range(sizes[2]):
        rankz = rankzs[k]
        for j in range(sizes[1]):
            ranky = rankys[j]
            for i in range(sizes[0]):
                rankx = rankxs[i]
                myrank = ranky * nprocs[2] + rankz
                box = pyvista.Box(bounds=get_bounds(i, j, k))
                plotter.add_mesh(box, color=colors[myrank])

def y1pencil(plotter, sizes, nprocs):
    nprocs_total = nprocs[0]*nprocs[1]*nprocs[2]
    # rank x
    rankxs = list()
    for i in range(sizes[0]):
        rankxs.append(get_myrank(nprocs[1], sizes[0], i))
    # rank y
    rankys = list()
    for j in range(sizes[1]):
        rankys.append(get_myrank(nprocs[0], sizes[1], j))
    # rank z
    rankzs = list()
    for k in range(sizes[2]):
        rankzs.append(get_myrank(nprocs[2], sizes[2], k))
    # colors
    colors = list()
    for n in range(nprocs_total):
        colors.append(get_color(n, nprocs_total))
    for k in range(sizes[2]):
        rankz = rankzs[k]
        for j in range(sizes[1]):
            ranky = rankys[j]
            for i in range(sizes[0]):
                rankx = rankxs[i]
                myrank = rankx * nprocs[2] + rankz
                box = pyvista.Box(bounds=get_bounds(i, j, k))
                plotter.add_mesh(box, color=colors[myrank])

def z1pencil(plotter, sizes, nprocs):
    nprocs_total = nprocs[0]*nprocs[1]*nprocs[2]
    # rank x
    rankxs = list()
    for i in range(sizes[0]):
        rankxs.append(get_myrank(nprocs[1], sizes[0], i))
    # rank y
    rankys = list()
    for j in range(sizes[1]):
        rankys.append(get_myrank(nprocs[2], sizes[1], j))
    # rank z
    rankzs = list()
    for k in range(sizes[2]):
        rankzs.append(get_myrank(nprocs[0], sizes[2], k))
    # colors
    colors = list()
    for n in range(nprocs_total):
        colors.append(get_color(n, nprocs_total))
    for k in range(sizes[2]):
        rankz = rankzs[k]
        for j in range(sizes[1]):
            ranky = rankys[j]
            for i in range(sizes[0]):
                rankx = rankxs[i]
                myrank = rankx * nprocs[2] + ranky
                box = pyvista.Box(bounds=get_bounds(i, j, k))
                plotter.add_mesh(box, color=colors[myrank])

def x2pencil(plotter, sizes, nprocs):
    nprocs_total = nprocs[0]*nprocs[1]*nprocs[2]
    # rank x
    rankxs = list()
    for i in range(sizes[0]):
        rankxs.append(get_myrank(nprocs[0], sizes[0], i))
    # rank y
    rankys = list()
    for j in range(sizes[1]):
        rankys.append(get_myrank(nprocs[2], sizes[1], j))
    # rank z
    rankzs = list()
    for k in range(sizes[2]):
        rankzs.append(get_myrank(nprocs[1], sizes[2], k))
    # colors
    colors = list()
    for n in range(nprocs_total):
        colors.append(get_color(n, nprocs_total))
    for k in range(sizes[2]):
        rankz = rankzs[k]
        for j in range(sizes[1]):
            ranky = rankys[j]
            for i in range(sizes[0]):
                rankx = rankxs[i]
                myrank = rankz * nprocs[2] + ranky
                box = pyvista.Box(bounds=get_bounds(i, j, k))
                plotter.add_mesh(box, color=colors[myrank])

def y2pencil(plotter, sizes, nprocs):
    nprocs_total = nprocs[0]*nprocs[1]*nprocs[2]
    # rank x
    rankxs = list()
    for i in range(sizes[0]):
        rankxs.append(get_myrank(nprocs[2], sizes[0], i))
    # rank y
    rankys = list()
    for j in range(sizes[1]):
        rankys.append(get_myrank(nprocs[0], sizes[1], j))
    # rank z
    rankzs = list()
    for k in range(sizes[2]):
        rankzs.append(get_myrank(nprocs[1], sizes[2], k))
    # colors
    colors = list()
    for n in range(nprocs_total):
        colors.append(get_color(n, nprocs_total))
    for k in range(sizes[2]):
        rankz = rankzs[k]
        for j in range(sizes[1]):
            ranky = rankys[j]
            for i in range(sizes[0]):
                rankx = rankxs[i]
                myrank = rankz * nprocs[2] + rankx
                box = pyvista.Box(bounds=get_bounds(i, j, k))
                plotter.add_mesh(box, color=colors[myrank])

def z2pencil(plotter, sizes, nprocs):
    nprocs_total = nprocs[0]*nprocs[1]*nprocs[2]
    # rank x
    rankxs = list()
    for i in range(sizes[0]):
        rankxs.append(get_myrank(nprocs[2], sizes[0], i))
    # rank y
    rankys = list()
    for j in range(sizes[1]):
        rankys.append(get_myrank(nprocs[1], sizes[1], j))
    # rank z
    rankzs = list()
    for k in range(sizes[2]):
        rankzs.append(get_myrank(nprocs[0], sizes[2], k))
    # colors
    colors = list()
    for n in range(nprocs_total):
        colors.append(get_color(n, nprocs_total))
    for k in range(sizes[2]):
        rankz = rankzs[k]
        for j in range(sizes[1]):
            ranky = rankys[j]
            for i in range(sizes[0]):
                rankx = rankxs[i]
                myrank = ranky * nprocs[2] + rankx
                box = pyvista.Box(bounds=get_bounds(i, j, k))
                plotter.add_mesh(box, color=colors[myrank])


def main(pencil):
    sizes = [24, 18, 12]
    # number of processes in each direction
    nprocs = [1, 3, 4]
    # init theme
    my_theme = themes.DefaultTheme()
    my_theme.lighting = True
    my_theme.show_edges = False
    my_theme.background = "#000000"
    my_theme.window_size = [800, 800]
    # init plotter, set camera
    plotter = pyvista.Plotter(theme=my_theme, off_screen=True)
    factor = 1.25
    plotter.camera.position = (-sizes[0]*0.9*factor, -sizes[1]*1.3*factor, sizes[2]*factor*2.)
    plotter.camera.focal_point = (sizes[0]*0.5, sizes[1]*0.5, sizes[2]*0.25)
    plotter.add_axes()
    if pencil == "x1":
        x1pencil(plotter, sizes, nprocs)
    if pencil == "y1":
        y1pencil(plotter, sizes, nprocs)
    if pencil == "z1":
        z1pencil(plotter, sizes, nprocs)
    if pencil == "x2":
        x2pencil(plotter, sizes, nprocs)
    if pencil == "y2":
        y2pencil(plotter, sizes, nprocs)
    if pencil == "z2":
        z2pencil(plotter, sizes, nprocs)
    plotter.show(screenshot=f"{pencil}pencil.png")
    plotter.close()

if __name__ == "__main__":
    assert(len(sys.argv[1]) == 2)
    main(sys.argv[1])

