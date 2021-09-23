# COMANDI UTILI PER TESTARE PROFINET

ifconfig

# BUILD
cd /home/plexus/profinet
## setup build command
cmake -B build -S p-net
## setup build command con parametri aggiuntivi
cmake -B build -S p-net -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON -DUSE_SCHED_FIFO=ON
## build command
cmake --build build --target install


# RUNNING THE PNET STACK
sudo /home/plexus/profinet/build/pn_dev -vvvvv -i enp2s0
sudo /home/plexus/profinet/build/pn_dev -vvvvv -i enp2s0 -b /home/plexus/pnet_button_1.txt -d /home/plexus/pnet_button_2.txt

Reset vecchia configurazione, dopo cambio build ad esempio, o dopo cambio IP.
sudo /home/plexus/profinet/build/pn_dev -r
sudo ifconfig enp2s0 169.254.206.121 netmask 255.255.0.0 up


## TESTARE FUNZIONAMENTO I/O
watch -n 0.1 cat pnet_led_1.txt
watch -n 0.1 cat pnet_led_2.txt

echo 0 > /home/plexus/pnet_button_1.txt
echo 1 > /home/plexus/pnet_button_1.txt
