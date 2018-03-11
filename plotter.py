import sys

x_axis_points = sys.argv[1]
input_pipe = sys.argv[2]
output_plot = sys.argv[3]

plot_data = [0.0 for x in range(int(x_axis_points))]

while True:
    print("Opening {}...".format(input_pipe))
    with open(input_pipe) as fifo:
        print("... {} opened".format(input_pipe))
        while True:
            next_value = fifo.readline()
            if len(next_value) == 0:
                print("{} closed. Retrying...".format(input_pipe))
                break
            plot_data = plot_data[1:] + [float(next_value)]
            with open(output_plot, "w") as liveplot:
                for x,y in enumerate(plot_data):
                    liveplot.write("{} {}\n".format(x,y))
            print(next_value)
