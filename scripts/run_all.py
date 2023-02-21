
import os

if __name__ == '__main__':
    kl_set = [(2, 2)]
    bin_dir = './build/bin'
    data_dir = './data/SNAP_modified'
    output_dir = './outputs'
    output_bound = 300
    runtime_bound = 300

    for data_file in os.listdir(data_dir):
        if not data_file.endswith('.txt'):
            continue
        for k, l in kl_set:
            for prog in os.listdir(bin_dir):
                output_file = "{}_k{}l{}c{}t{}_{}.txt".format(
                    os.path.splitext(data_file)[0],
                    k,
                    l,
                    output_bound,
                    runtime_bound,
                    os.path.splitext(prog)[0]
                )
                command = "{} {} {} {} {} {} > {}".format(
                    os.path.join(bin_dir, prog),
                    os.path.join(data_dir, data_file),
                    output_bound,
                    runtime_bound,
                    k,
                    l,
                    os.path.join(output_dir, output_file)
                )
                print("Running " + command + " ...")
                os.system(command)
                print("Done")
