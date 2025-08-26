#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
/*
A la compilation, réglage de l'heure et de l'alarme = heure + 30s

Etat "Affichage normal"
- Affichage heure ligne 1
- Affichage date ligne 2
- Appui court sur BT1 : arrête l'alarme
- Appui long sur BT1 : passage état "Affichage alarme"

Etat "Affichage alarme"
- Affiche "Alarme :" en ligne 1
- Affichage heure alarme en ligne 2
- Appui court sur BT1 : retour "Affichage normal"
*/

// Création des objets
RTC_DS3231 rtc;                                             // Objet RTC
LiquidCrystal_I2C lcd(0x27, 16, 2);                         // Adresse I2C, 16 colonnes, 2 lignes
SoftwareSerial mySerial(10, 11);                            // RX, TX (Arduino -> DFPlayer)
DFRobotDFPlayerMini dfplayer;                               // Objet lecteur MP3

static bool alarmTriggered = false;                         // évite de relancer la musique plusieurs fois
bool PresenceDfplayer = true;                               // Teste si MP3 présent

byte bell[8] = {
  B00100,  //   *  
  B01110,  //  *** 
  B01110,  //  *** 
  B01110,  //  *** 
  B11111,  // *****
  B00000,  //      
  B00100,  //   *  
  B00000   //      
};                                                          // Définition du caractère "cloche"

const int BT1 = 5;                                          // Définition de la broche de test
const int BT2 = 4;                                          // Définition de la broche de test

// États possibles de la machine d'état
enum State { HEURE, ALARME };                               // On n'a que 2 modes
State currentState = HEURE;                                 // On est dans HEURE au départ

// Variables pour gestion appui long
unsigned long buttonPressTime = 0;                          // Pour mesurer le temps d'appui
bool buttonHeld = false;                                    // Pour savoir si l'appui est maintenu
const unsigned long LONG_PRESS_TIME = 1000;                 // 1s pour appui long

// Timer d'inactivité (10s max)
unsigned long lastActionTime = 0;                           // Pas utilisé pour l'instant
const unsigned long INACTIVITY_TIMEOUT = 10000;             // 10 secondes

int readButton()                                            // Fonction de lecture des boutons
    {
    if (digitalRead(BT1) == LOW) return 5;                  // Un appui sur BT1 renvoi 5
    if (digitalRead(BT2) == LOW) return 5;                  // Un appui sur BT2 renvoi 5 aussi (pas de diff entre les boutons)
    return 0;                                               // aucun bouton
    }

DateTime compileTime()                                      // Date et heure de la compilation
    {
    return DateTime(F(__DATE__), F(__TIME__));              //
    }
DateTime HeureAlarme(2025, 8, 26, 23, 36, 12);              // Heure d'alarme en variable globale
DateTime HeureCompil;                                       // Heure en variable globale

void setup()                                                // Initialisation des composants
    {
    char bufferTime[16];                                    // "hh:mm:ss"
    pinMode(BT1, INPUT_PULLUP);                             // Config broche
    pinMode(BT2, INPUT_PULLUP);                             //
    
    Wire.begin();                                           // Initialisation du bus I2C

    Serial.begin(9600);
    mySerial.begin(9600);
    while (!Serial);                                        // Attendre que le port série soit prêt

    lcd.begin();                                            // Initialisation de l'afficheur
    lcd.backlight();                                        // Active le rétroéclairage
    lcd.createChar(0, bell);                                // Charger le caractère cloche en slot 0
    lcd.clear();                                            //
    
    Serial.println("Initialisation du RTC...");
    if (!rtc.begin())                                       // Init RTC
        {
        Serial.println("Erreur : DS3231 non détecté !");
        lcd.print("DS3231 absent!");
        while (1);                                          // Stop et plante si pas de RTC
        }
    else
        Serial.println("RTC initialisé correctement.");
    if (rtc.lostPower())                                    // Pour le réglage de l'heure à la compilation
        {
        Serial.println("RTC désactivé, réglage de l'heure...");
        HeureCompil = compileTime();
        rtc.adjust(HeureCompil);                            // Régle l'heure à la compilation si besoin
        }
    HeureCompil = rtc.now(); 
    HeureAlarme = rtc.now() + TimeSpan(30);                // DateTime + 30 secondes (TimeSpan(0,0,30,0) pour 30 minutes)
    sprintf(bufferTime, "%02d:%02d:%02d", HeureCompil.hour(), HeureCompil.minute(), HeureCompil.second()); // Préparation de la ligne à afficher
    Serial.println(bufferTime);                             // Affiche heure
    sprintf(bufferTime, "%02d:%02d:%02d", HeureAlarme.hour(), HeureAlarme.minute(), HeureAlarme.second()); // Préparation de la ligne à afficher
    Serial.println(bufferTime);                             // Affiche heure

    Serial.println("Initialisation du DFPlayer...");
    if (!dfplayer.begin(mySerial))                          // Initialisation DFPlayer
        {
        lcd.clear();                                        // Efface LCD
        lcd.print("DFPlayer ERROR");                        // Message d'erreur si pas de MP3
        delay(1000);                                        //
        PresenceDfplayer = false;                           // Variable pour indique l'absence de MP3
        }
    else
        Serial.println("DFPlayer initialisé correctement.");
    dfplayer.volume(10);                                    // Volume entre 0 et 30
    //dfplayer.EQ(DFPLAYER_EQ_NORMAL);                        //
  
    lcd.clear();                                            // Efface LCD
    }

