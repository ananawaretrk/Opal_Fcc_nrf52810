# Trackonomy Radio Certification Firmware
---
This repository is used for radio certification of the hardware listed below. The hardware can be set to a particular frequency using the android app. It support the following radio modes:
1. Transmit
    1. Trasmit Continous Wave at a predefined frequency set by android application
    2. Transmit Modulated Wave at a predefined frequency set by android application
2. Receive
    1. Receive at a predefined frequency set by android application

There are in general four modes of operation:
1. TX continous
2. RX continuous
3. TX and sleep
4. RX and sleep
5. TX, sleep and RX (not supported by Onyx LTE-M/NB-IOT radios)

## Supported Hardware:
--- 
1. Garnet V2.3.7
2. Jade V2.3.0
3. Onyx V3.3.x
4. Onyx V2.5.5
5. Opal V1.2.X
6. Rim  V1.2.x

# Links
--- 
1. [Android App Arguments for Garnet and Jade](https://docs.google.com/spreadsheets/d/1HP615XFYZt2STN2pi8A0y3Usc1Xs2-M48XEhPg5eEbU/edit?usp=sharing) - Details arguments vs behaviour of the radio. 
2. [Android App Arguments for Onyx](https://docs.google.com/spreadsheets/d/1EHz_Uu7MBMxNY85WKWDVzoMsf-hqcWa8d0k8Y2omyeA/edit?usp=sharing) - Details arguments vs behaviour of the radio.


## Firmware:
---
1. Jade     :   fcc_jade_1
2. Onyx 2   :   fcc_onyx_2_sm
3. Onyx 3   :   fcc_onyx_3_sm
4. Garnet   :   fcc_garnet
5. Opal     :   fcc_opal_1
6. Rim      :   fcc_onyx_2_sm (Update the preprocessor directive at the top of the main to change the configuration from Onyx_2 to Rim V1.2)
