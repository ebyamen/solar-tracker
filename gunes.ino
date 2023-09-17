//Servo motor kütüphanesi
#include <Servo.h>

//Değişken tanımlamaları

int mode = 0;
int axe  = 0;            
int butonDurumu1 = 0;    
int butonDurumu2 = 0;   
int prevButonDurumu1 = 0;
int prevButonDurumu2 = 0; 
 
int ldr_sag_ust= 0;                                      
int ldr_sol_ust = 1;                                        
int ldr_sag_alt = 2;                                 
int ldr_sol_alt = 3;                                
int sol_ust = 0;
int sag_ust = 0; 
int sol_alt = 0;
int sag_alt = 0;

//Servo motorları tanımlıyoruz.
Servo servo_updown;
Servo servo_rightleft;

int esik_degeri=10;           // Ölçüm hassasiyeti

void setup()  //Başlangıç Fonksiyonu
{
  Serial.begin(9600);                                //Seri Bağlantı  //9600 Bant hızında haberleşme başlatıldı
  Serial.println("Veri Temizlendi");                    
  Serial.println("Label,t,V,I,P,Mod");   //define the column headings (PLX-DAQ command)

  pinMode(12, INPUT);              //Mod değiştirme butonu
  pinMode(11, INPUT);              //Servo motor değiştirme butonu
  pinMode(A4, INPUT);              //Potansiyometre girişi
  
  servo_updown.attach(5);             //Servo motor SG90 (yukarı-aşağı hareket eden)
  servo_rightleft.attach(6);          //Servo motor MG995(sağa sola hareket ettiren)
}

void loop() //Döngü Başlıyor
{
    char Mode;
    float volt = analogRead(A5)*5.0/1023;
    float voltage = 2*volt;                //  V=(R1/R1+R2)*V / R1=R2=10Ohm  => v=2*v)
    float akim = voltage/20;               //  I=V/(R1+R2) 
    float power = voltage*akim;
    Serial.print("Veri,Zaman,");
    Serial.print(voltage);    
    Serial.print(",");
    Serial.print(akim);    
    Serial.print(",");
    Serial.print(power);  
    Serial.print(",");
    
    
  butonDurumu1 = digitalRead(12);
  if (butonDurumu1 != prevButonDurumu1) {
    if (butonDurumu1 == HIGH) {  
      if (mode == 1) {
        mode = 0;
      } else {
         mode = 1;
      }
    }
  }
  prevButonDurumu1 = butonDurumu1;
  delay(50); 
  
  if (mode == 0) {
    Mode= 'M';
    Serial.println(Mode);   
          manuel_gunes_takip();
  } else { 
    Mode = 'A';
    Serial.println(Mode);      
          otomatik_gunes_takip();
    } 
}

void otomatik_gunes_takip(){ //Otomatik Takip Fonksiyonu
  
     //Herbir LDR'nin değeri kaydediliyor.
     sag_ust= analogRead(ldr_sag_ust);        
     sol_ust= analogRead(ldr_sol_ust);        
     sag_alt= analogRead(ldr_sag_alt);        
     sol_alt= analogRead(ldr_sol_alt);        

   //Ortalama Değerler
     int ort_ust = (sag_ust + sol_ust) / 2;     
     int ort_alt = (sag_alt + sol_alt) / 2;     
     int ort_sol = (sol_ust + sol_alt) / 2;    
     int ort_sag = (sag_ust + sag_alt) / 2;   
   
 
     int diff1 = ort_alt - ort_ust;    //  
     int diff2 = ort_sag - ort_sol;    
    
    //Sağa ve Sola Hareket
     
      if (abs(diff2) >= esik_degeri){        // esik_degerinden büyükse hareket gerçekleşicek.
       if (diff2 > 0) {
        if (servo_rightleft.read() < 180) {
          servo_rightleft.write((servo_updown.read() + 2));
        }
      }
      if (diff2 <  0) {
        if (servo_rightleft.read() > 0) {
          servo_rightleft.write((servo_updown.read() - 2));
        }
      }
    }
             
      //Yukarı ve Aşağıya Hareket

      if (abs(diff1) >= esik_degeri){    //esik_degerinden büyükse hareket gerçekleşicek.
       if (diff1 > 0) {
        if (servo_updown.read() < 180) {
          servo_updown.write((servo_rightleft.read() - 2));
        }
      }
      if (diff1 <  0) {
        if (servo_updown.read() > 0) {
          servo_updown.write((servo_rightleft.read() + 2));
        }
      }
    }       
 }  


void manuel_gunes_takip(){    //Manuel Kontrol İçin Fonksiyon
  butonDurumu2= digitalRead(13);
  if (butonDurumu2 != prevButonDurumu2) {
    if (butonDurumu2 == HIGH) {
      if (axe == 1) {
        axe = 0;
      } else {
        axe = 1;
      }
    }
  }
  prevButonDurumu2= butonDurumu2;
  delay(50); //50 ms saniye bekle
  if (axe == 0) {     //Sağa ve Sola Manuel Hareket
    servo_rightleft.write(map(analogRead(A4), 0, 1023, 0, 180));
  } else {            //Yukarı ve Aşağıya Manuel Hareket
    servo_updown.write(map(analogRead(A4), 0, 1023, 0, 180));
  } 
}