void loop()
    {
    DateTime now = rtc.now();                               // Lecture de l'heure du RTC
    char bufferTime[16];                                    // "hh:mm:ss"
    char bufferDate[16];                                    // "dd/mm/yyyy"

    int btn = readButton();                                 // Lecture boutons
    unsigned long nowb = millis();                          // ref temps pour lecture boutons

    if (btn == 5)                                           // Gestion appui long sur un des boutons
        {                                                   // SELECT pressé
        if (buttonPressTime == 0)                           // début de l'appui
            { 
            buttonPressTime = nowb;                         // Heure de début d'appui avec compteur interne arduino
            buttonHeld = false;                             // Pour la détection du relachement bouton
            }
        else
          if (!buttonHeld && (nowb - buttonPressTime > LONG_PRESS_TIME)) // Si l'appui dure plus d'une seconde
              {
              buttonHeld = true;                            // appui long validé
              if (currentState == HEURE)                    // Si on était en état HEURE
                  {
                  currentState = ALARME;                    // On va dans ALARME
                  lcd.clear();                              // Efface LCD
                  }
              }
        }
    else
        {
        if (buttonPressTime != 0 && !buttonHeld)            // ici = appui court
            {
            if (currentState != HEURE && btn == 0)          // Si on n'était pas dans HEURE
                {
                currentState = HEURE;                       // On retourne dans Heure
                lcd.clear();
                }
            dfplayer.stop();                                // arrêt de la piste MP3 en cours
            alarmTriggered = false;                         // Réinitialise le flag alarme
            lcd.clear();                                    // Efface LCD
            lcd.backlight();
            }
        if (btn != 0)                                       // toute action sur un bouton remet le timer à zéro
            {
            lastActionTime = nowb;
            }
        buttonPressTime = 0;
        buttonHeld = false;
        }

    switch (currentState)                                   // Machine à états
        {
        case HEURE:                                         // Quand on est en mode HEURE : on fait ça
            {
            // Format de l'heure : HH:MM:SS
            sprintf(bufferTime, "%02d:%02d:%02d    ", now.hour(), now.minute(), now.second()); // Préparation de la ligne à afficher

            // Format de la date : JJ/MM/AAAA
            int annee = now.year() - 2000;                  // ou now.year() % 100;
            sprintf(bufferDate, "%02d/%02d/%02d", now.day(), now.month(), annee); // Préparation de la ligne à afficher

            // Affichage sur le LCD
            lcd.setCursor(0, 0);                            // Ligne 1
            lcd.print("   ");                               // Début de ligne
            lcd.print(bufferTime);                          // Affiche heure
            lcd.write((uint8_t)0);                          // Affiche la cloche.
            
            lcd.setCursor(0, 1);                            // Ligne 2
            lcd.print("   ");                               // Début de ligne
            lcd.print(bufferDate);                          // Affiche date
            }
            break;        
        case ALARME:                                        // Quand on est en mode ALARME : on fait ça
            {
            // Format de l'heure : HH:MM:SS
            sprintf(bufferTime, "%02d:%02d:%02d ", HeureAlarme.hour(), HeureAlarme.minute(), HeureAlarme.second());  // Préparation de la ligne à afficher

            // Affichage sur le LCD
            lcd.setCursor(0, 0);                            // Ligne 1
            lcd.print("Alarme:");                           //
            
            lcd.setCursor(0, 1);                            // Ligne 2
            lcd.print("Heure: ");                           // Avec l'heure de l'alarme
            lcd.print(bufferTime);                          // 
            }
            break;
        }

    // Si on est arrivé à l'heure d'alarme
    uint32_t current = now.hour() * 3600 + now.minute() * 60 + now.second();
    uint32_t target = HeureAlarme.hour() * 3600 + HeureAlarme.minute() * 60 + HeureAlarme.second();
    //Serial.println(current);
    //Serial.println(target);
    if (current == target && !alarmTriggered)
    //if (now.hour() == HeureAlarme.hour() && now.minute() == HeureAlarme.minute() && now.second() == HeureAlarme.second())
        {
        alarmTriggered = true;                              // Empêche de relancer chaque seconde
        Serial.println("Alarme en cours");                  // Déclenchement de l'alarme (sur liaison série)
        lcd.clear();                                        // Efface LCD
        lcd.setCursor(0, 0);                                // Affiche sur LCD
        lcd.print(">>> ALARME <<<");                        //
        lcd.setCursor(0, 1);                                //
        lcd.print("Il est l'heure");                        //
        
        if (PresenceDfplayer == true)                       // Si le lecteur a été détecté
            {
            Serial.println("Déclenchement MP3");            //
            dfplayer.play(1);                               // Joue le fichier 001.mp3
            delay(5000);                                    // Petite tempo pour ne pas rerentrer dans la boucle
            }
        else
            delay(5000);                                    // Si le lecteur est absent, on laisse afficher pendant 5 secondes
        }
    if ((now.second() %2) == 1 && alarmTriggered == true)
        lcd.noBacklight();
    else
        lcd.backlight();
    }
