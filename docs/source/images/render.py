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
    return (
            xmin + eps, xmax - eps,
            ymin + eps, ymax - eps,
            zmin + eps, zmax - eps
    )

def get_mysize(total, nprocs, myrank):
    return (total + myrank) // nprocs

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

def is_interior(sizes, indices):
    # internal cells which are invisible
    nhalo = 1
    i = indices[0]
    j = indices[1]
    k = indices[2]
    if sizes[2] > 2:
        return i >= nhalo and j >= nhalo and k < sizes[2] - nhalo
    else:
        return False

def get_color(myrank, nprocs):
    colormap = matplotlib.colormaps["gist_rainbow"]
    rgba = colormap(myrank / (nprocs - 1))
    return rgba[:3]

def nopencil(plotter, sizes):
    for k in range(sizes[2]):
        for j in range(sizes[1]):
            for i in range(sizes[0]):
                if is_interior(sizes, (i, j, k)):
                    continue
                box = pyvista.Box(bounds=get_bounds(i, j, k))
                plotter.add_mesh(box, color="#AAAAAA")

def x1pencil(plotter, ndims, sizes, nprocs):
    if 3 == ndims:
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
                    if is_interior(sizes, (i, j, k)):
                        continue
                    rankx = rankxs[i]
                    myrank = ranky * nprocs[2] + rankz
                    box = pyvista.Box(bounds=get_bounds(i, j, k))
                    plotter.add_mesh(box, color=colors[myrank])
    else:
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

def y1pencil(plotter, ndims, sizes, nprocs):
    if 3 == ndims:
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
                    if is_interior(sizes, (i, j, k)):
                        continue
                    rankx = rankxs[i]
                    myrank = rankx * nprocs[2] + rankz
                    box = pyvista.Box(bounds=get_bounds(i, j, k))
                    plotter.add_mesh(box, color=colors[myrank])
    else:
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
                if is_interior(sizes, (i, j, k)):
                    continue
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
                if is_interior(sizes, (i, j, k)):
                    continue
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
                if is_interior(sizes, (i, j, k)):
                    continue
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
                if is_interior(sizes, (i, j, k)):
                    continue
                rankx = rankxs[i]
                myrank = ranky * nprocs[2] + rankx
                box = pyvista.Box(bounds=get_bounds(i, j, k))
                plotter.add_mesh(box, color=colors[myrank])

def main(ndims, pencil):
    sizes = [24, 18, 12] if 3 == ndims else [24, 18, 1]
    # number of processes in each direction
    nprocs = [1, 3, 4] if 3 == ndims else [1, 6]
    # init theme
    my_theme = themes.Theme()
    my_theme.lighting = True
    my_theme.show_edges = False
    my_theme.background = "#000000"
    my_theme.window_size = [800, 800]
    # init plotter
    plotter = pyvista.Plotter(theme=my_theme, off_screen=True)
    # render
    if pencil == "no":
        nopencil(plotter, sizes)
    elif pencil == "x1":
        x1pencil(plotter, ndims, sizes, nprocs)
    elif pencil == "y1":
        y1pencil(plotter, ndims, sizes, nprocs)
    elif pencil == "z1":
        z1pencil(plotter, sizes, nprocs)
    elif pencil == "x2":
        x2pencil(plotter, sizes, nprocs)
    elif pencil == "y2":
        y2pencil(plotter, sizes, nprocs)
    elif pencil == "z2":
        z2pencil(plotter, sizes, nprocs)
    else:
        print("N/A")
        plotter.close()
        return
    # set camera
    plotter.camera.position = (
            - sizes[0] * 1.17,
            - sizes[1] * 1.69,
            + 30.
    )
    plotter.camera.focal_point = (
            + sizes[0] * 0.485,
            + sizes[1] * 0.515,
            + 3.
    )
    # axes
    plotter.add_axes(
            color="#FFFFFF",
            line_width=7,
            viewport=(0.6, 0.0, 1.0, 0.4),
    )
    # save and quit
    ndims = str(ndims)
    plotter.show(screenshot=f"{pencil}pencil_{ndims}d.png")
    plotter.close()

if __name__ == "__main__":
    argv = sys.argv
    assert(2 == len(argv))
    ndims = int(argv[1])
    assert(2 == ndims or 3 == ndims)
    if 2 == ndims:
        pencils = ("no", "x1", "y1", )
    else:
        pencils = ("no", "x1", "y1", "z1", "x2", "y2", "z2", )
    for pencil in pencils:
        main(ndims, pencil)

