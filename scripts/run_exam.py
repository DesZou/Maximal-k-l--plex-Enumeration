
import os

if __name__ == '__main__':
    data_dir = './data/SNAP_modified/'
    output_dir = './outputs/'
    algo_exe = './build/bin/algo_on_server'
    bf_exe = './build/bin/bf_on_server'
    output_bound = 100

    for filename in os.listdir(data_dir):
        for k, l in [(2, 2), (2, 3), (3, 3)]:
            for exe in [algo_exe, bf_exe]:
                output_filename = exe[12:-6] + "k{}l{}b{}_".format(k, l, output_bound) + filename
                command = "{} {} {} {} {} > {}{}".format(
                    exe,
                    data_dir + filename,
                    output_bound,
                    k,
                    l,
                    output_dir,
                    output_filename)
                print("Running " + command + " ...")
                os.system(command)
                print("Done")
