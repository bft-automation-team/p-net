# FARE UNA BUILD

Ogni volta che cambia codice (.h o .c) va ricreata la build e rilanciato il programma.

Per fare la build, muoversi nella cartella parent /profinet e lanciare il comando di build.

La build si troverà nella sottocartella /profinet/build

```
cd /home/automation-dell/profinet
cmake --build build --target install
```

## CONFIGURARE LA BUILD

Va fatto solo se necessario, per cambiare i parametri della build stessa.

Per configurare la build, muoversi nella cartella parent /profinet e lanciare il comando di cmake apposito.

Ad esempio, ora è configurata così:

```
cmake -B build -S p-net
```

ma, se necessario, a fini di debug, test su prestazioni Linux, ecc... va rifatta con le flag necessarie (vedi rt-labs docs)

```
cmake -B build -S p-net -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON -DUSE_SCHED_FIFO=ON
```


# RUNNING THE PNET STACK

Per lanciare l'eseguibile buildato, che si trova nella subdir /build/pn_dev, eseguire i seguenti comandi.

Le "v" aumentano il livello di verbosità dell'output, necessario in fase di sviluppo e testing.

La "i" indica l'interfaccia di rete da utilizzare sulla quale aspettarsi una richiesta di connessione del PLC Controller.

Altri parametri sono per gestire input da file e output su file.

```
# sudo /home/automation-dell/profinet/build/pn_dev -vvvvv -i enp7s0
sudo /home/automation-dell/profinet/build/pn_dev -vvvvv -i enp7s0 -b /home/automation-dell/profinet/pnet_button_1.txt -d /home/automation-dell/profinet/pnet_button_2.txt
sudo /home/automation-dell/profinet/build/pn_dev -vvvvv -i enp7s0 -b /home/automation-dell/profinet/pnet_button_1.txt -d /home/automation-dell/profinet/pnet_button_2.txt -x /home/automation-dell/profinet/pnet_digital_inputs_16.txt
```

## Reset vecchia configurazione

Se cambia l'IP o qualche settaggio di rete, va resettata la cache prima di rilanciare di nuovo l'eseguibile.

Questa cosa si fa col parametro "r".

Di seguito, un esempio di reset + settaggio IP e submask per l'interfaccia da usare.

```
sudo /home/automation-dell/profinet/build/pn_dev -r
sudo ifconfig enp2s0 169.254.206.121 netmask 255.255.0.0 up
```


## TESTARE FUNZIONAMENTO I/O

Tenere monitorati gli output (fake LEDs I/O acceso spento)

```
watch -n 0.1 cat pnet_led_1.txt
watch -n 0.1 cat pnet_led_2.txt
```

Impostare valori di input (fake buttons I/O acceso spento)

```
echo 0 > /home/automation-dell/profinet/pnet_button_1.txt
echo 1 > /home/automation-dell/profinet/pnet_button_1.txt
```

Simulare allarme (fake buttons I/O acceso spento)

```
echo 0 > /home/automation-dell/profinet/pnet_button_2.txt
echo 1 > /home/automation-dell/profinet/pnet_button_2.txt
```

Impostare valori digitali di input

```
echo 1010101010101010 > /home/automation-dell/profinet/pnet_digital_inputs_16.txt
```
