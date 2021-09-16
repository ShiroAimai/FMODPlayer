#MGD 20/21 Sound Programming - End of term Project 

Sviluppato da ```Nicola Cisternino``` per l'esame del corso Sound Programming del master in Computer Game Development 2020/2021.

Sviluppato usando:

* Visual Studio 2019 : Version 16.9.6
* FMOD CORE API : 2.01.10

## Struttura Progetto

Il progetto ha come obiettivo quello di realizzare un semplice audio player per riprodurre
diversi formati di file audio.

La soluzione contiene 2 progetti:

1. ```FmodWrapper```: una libreria statica che si propone di incapsulare ed esporre 
   determinate funzionalità di FMOD Core API.
2. ```WrapperClient```: App client che utilizza la libreria statica creata per mostrare 
un'interfaccia grafica minimale a console e permette di fornire dei comandi che verranno interpretati
   dalla libreria statica.
   
## Modalità di utilizzo
E' possibile utilizzare l'applicazione aprendo la soluzione VS e avviando l'esecuzione sul progetto
``WrapperClient`` 

### Pannelli

L'applicazione dispone di due pannelli principali:

* **Pannello di riepilogo** : mostra i media caricati, le opzioni settate sul Master Channel Group e lo stato
di tutti i canali a disposizione;

* **Pannello di inserimento comando**: Cliccando la barra spaziatrice dal pannello di riepilogo si accede al pannello comandi,
qui è possibile eseguire un'operazione tra quelle consentite.

## Funzionalità

### Inizializzazione 

Il metodo statico **NCWrapper::Wrapper::Init** si preoccupa di inizializzare il sistema FMOD sottostante,
richiedere e salvare il path assoluto alla directory dove sono contenuti i file audio che si desidera riprodurre e 
il numero di canali che si vuole utilizzare.

### Load (Statico e Streaming)
Nell'applicazione è possibile cliccare i tasti [1] e [2] per caricare un file audio rispettivamente
in modo statico o streaming. Una volta selezionata la funzionalità verrà richiesto di inserire
il nome completo del file nel seguente formato : **nome_file**.**formato_estensione**

### Play e Play Loop
Una volta caricato almeno un file audio sarà possibile metterlo in play nelle modalità
**play one-shot** e **play loop** selezionando i tasti [3] e [4].
Questa funzionalità richiede di specificare l'ID del file audio caricato (visibile nella tabella di riepilogo dedicata ai media)
e l'ID del canale (visibile nella tabella riportante lo stato dei canali attivi). Il prodotto
di questa operazione metterà in riproduzione il file audio desiderato nel canale selezionato. In caso ci sia già un file audio
in riproduzione sul canale selezionato quest'ultimo verrà stoppato e verrà riprodotto il file audio dichiarato nell'ultima operazione.

### Mute/Unmute
E' possibile **mutare** e **unmutare** un canale che ha un media in riproduzione (anche se in pausa). L'operazione
richiede di fornire l'ID di un canale tra quelli a disposizione.

### Pause / Resume
E' possibile **pausare** e **resumare** un canale che ha un media in riproduzione. L'operazione
richiede di fornire l'ID di un canale tra quelli a disposizione.

### Stop
E' possibile **stoppare** un canale che ha un media in riproduzione (anche se in pausa). L'operazione
richiede di fornire l'ID di un canale tra quelli a disposizione.

### Pan (Left | Right)
E' possibile modificare il valore di PAN per un canale che ha un media in riproduzione (anche se in pausa). L'operazione
richiede di fornire l'ID di un canale tra quelli a disposizione e un valore in virgola mobile compreso nell'intervallo [-1,1].

### Volume
E' possibile modificare il valore di Volume per un canale che ha un media in riproduzione (anche se in pausa). L'operazione
richiede di fornire l'ID di un canale tra quelli a disposizione e un valore intero compreso nell'intervallo [0, 100].

### Volume (Globale)
E' possibile modificare il valore di Volume del Master Channel Group, che avrà effetto globalmente su tutti i canali a disposizione. L'operazione richiede di fornire
un valore intero compreso nell'intervallo [0, 100].

### Mute/Unmute (Globale)
E' possibile **mutare** e **unmutare** il Master Channel Group, che avrà effetto globalmente su tutti i canali a disposizione. Il prodotto
di questa operazione muterà o unmuterà tutti i canali a disposizione che hanno un media in riproduzione.

### Pause/Resume (Globale)
E' possibile **mettere pausa** e **rimuovere pausa** il Master Channel Group, che avrà effetto globalmente su tutti i canali a disposizione. Il prodotto
di questa operazione pauserà o resumerà la riproduzione dei media su tutti i canali a disposizione.

### Stop (Globale)
E' possibile **stoppare** il Master Channel Group, che avrà effetto globalmente su tutti i canali a disposizione. Il prodotto
di questa operazione stoppera i media in riproduzione su tutti i canali a disposizione.


