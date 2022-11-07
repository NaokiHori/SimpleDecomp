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

def nopencil(plotter, sizes):
    for k in range(sizes[2]):
        for j in range(sizes[1]):
            for i in range(sizes[0]):
                box = pyvista.Box(bounds=get_bounds(i, j, k))
                plotter.add_mesh(box, color="#AAAAAA")

def main():
    sizes = [24, 18, 12]
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
    nopencil(plotter, sizes)

    plotter.show(screenshot="nopencil.png")
    plotter.close()

if __name__ == "__main__":
    main()

