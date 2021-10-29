#!/bin/sh

base_file_path_dir=/var/www/plexus_db/pnet_data

if [ ! -d "$base_file_path_dir" ]; then
	sudo mkdir /var/www/plexus_db/pnet_data
	echo "Created folder $base_file_path_dir"
fi

output_filename=plexus_outputs_as_profinet_inputs.txt
input_filename=pnet_outputs_as_plexus_inputs.txt
heartbeat_filename=pnet_heartbeat.txt

if [ ! -f "$base_file_path_dir/$heartbeat_filename" ]; then
	sudo touch "$base_file_path_dir/$heartbeat_filename"
	echo "Created file $base_file_path_dir/$heartbeat_filename"
fi

if [ ! -f "$base_file_path_dir/$output_filename" ]; then
        sudo touch "$base_file_path_dir/$output_filename"
        echo "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0" | sudo tee "$base_file_path_dir/$output_filename"
        echo "Created file $base_file_path_dir/$output_filename"
fi

if [ ! -f "$base_file_path_dir/$input_filename" ]; then
        sudo touch "$base_file_path_dir/$input_filename"
        echo "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0" | sudo tee "$base_file_path_dir/$input_filename"
        echo "Created file $base_file_path_dir/$input_filename"
fi


sudo chmod -R 777 /var/www/plexus_db/pnet_data

base_pnet_executable_path_dir=/var/www/html/plexus/system/loops

cd $base_pnet_executable_path_dir
sudo "$base_pnet_executable_path_dir/pnet" -vvvvv -i enp7s0 -a "$base_file_path_dir/$output_filename" -b "$base_file_path_dir/$heartbeat_filename" -c "$base_file_path_dir/$input_filename"
