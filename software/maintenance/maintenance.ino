#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
//#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Création des objets
RTC_DS3231 rtc;                                                            // Objet real time clock DS3231
LiquidCrystal_I2C lcd(0x27, 16, 2);                                        // Objet LCD : Adresse I2C = 0x27 = 0100111b, 16 colonnes, 2 lignes
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);                                     // LCD Keypad Shield V1.1 câblage classique
SoftwareSerial mySerial(10, 11);                                           // RX, TX (Pour accès à DFPlayer)
DFRobotDFPlayerMini dfplayer;                                              // Objet dfplayer

bool alarmTriggered = false;                                               // évite de relancer la musique plusieurs fois
bool PresenceDfplayer = true;                                              // Teste si MP3 présent

const int BT1 = 5;                                                         // Définition de la broche de test
const int BT2 = 6;                                                         // Définition de la broche de test

const unsigned long HOLD_TIME = 3000;                                      // 3 secondes
const unsigned long TIMEOUT_MAX = 10000;                                   // 10 secondes max pour tout le test

bool scanI2C(int nb, byte adr1, byte adr2)
    {
    byte error, address;
    int nDevices = 0;                                                      // Pas de périf pour l'instant
	bool resultat1 = false;                                                // Résultats KO au début
	bool resultat2 = false;                                                //
	bool resultat3 = false;                                                //
	bool resultat = false;                                                 //

    for (address = 1; address < 127; address++)                            // Scrute toutes les adresses possibles
        {
        Wire.beginTransmission(address);                                   // Init à l'adresse de boucle
        error = Wire.endTransmission();                                    // Fin de transmission

        if (error == 0)                                                    // Si on à 0, c'est bon
			{
            Serial.print("I2C device trouve a l'adresse 0x");              // Affiche en hexa
            if (address < 16)                                              // Si c'est moins de 16, il faut mettre un 0
                Serial.print("0");                                         //
            Serial.println(address, HEX);                                  // Affiche adresse trouvée
			if (adr1 == address)                                           // Si c'est celle du param1
			    {
                Serial.println("Afficheur LCD détecté");                   // c'est le LCD
			    resultat1 = true;                                          // Résultat bon
				}
			if (adr2 == address)                                           // Si c'est celle du param2
			    {
                Serial.println("RTC détecté");                             // c'est le RTC
			    resultat2 = true;                                          // Résultat bon
				}
            nDevices++;                                                    // Ajoute 1 aux perifs trouvés
            }
        }

    if (nDevices == 0)                                                     // Bilan comptage I2C
        Serial.println("Aucun peripherique I2C trouve.\n");                //
    else
	    {
        Serial.println("Scan termine.\n");                                 // Affichage
		if (nDevices == 2)                                                 // Si j'en ai 2, c'est bon
		    resultat3 = true;                                              //
		}
    if ((resultat1 == true) && (resultat2 == true) && (resultat3 == true)) // Combine les résultats partiels
	    {
		resultat = true;                                                   // Résultat total
        lcd.print("Autotest I2C");                                         //
		}
	return resultat;
	}

bool autotestLCD()                                                         // Fonction autotest LCD    
    {
    bool reponse = true;                                                   // pour attendre une réponse clavier valide
    bool result = false;                                                   // Résultat autotest

    lcd.clear();                                                           // Efface LCD
    lcd.print("LCD AUTOTEST");                                             // Affiche
    delay(1000);                                                           // Tempo

    // Test 1 : Affichage caractères pleins
    lcd.clear();                                                           // Efface
    for (int i = 0; i < 16; i++)                                           // Pour toute la ligne
		lcd.print((char)255);                                              // ligne 1 pleine
    lcd.setCursor(0, 1);                                                   // Saute ligne 2
    for (int i = 0; i < 16; i++)                                           // Pour toute la ligne
		lcd.print((char)255);                                              // ligne 2 pleine
    delay(1000);                                                           // Tempo

    // Test 2 : Affichage de tous les caractères ASCII 32–127
    lcd.clear();                                                           // Efface
    for (int c = 32; c < 128; c++)                                         // Pour tous les caractères affichables
		{
        lcd.print((char)c);                                                // Affiche
        delay(50);                                                         // Tempo
        if ((c - 31) % 16 == 0)                                            // Pour détecter la ligne pleine
			{
            delay(500);                                                    // Tempo
            lcd.clear();                                                   // Affiche
            }
        }

    // Test 3 : Balayage des positions
    lcd.clear();                                                           // Efface
    for (int row = 0; row < 2; row++)                                      // Boucle sur les 2 lignes
		{
        for (int col = 0; col < 16; col++)                                 // Boucle sur les 16 positions
			{
            lcd.setCursor(col, row);                                       // place curseur
            lcd.print("*");                                                // affiche *
            delay(100);                                                    // Tempo
            lcd.setCursor(col, row);                                       // revient à la même place
            lcd.print(" ");                                                // pour effacer l'*
            }
        }

    // Fin du test
    lcd.clear();                                                           // Efface   
    lcd.print("Autotest LCD OK");                                          // Affiche sur LCD
    Serial.println("L'affichage durant l'autotest est t'il conforme ?");   // Question sur la console
    Serial.println("o pour oui, n pour non");                              //
    while(reponse)                                                         // Attente réponse
        {
        if (Serial.available() > 0)                                        // Si on a un caractère reçu
            {
            char c = Serial.read();                                        // lit un caractère
            switch (c)                                                     // Regarde tous les cas
                {
                case 'o':                                                  // Si o
                    result = true;                                         // le test est bon
                    reponse = false;                                       // On quitte la boucle while
                break;

                case 'n':                                                  // Si n
                    result = false;                                        // le test est pas bon
                    reponse = false;                                       // On quitte la boucle
                break;

                default:                                                   // On ignore le caractère
                break;                                                     // On reste dans la boucle
                }
            }
        }
    return result;
    }

