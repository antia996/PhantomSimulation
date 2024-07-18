# PhantomSimulation
Simulación de neutrones frente a una lámina de plomo. Detrás de esta lámina de plomo se encuentran dos phantoms de PLA y, posteriormente un detector de NaI


------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- PHANTOM SIMULATION ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

**ActionInitialization.cc**

**DetectorConstruction.cc:**
Se definen todos los materiales: vacío, aire, teflón, polietileno y PLA.
Se crean los sólidos, volúmenes lógicos y físicos:
->polietilenoBox: caja de polietileno de 5 cm de espesor que modera los neutrones de la fuente de neutrones rápidos
->colimador
->polietilenoBigBox, solidInnerBox, solidUpperBox, solidDownBox: cajas de polietileno que rodean los volúmenes del moderador y colimador
->plomoBox: lámina de plomo de 2.5 cm de espesor. Está recortada por uno de los laterales 
->phantomBox: phantom de PLA con 3 orificios circulares
->phantom2Box: phantom de teflón con 3 orificios rectangulares

**EventAction.cc**
Se definen las variables *fEnergyDetector* y *fTrackLDetector* que son la energía y la longitud recorrida por las partículas en el detector, respectivamente. 
Se rellenan los histogramas 1D en el caso de que la partícula encontrada sea un neutrón (código = 2112)


**PrimaryGeneratorAction.cc**
Se define la posición, desde la que se lanzan las partículas, así como la dirección, velocidad y/o tipo.
Las partículas, por defecto, se lanzan a 80 cm del centro del World Volume, a 2.5 MeV, hacia el frente en una vetana de direcciones que tiene una amplitud de 16 grados. 


**RunAction.cc**
Se crean los histogramas en 1D de las energías y distancia recorrida en el detector por las partículas, así como el histograma en 2D de las posiciones X e Y de las partículas en el detector.

**SteppingAction.cc**
Step a step va filtrando las partículas para solo completar los histogramas con los neutrones. 
