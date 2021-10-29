# FARE UNA BUILD

Ogni volta che cambia codice (.h o .c) va ricreata la build e rilanciato il programma.

Per fare la build, muoversi nella cartella parent /profinet e lanciare il comando di build.

La build si troverà nella sottocartella /profinet/build

```
cd /home/automation-dell/profinet
cmake --build build --target install
```

Se si vuole cambiare la frequenza di aggiornamento di lettura/scrittura dei file (in pratica, accorci o allunghi il ciclo di esecuzione del main), agire su simple_app_common.h
- APP_TICKS_READ_FILES
- APP_TICKS_UPDATE_DATA

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
sudo /home/automation-dell/profinet/build/pn_dev -vvvvv -i enp7s0 -a /home/automation-dell/profinet/plexus_outputs_as_profinet_inputs.txt -b /home/automation-dell/profinet/heartbeat.txt
```

## Reset vecchia configurazione

Se cambia l'IP o qualche settaggio di rete, va resettata la cache prima di rilanciare di nuovo l'eseguibile.

Questa cosa si fa col parametro "r".

Di seguito, un esempio di reset + settaggio IP e submask per l'interfaccia da usare.

```
sudo /home/automation-dell/profinet/build/pn_dev -r
sudo ifconfig enp7s0 169.254.206.121 netmask 255.255.0.0 up
```


## TESTARE FUNZIONAMENTO I/O

Impostare valori di input

```
echo 0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 > /home/automation-dell/profinet/plexus_outputs_as_profinet_inputs.txt
```