// Fonction de test des deux boutons
bool testBoutons() {
    bool testButton1 = false;                                              // Mauvais au départ      
    bool testButton2 = false;                                              //
    unsigned long pressStart1 = 0;                                         // Variable de mesure durée
    unsigned long pressStart2 = 0;                                         //
    unsigned long startTime = millis();                                    // Top départ de la fonction de test

    while (!(testButton1 && testButton2))
        {
        unsigned long now = millis();                                      // Top entrée dans la boucle

        if (now - startTime > TIMEOUT_MAX)
            {
            Serial.println("Timeout test atteint");
            if (!testButton1) Serial.println("Bouton 1 non valide");
            if (!testButton2) Serial.println("Bouton 2 non valide");
            return false;                                                  // test échoué on quitte la fonction
            }

        bool b1 = (digitalRead(BT1) == LOW);                               // Lecture boutons                        
        bool b2 = (digitalRead(BT2) == LOW);                               //

        if (b1 == HIGH)                                                    // --- Gestion bouton 1 ---
            {
            if (pressStart1 == 0)                                          // On n'avait pas encore appuyé
                {
                pressStart1 = now;                                         // Temps début appui
                }
            else
                if (!testButton1 && (now - pressStart1 >= HOLD_TIME))      // Si appui 3s
                    {
                    testButton1 = true;                                    // Bouton OK
                    Serial.println("Bouton 1 OK");                         //
                    }
            }
        else
            {
            pressStart1 = 0;                                               // relâché → reset compteur
            }

        if (b2)                                                            // --- Gestion bouton 2 ---
            {
            if (pressStart2 == 0)                                          // On n'avait pas encore appuyé
                {
                pressStart2 = now;                                         // Temps début appui
                }
            else
                if (!testButton2 && (now - pressStart2 >= HOLD_TIME))      // Si appui 3s
                    {
                    testButton2 = true;                                    // Bouton OK
                    Serial.println("Bouton 2 OK");                         //
                    }
            }
        else
            {
            pressStart2 = 0;                                               // relâché → reset compteur
            }
        }

    Serial.println("Les boutons ont passe le test");
    return true; // test réussi
    }

void setup()
    {
	bool testI2C;                                                          // Etat du test I2C
	bool testRTC;                                                          // Etat RTC
	bool testPlayer;                                                       // Etat lecteur MP3
    bool testLCD;                                                          // Etat afficheur
    bool testButton1 = false;                                              //
    bool testButton2 = false;                                              //

    pinMode(BT1, INPUT_PULLUP);                                            // Config broche
    pinMode(BT2, INPUT_PULLUP);                                            //

    Wire.begin();                                                          // Initialisation du bus I2C
    lcd.begin();                                                           // Initialisation de l'afficheur I2C
    //lcd.begin(16, 2);                                                    // Initialisation de l'afficheur parallèle       

    Serial.begin(9600);                                                    // Liaison USB 9600
    while (!Serial);                                                       // Attendre que le port série soit prêt

    Serial.println("=== Test de mon reveil ===");                          // Affichage
    Serial.println("===> Composants I2C <===");                            //
    Serial.println("Scanner I2C...");                                      //
    testI2C = scanI2C(2,0x27,0x68);                                        // Recherche I2C

    Serial.println("===> test RTC <===");                                  // Affichage
    if (!rtc.begin())                                                      // Init RTC
		{
        Serial.println("Erreur : DS3231 non détecté");                     // Erreur
		testRTC = false;                                                   //
        }
	else
        {
        Serial.println("DS3231 disponible");                               // Bon
		testRTC = true;                                                    //
        }
	
    Serial.println("\n===> test Afficheur <===");                          // Affichage
	lcd.backlight();                                                       // Active le rétroéclairage afficheur I2C
    testLCD = autotestLCD();                                               // Autotest
    if (testLCD == false)                                                  // Affiche résultat
        Serial.println("Erreur : afficheur défectueux");                   //
    else
        Serial.println("Afficheur fonctionnel");                           //


    Serial.println("\n===> test MP3 <===");                                // Affichage
    if (!dfplayer.begin(mySerial))                                         // Autotest
		{
        Serial.println("Erreur : DFPlayer non initialisé");                // Erreur
		testPlayer = false;                                                //
        }
	else
        {
        Serial.println("DFPlayer disponible");                             // Bon
 		testPlayer = true;                                                 //
        }

    Serial.println("\n===> Test boutons <===");
    Serial.println("Appuyer 3s sur bouton 1 (haut)");
    Serial.println("Appuyer 3s sur bouton 2 (bas)");
    testBoutons();

    Serial.println("\nTEST TERMINE");                                      // Tests terminés
    }

void loop()                                                                // Rien en loop
    {
    }




