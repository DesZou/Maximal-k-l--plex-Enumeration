
import os

if __name__ == '__main__':
    data_dir = './data/SNAP_modified/'
    output_dir = './outputs/'
    bin_dir = './build/bin'
    algo_exe = './build/bin/algo_on_server'
    algo2_exe = './build/bin/algo2_on_server'
    bf_exe = './build/bin/bf_on_server'
    bf2_exe = './build/bin/bf2_on_server'
    output_bound = 300
    running_time_bound = 300 # seconds

    for filename in os.listdir(data_dir):
        for k, l in [(2, 2), (2, 3), (3, 3)]:
            for exe in [algo_exe, algo2_exe, bf_exe, bf2_exe]:
                output_filename = exe[12:-6] + "k{}l{}b{}_".format(k, l, output_bound) + filename
                command = "{} {} {} {} {} {} > {}{}".format(
                    exe,
                    data_dir + filename,
                    output_bound,
                    running_time_bound,
                    k,
                    l,
                    output_dir,
                    output_filename)
                print("Running " + command + " ...")
                os.system(command)
                print("Done")
